/****************************************************************************
*  Copyright 2017 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_COMPONENT_SW_SDIO == ENABLED
#include "..\..\..\service\service.h"

/*============================ MACROS ========================================*/
//! SDIO  PROTOCOL TIMING CONTRAL
#define SDIO_ONE_CLOCK          (1)                 //!< The number of SDIO clock
#define SDIO_WORK_CLOCK         (13)        
#define SDIO_RESPONSE_TIME      (0x65)              //!< Command response time
#define SDIO_READ_TIME		    (0x1FFF)      
#define SDIO_BUSY_TIME          (0x1FFF)
#define SDIO_CMD                (0)                 //!< Select CMD line
#define SDIO_DATA               (1)                 //!< Select DATA line
#define SDIO_CLK_TIME           (s_wClockPeriod)    //!< SDIO clock counter

#define SHORT_RESPONSE_LENGTH   (6) 
#define LONG_RESPONSE_LENGTH    (17)
#define LAST_BLOK               (1)

#define SDIO_CLK_SYNC           ((uint8_t)0x01)     
#define SDIO_CMD_SYNC           ((uint8_t)0x02)    
#define SDIO_DATA_SYNC          ((uint8_t)0x04)
#define SDIO_SYNC_END           ((uint8_t)0x07)
#define SDIO_SYNC_RESET         ((uint8_t)0x08)

#define SDIO_LOW_SYNC1          ((uint8_t)0x10)
#define SDIO_LOW_SYNC2          ((uint8_t)0x20)
#define SDIO_HIGH_SYNC1         ((uint8_t)0x40)
#define SDIO_HIGH_SYNC2         ((uint8_t)0x80)
#define IS_SDIO_LOW_SYNC        (SDIO_LOW_SYNC1 | SDIO_LOW_SYNC2)
#define IS_SDIO_HIGH_SYNC       (SDIO_HIGH_SYNC1 | SDIO_HIGH_SYNC2)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name sdio return value type
//! @{ 
typedef enum {	
    SDIO_ON_GOING      = 0,    //!< SDIO is transfering 			
    SDIO_CMD_SET       = 1,    //!< Command already send, but no response required
    SDIO_CMD_RESPONSE  = 2,    //!< Command already receive, CRC check passed
    SDIO_CMD_CRCFAIL   = 3,    //!< Command already receive, CRC check failed
    SDIO_CMD_TIMEOUT   = 4,    //!< Command response timeout
    SDIO_DATA_CRCFALL  = 5,    //!< Wirte or read data CRC check failed
    SDIO_DATA_PASSED   = 6,    //!< Wirte or read data and CRC check passed
    SDIO_DATA_TIMEOUT  = 7,    //!< read data timeout
    SDIO_DATA_ERROR    = 8 ,   //!< Write or read data error
} em_sdio_rt_t;		
//! @}
		   
//! \name sdio response type
//! @{ 
typedef enum {
    SDIO_NO_RESPONSE = 0,       
    SDIO_STANDARD_RESPONSE,     //!< Response 48 bits format
    SDIO_EXPAND_RESPONSE,       //!< Response 136 bits format
    SDIO_SYNCHRONOUS_RECEIVE,   //!< Synchronous receive 48 bits response and data
} em_sdio_response_t;
//! @}

//! \name sdio direction type
//! @{ 
typedef enum {
    SDIO_READ = 0,
    SDIO_WRITE,
} em_sdio_direction_t;
//! @}

//! \name sdio block size type
//! @{ 
typedef enum {
    SDIO_1B_BLOCK    = 1,
    SDIO_2B_BLOCK    = 2,
    SDIO_4B_BLOCK    = 4,
    SDIO_8B_BLOCK    = 8,
    SDIO_16B_BLOCK   = 16,
    SDIO_32B_BLOCK   = 32,
    SDIO_64B_BLOCK   = 64,
    SDIO_128B_BLOCK  = 128,
    SDIO_256B_BLOCK  = 256,
    SDIO_512B_BLOCK  = 512,
    SDIO_1024B_BLOCK = 1024,
    SDIO_2048B_BLOCK = 2048
} em_sdio_block_size_t;
//! @}

//! \name sdio config clock
//! @{ 
typedef struct {
    uint8_t    chClockDiv;
} sdio_cfg_t;
//! @}

//! \name sdio command format
//! @{ 
#pragma pack(1)
typedef union {
    uint8_t chCMDFrame[17];
    struct {
        uint8_t Index         : 6;    //!< 0 - 5
        uint8_t TransferBit   : 1;    //!< 6
        uint8_t StartBit      : 1;    //!< 7
        uint8_t chArguments[4];
        uint8_t StopBit       : 1;    //!< 0
        uint8_t CRC7          : 7;    //!< 1 - 7
        uint8_t chReserved[11];
    };
} sdio_cmd_t;        
#pragma pack()
//! @}

/*============================ PROTOTYPES ====================================*/
//! clock pin
extern void SET_SDIO_CLK(void);
extern void CLR_SDIO_CLK(void);
extern bool READ_SDIO_CLK(void);

//! command pin
extern void SET_SDIO_CMD(void);
extern void CLR_SDIO_CMD(void);
extern bool READ_SDIO_CMD(void);

//! data pin
extern void SET_SDIO_DATA(void);
extern void CLR_SDIO_DATA(void);
extern bool READ_SDIO_DATA(void);
 
/*============================ GLOBAL VARIABLES ==============================*/
NO_INIT static sdio_cfg_t s_tSDIOCfg;   //!< Config SDIO speed

NO_INIT static uint8_t s_chSDIOClkSync;  //!< read command and data synchronization

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*! \brief four byte combination a word(big endian)
 *! \param pchDataAddr target data address
 *! \return combination word data
 */     
static uint32_t byte_combination_word(uint8_t *pchDataAddr)
{
    uint32_t wValue;

    wValue = (pchDataAddr[0] << 24) | (pchDataAddr[1] << 16) |
             (pchDataAddr[2] << 8)  |  pchDataAddr[3];

    return wValue;
}


/*! \brief sdio work clock generator
 *! \param hwClockNumbres number of clocks 
 *! \return state machine status
 */   
#define SDIO_WORK_CLK_RESET_FSM()   do {s_tState = START;} while (0)

static fsm_rt_t sdio_work_clk(uint16_t hwClockNumbres)
{
    NO_INIT static uint16_t s_hwClockCount;
    NO_INIT static uint32_t s_wTimeCount;
    NO_INIT static uint32_t s_wClockPeriod;
    static enum {
        START = 0,
        SDIO_CLR_CLK_TIME,      
        SDIO_SET_CLK_TIME,
        SDIO_CLOCK_COUNT,
    } s_tState = START;

    switch (s_tState) {
        case START: 
            s_wTimeCount = 0;
            s_hwClockCount = hwClockNumbres;
            SAFE_ATOM_CODE(
                s_wClockPeriod = s_tSDIOCfg.chClockDiv;
            );
            CLR_SDIO_CLK();
            s_tState = SDIO_CLR_CLK_TIME;
            break;                          
    
        case SDIO_CLR_CLK_TIME:
            if (SDIO_CLK_TIME >= s_wTimeCount++) {
                break;
            }
            s_wTimeCount = 0;
            SET_SDIO_CLK();
            s_tState = SDIO_SET_CLK_TIME; 

        case SDIO_SET_CLK_TIME:
            if (SDIO_CLK_TIME >= s_wTimeCount++) {
                break;
            } 
            s_wTimeCount = 0;
            s_tState = SDIO_CLOCK_COUNT;       
    
        case SDIO_CLOCK_COUNT:
            if (0 == --s_hwClockCount) {
                SDIO_WORK_CLK_RESET_FSM();
                return fsm_rt_cpl;
            }
            CLR_SDIO_CLK();
            s_tState = SDIO_CLR_CLK_TIME;
            break;         
    }

    return fsm_rt_on_going;
}


/*! \brief sdio work clock generator
 *! \param none
 *! \return state machine status
 */   
#define SDIO_WORK_SYNC_RESET_FSM()    do {s_tState = SDIO_SYNC_START;} while (0)

static fsm_rt_t sdio_work_sync(void) 
{
    static uint8_t s_chClockCount = 0;
    static enum {
        SDIO_SYNC_START = 0,
        SDIO_LOW_SYNC,      
        SDIO_HIGH_SYNC,
        SDIO_WORK_CLK,
        SDIO_WAIT_SYNC,
    } s_tState = SDIO_SYNC_START;

    switch (s_tState) {
        case SDIO_SYNC_START:
            if (!(SDIO_CLK_SYNC & s_chSDIOClkSync)) {   //check synchronization
                break;
            }
            CLR_SDIO_CLK();
            s_tState = SDIO_LOW_SYNC;
            //break;

        case SDIO_LOW_SYNC:
            if (IS_SDIO_LOW_SYNC != (s_chSDIOClkSync & IS_SDIO_LOW_SYNC)) {
                break;
            }         
            SET_SDIO_CLK();
            if (SDIO_SYNC_RESET & s_chSDIOClkSync) {
                SDIO_WORK_SYNC_RESET_FSM();
                break;
            }
            s_tState = SDIO_HIGH_SYNC; 

        case SDIO_HIGH_SYNC:
            if (IS_SDIO_HIGH_SYNC != (s_chSDIOClkSync & IS_SDIO_HIGH_SYNC)) {
                break;
            }
            s_chSDIOClkSync &= ~(IS_SDIO_LOW_SYNC | IS_SDIO_HIGH_SYNC); 
            if (SDIO_SYNC_END != (SDIO_SYNC_END & s_chSDIOClkSync)) {
                SDIO_WORK_SYNC_RESET_FSM();
                break;
            }
            s_tState = SDIO_WAIT_SYNC;
            //break;

        case SDIO_WAIT_SYNC:
            SDIO_WORK_SYNC_RESET_FSM();
            if (SDIO_WORK_CLOCK == ++s_chClockCount) {
                s_chClockCount = 0;
                return fsm_rt_cpl;
            }         
            break;
    }
    
    return fsm_rt_on_going;
}


/*! \brief sdio command response synchronization with read data
 *! \param none
 *! \return state machine status
 */   
#define SDIO_CMD_SYNC_RESET_FSM()    do {s_tState = START;} while (0)

static fsm_rt_t sdio_cmd_sync(void)
{
    static enum {
        START = 0,
        CHECK_LOW_LEVEL,
        CHECK_HIGH_LEVEL,
    } s_tState = START;

    switch (s_tState) {
        case START:
            s_tState = CHECK_LOW_LEVEL;
            //break;

        case CHECK_LOW_LEVEL:
            if (READ_SDIO_CLK()) {
                break;
            }
            s_chSDIOClkSync |=  SDIO_LOW_SYNC1;
            s_tState = CHECK_HIGH_LEVEL;
            
        case CHECK_HIGH_LEVEL:
            if (!READ_SDIO_CLK()) {
                break;
            }
            s_chSDIOClkSync |=  SDIO_HIGH_SYNC1;
            SDIO_CMD_SYNC_RESET_FSM();
            return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}


/*! \brief sdio read data synchronization with command response 
 *! \param none
 *! \return state machine status
 */  
#define SDIO_DATA_SYNC_RESET_FSM()    do {s_tState = START;} while (0)

static fsm_rt_t sdio_data_sync(void)
{
    static enum {
        START = 0,
        CHECK_LOW_LEVEL,
        CHECK_HIGH_LEVEL,
    } s_tState = START;

    switch (s_tState) {
        case START:
            s_tState = CHECK_LOW_LEVEL;
            //break;

        case CHECK_LOW_LEVEL:
            if (READ_SDIO_CLK()) {
                break;
            }
            s_chSDIOClkSync |=  SDIO_LOW_SYNC2;
            s_tState = CHECK_HIGH_LEVEL;
            
        case CHECK_HIGH_LEVEL:
            if (!READ_SDIO_CLK()) {
                break;
            }
            s_chSDIOClkSync |=  SDIO_HIGH_SYNC2;
            SDIO_DATA_SYNC_RESET_FSM();
            return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}


/*! \brief read command line bit data
 *! \param phwReadBit read data bit variable
 *! \param chBitNumbers number of bits
 *! \return state machine status
 */  
#define SDIO_READ_CMD_BIT_FSM()   do {s_tState = START;} while (0)

static fsm_rt_t sdio_read_cmd_bit(uint16_t *phwReadBit, 
                                  uint8_t  chBitNumbers)
{
    NO_INIT static uint8_t s_chReadCount;
    static enum {
        START = 0,
        SDIO_READ_DATA,
        SDIO_READ_CLOCK,
        SDIO_READ_COUNT,
    } s_tState = START;

    if (NULL == phwReadBit) {
        return fsm_rt_err;
    }

    switch (s_tState) {
        case START:
            *phwReadBit = 0;
            s_chReadCount = chBitNumbers;
            if (0 == chBitNumbers) {
                return fsm_rt_cpl;
            }
            s_tState = SDIO_READ_CLOCK;        
            //break;

        case SDIO_READ_CLOCK:
            if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                if (fsm_rt_cpl != sdio_cmd_sync()) {
                    break;
                }
            } else {
                if (fsm_rt_cpl != sdio_work_clk(SDIO_ONE_CLOCK)) {
                    break;
                }
            }
            s_tState = SDIO_READ_DATA;          

        case SDIO_READ_DATA:
            *phwReadBit <<= 1;
            if (READ_SDIO_CMD()) {
                (*phwReadBit)++;
            } 
            s_tState = SDIO_READ_COUNT;
            //break;

        case SDIO_READ_COUNT:
            if (0 == --s_chReadCount) {
                SDIO_READ_CMD_BIT_FSM();
                return fsm_rt_cpl;
            }
            s_tState = SDIO_READ_CLOCK;
            break;
    }

    return fsm_rt_on_going;
}


/*! \brief read data line bit data
 *! \param phwReadBit read data bit variable
 *! \param chBitNumbers number of bits
 *! \return state machine status
 */  
#define SDIO_READ_DATA_BIT_FSM()   do {s_tState = START;} while (0)

static fsm_rt_t sdio_read_data_bit(uint16_t *phwReadBit, 
                                   uint8_t  chBitNumbers) 
{
    NO_INIT static uint8_t s_chReadCount;
    static enum {
        START = 0,
        SDIO_READ_DATA,
        SDIO_READ_CLOCK,
        SDIO_READ_COUNT,
    } s_tState = START;

    if (NULL == phwReadBit) {
        return fsm_rt_err;
    }

    switch (s_tState) {
        case START:
            *phwReadBit = 0;
            s_chReadCount = chBitNumbers;
            if (0 == s_chReadCount) {
                return fsm_rt_err;
            }
            s_tState = SDIO_READ_CLOCK;        
            //break;

        case SDIO_READ_CLOCK:
            if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                if (fsm_rt_cpl != sdio_data_sync()) {
                    break;
                }
            } else {
                if (fsm_rt_cpl != sdio_work_clk(SDIO_ONE_CLOCK)) {
                    break;
                }
            }
            s_tState = SDIO_READ_DATA;         

        case SDIO_READ_DATA:
            *phwReadBit <<= 1;
             if (READ_SDIO_DATA()) {
                (*phwReadBit)++;
             }
            s_tState = SDIO_READ_COUNT;
            //break;

        case SDIO_READ_COUNT:
            if (0 == --s_chReadCount) {
                SDIO_READ_DATA_BIT_FSM();
                return fsm_rt_cpl;
            }
            s_tState = SDIO_READ_CLOCK;
            break;
    }

    return fsm_rt_on_going;
}


/*! \brief sdio write data line or cmd line data bits 
 *! \param hwWriteBit write data bit  variable
 *! \param chBitNumbers the number of bits
 *! \param chDataOrCMD select data line or cmd line
 *! \return state machine status
 */

#define SDIO_WRITE_BYTE_RESET_FSM()     do {s_tState = START;} while (0)

static fsm_rt_t sdio_write_bit( uint16_t hwWriteBit,
                                uint8_t  chBitNumbers,
                                uint8_t  chDataOrCMD)
{
    NO_INIT static uint8_t s_chWriteCount;
    static enum {
        START = 0,
        SDIO_WRITE_BIT,
        SDIO_WRITE_CLOCK,        
        SDIO_WRITE_COUNT,
    } s_tState = START;

    switch (s_tState) {
        case START:
            s_chWriteCount = chBitNumbers;
            if (0 == chBitNumbers) {
                return fsm_rt_err;
            }
            s_tState = SDIO_WRITE_BIT;
            //break;    

        case SDIO_WRITE_BIT:
            if (0x0001 & (hwWriteBit >> (s_chWriteCount - 1 ))) {       //!> MSB
                if (SDIO_DATA == chDataOrCMD) {
                    SET_SDIO_DATA();
                } else {
                    SET_SDIO_CMD();
                }
            } else {
                if (SDIO_DATA == chDataOrCMD) {
                    CLR_SDIO_DATA();
                } else {
                    CLR_SDIO_CMD();
                }
            }
            s_tState = SDIO_WRITE_CLOCK;
            //break;

        case SDIO_WRITE_CLOCK:
            if (fsm_rt_cpl != sdio_work_clk(SDIO_ONE_CLOCK)) {
                break;
            }
            s_tState = SDIO_WRITE_COUNT;

        case SDIO_WRITE_COUNT:
            if (0 == --s_chWriteCount) {
                SDIO_WRITE_BYTE_RESET_FSM();
                return fsm_rt_cpl;
            }
            s_tState = SDIO_WRITE_BIT;
            break;
    }
    
    return fsm_rt_on_going;
}


/*! \brief sdio write cmd 
 *! \param chCMDIndex command index
 *! \param wArgument  command argument
 *! \return state machine status
 */

#define SDIO_SENT_CMD_RESET_FSM()   do {s_tState = START;} while(0)

static fsm_rt_t sdio_write_cmd(uint8_t chCMDIndex, uint32_t wArgument)
{
    NO_INIT static sdio_cmd_t s_tSDIOCMD;
    NO_INIT static uint8_t  s_chWritrCount;
    static enum {
        START = 0,
        SDIO_WRITE_CMD,  
        SDIO_CMD_COUNT,
    } s_tState = START;

    switch (s_tState) {
        case START:
            s_chWritrCount            = 0;
            s_tSDIOCMD.StartBit       = 0;            //!< Always 0
            s_tSDIOCMD.TransferBit    = 1;            //!< SDIO transfer to card
            s_tSDIOCMD.Index          = chCMDIndex;
            //! SDIO data high byte first send, but single byte high bit frist sned
            //! Program already process big and little endian problem,any cpu platform can run
            s_tSDIOCMD.chArguments[0] = (uint8_t)(wArgument >> 24);
            s_tSDIOCMD.chArguments[1] = (uint8_t)(wArgument >> 16);
            s_tSDIOCMD.chArguments[2] = (uint8_t)(wArgument >> 8);
            s_tSDIOCMD.chArguments[3] = (uint8_t)(wArgument);
            s_tSDIOCMD.chCMDFrame[5]  = CRC7_INIT;    //!< Calculate CRC value 
            crc7_stream_check(&s_tSDIOCMD.chCMDFrame[5], s_tSDIOCMD.chCMDFrame, 5);
            s_tSDIOCMD.chCMDFrame[5]  = s_tSDIOCMD.chCMDFrame[5] << 1;
            s_tSDIOCMD.StopBit        = 1;
            s_tState                  = SDIO_WRITE_CMD;
            //break;

        case SDIO_WRITE_CMD:
            if (fsm_rt_cpl != sdio_write_bit(s_tSDIOCMD.chCMDFrame[s_chWritrCount], 8, SDIO_CMD)) {
                break;
            }  
            s_tState = SDIO_CMD_COUNT;

        case SDIO_CMD_COUNT:
            if (6 <= ++s_chWritrCount) {
                SDIO_SENT_CMD_RESET_FSM();
                return fsm_rt_cpl;
            }
            s_tState = SDIO_WRITE_CMD;
            break;
    }

    return fsm_rt_on_going;
} 


/*! \brief sdio read cmd 
 *! \param ptSDIOStatus sdio read command status, refer to em_sdio_rt_t type
 *! \param tResponseExpected response type, refer to em_sdio_response_t type
 *! \param pwResponse save response value 
 *! \return state machine status
 */

#define SDIO_RECEIVE_CMD_RESET_FSM()        do {s_tState = START;} while (0)

static fsm_rt_t sdio_read_cmd( em_sdio_rt_t *ptSDIOStatus, 
                               em_sdio_response_t tResponseExpected, 
                               uint32_t *pwResponse )
{
    NO_INIT static sdio_cmd_t s_tSDIOCMD;
    NO_INIT static uint8_t  s_chReadBitsCount;  //!< The number of read bits
    NO_INIT static uint8_t  s_chReadCount;      //!< The number of read data
    NO_INIT static uint16_t s_hwReadData;       //!< Save read out data 
    NO_INIT static uint8_t  s_chCRCValue;
    static enum {
        START = 0,
        SDIO_WAIT_CMD,
        SDIO_CMD_TIME,
        SDIO_READ_CMD,  
        SDIO_STANDARD_CMD,
        SDIO_EXTEND_CMD,
        SDIO_WAIT_SYNC,
    } s_tState = START;

    if ((NULL == ptSDIOStatus) || (NULL == pwResponse)) {
        return fsm_rt_err;
    }

    switch (s_tState) {
        case START:
            s_chReadCount = 0;
            s_hwReadData  = 0;
            s_chReadBitsCount  = 0;   
            *ptSDIOStatus = SDIO_CMD_RESPONSE;   
            s_tState      = SDIO_WAIT_CMD;
            //break;

        case SDIO_WAIT_CMD:
            if (fsm_rt_cpl != sdio_read_cmd_bit(&s_hwReadData, 1)) {
               break;
            } 
            if (0 == s_hwReadData) {
                s_chReadCount = 0;
                s_chReadBitsCount  = 7;
                s_tState = SDIO_READ_CMD;
                break;
            } else {
                s_chReadCount++;
                s_tState = SDIO_CMD_TIME;
            }

        case SDIO_CMD_TIME:
            if (SDIO_RESPONSE_TIME >= s_chReadCount) { 
                s_tState = SDIO_WAIT_CMD;
                break;
            }
            if (SDIO_CLK_SYNC & s_chSDIOClkSync) {  //!< needed to synchronization
                *ptSDIOStatus = SDIO_DATA_ERROR;
                s_chSDIOClkSync |= SDIO_CMD_SYNC;
                s_tState = SDIO_WAIT_SYNC;
                break;
            }
            *ptSDIOStatus = SDIO_CMD_TIMEOUT;     //!< Command response timeout
            SDIO_RECEIVE_CMD_RESET_FSM();
            return fsm_rt_cpl;

        case SDIO_READ_CMD:
            if (fsm_rt_cpl != sdio_read_cmd_bit(&s_hwReadData, s_chReadBitsCount)) {    
                break;
            }
            s_chReadBitsCount  = 8;
            s_tSDIOCMD.chCMDFrame[s_chReadCount++] = (uint8_t)s_hwReadData;
            if (SDIO_EXPAND_RESPONSE == tResponseExpected) {  
                s_tState = SDIO_EXTEND_CMD;
            } else {
                s_tState = SDIO_STANDARD_CMD;
            }
            break;

        case SDIO_STANDARD_CMD:
            if (SHORT_RESPONSE_LENGTH != s_chReadCount ) {  //!< short response 
                s_tState = SDIO_READ_CMD;
                break;
            }
            s_chCRCValue = CRC7_INIT;   
            crc7_stream_check(&s_chCRCValue, s_tSDIOCMD.chCMDFrame, 5);
            s_chCRCValue = (s_chCRCValue << 1) | 0x01;
            //! CRC + Stop bit check
            if ((s_chCRCValue != s_tSDIOCMD.chCMDFrame[5]) && (0xFF != s_tSDIOCMD.chCMDFrame[5])) {  
                if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                    *ptSDIOStatus = SDIO_DATA_ERROR;
                    s_chSDIOClkSync |= SDIO_CMD_SYNC;
                    s_tState = SDIO_WAIT_SYNC;
                    break;
                }
                *ptSDIOStatus = SDIO_CMD_CRCFAIL;            //!< CRC check failed
            } else {
                 //! SDIO data high byte first receive, but single byte high bit frist receive
                 //! Program already process big and little endian problem,any cpu platform can run
                *pwResponse = byte_combination_word(s_tSDIOCMD.chArguments);                  
            }
            if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                s_chSDIOClkSync |= SDIO_CMD_SYNC;
                s_tState = SDIO_WAIT_SYNC;
                break;
            }
            SDIO_RECEIVE_CMD_RESET_FSM();
            return fsm_rt_cpl;  

        case SDIO_EXTEND_CMD:
            if (LONG_RESPONSE_LENGTH != s_chReadCount) {    //!< long response 
                s_tState = SDIO_READ_CMD;
                break;
            }
            s_chCRCValue = CRC7_INIT;     
            crc7_stream_check(&s_chCRCValue, &s_tSDIOCMD.chCMDFrame[1], 15);
            s_chCRCValue = (s_chCRCValue << 1) | 0x01;
            if (s_chCRCValue != s_tSDIOCMD.chCMDFrame[16]) {
                if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                    *ptSDIOStatus = SDIO_DATA_ERROR; 
                    s_chSDIOClkSync |= SDIO_CMD_SYNC;
                    s_tState = SDIO_WAIT_SYNC;
                    break;
                }
                *ptSDIOStatus = SDIO_CMD_CRCFAIL;            //!< CRC check failed
            } else {
                //! SDIO data high byte first receive, but single byte high bit frist receive
                //! Program already process big and little endian problem,any cpu platform can run   
                pwResponse[0] = byte_combination_word(&s_tSDIOCMD.chCMDFrame[1]);  //!> [127:96]
                pwResponse[1] = byte_combination_word(&s_tSDIOCMD.chCMDFrame[5]);  //!> [95:64]
                pwResponse[2] = byte_combination_word(&s_tSDIOCMD.chCMDFrame[9]);  //!> [63:32]
                pwResponse[3] = byte_combination_word(&s_tSDIOCMD.chCMDFrame[13]); //!> [31:1]
            }
            if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                s_chSDIOClkSync |= SDIO_CMD_SYNC;
                s_tState = SDIO_WAIT_SYNC;
                break;
            }
            SDIO_RECEIVE_CMD_RESET_FSM();
            return fsm_rt_cpl;  
        
        case SDIO_WAIT_SYNC:
            if (fsm_rt_cpl != sdio_read_cmd_bit(&s_hwReadData, 1)) {
               break;
            } 
            if (SDIO_SYNC_RESET & s_chSDIOClkSync) {
                SDIO_RECEIVE_CMD_RESET_FSM();
                return fsm_rt_cpl;  
            }
    }

    return fsm_rt_on_going;
}



/*! \brief sdio write data
 *! \param tBlockSize set block size
 *! \param pchData data buffer pointer
 *! \param hwDataSize data size
 *! \return state machine status
 */

#define SDIO_WRITE_DATA_RESET_FSM()     do {s_tState = START;} while (0)

static fsm_rt_t sdio_write_data( em_sdio_block_size_t tBlockSize, 
                                 uint8_t *pchData, 
                                 uint16_t hwDataSize )
{
    NO_INIT static uint8_t  *s_pchData;
    NO_INIT static uint16_t s_hwCRC16;            //!< Use to crc check
    NO_INIT static uint16_t s_hwRestData;       
    NO_INIT static uint16_t s_hwDataCount;
    NO_INIT static uint16_t s_hwBlockNumbers;
    static enum {
        START = 0,
        SDIO_WRITE_START,
        SDIO_WRITE_DATA,
        SDIO_WRITE_COUNT,
        SDIO_WRITE_CRC,
        SDIO_WRITE_STOP,
        SDIO_CHECK_CRC,
        SDIO_CHECK_BUSY,
        SDIO_WRITE_WAIT,
    } s_tState = START;

    if (NULL == pchData) {
        return fsm_rt_err;
    }

    switch (s_tState) {
        case START:
            s_hwRestData     = 0;
            s_hwDataCount    = 0;
            s_hwBlockNumbers = 0;
            s_hwCRC16        = CRC16_MODEM_INIT;
            s_pchData        = pchData;
            if ((0 == (uint16_t)tBlockSize) && (0 == hwDataSize)) {
                return fsm_rt_err;
            }
            if (0 == hwDataSize % (uint16_t)tBlockSize) {
                s_hwRestData     = (uint16_t)tBlockSize;
                s_hwBlockNumbers = hwDataSize / (uint16_t)tBlockSize;
            } else {
                s_hwRestData     = hwDataSize % (uint16_t)tBlockSize;
                s_hwBlockNumbers = hwDataSize / (uint16_t)tBlockSize + 1;
            }
            CLR_SDIO_DATA();
            s_tState = SDIO_WRITE_START;
            //break

        case SDIO_WRITE_START:
            if (fsm_rt_cpl != sdio_work_clk(SDIO_ONE_CLOCK)) {
                break;
            }
            s_tState = SDIO_WRITE_DATA;

        case SDIO_WRITE_DATA:
            if (fsm_rt_cpl != sdio_write_bit((uint16_t)(*s_pchData), 8, SDIO_DATA)) {
                break;
            } 
            crc16_modem_check(&s_hwCRC16, *s_pchData++);
            s_hwDataCount++;
            s_tState = SDIO_WRITE_COUNT;

        case SDIO_WRITE_COUNT:
            if (LAST_BLOK == s_hwBlockNumbers) {
                if (s_hwRestData == s_hwDataCount) {
                    s_hwDataCount = 0;
                    s_tState = SDIO_WRITE_CRC;
                } else {
                    s_tState = SDIO_WRITE_DATA;
                    break;
                }  
            } else {
                if (tBlockSize == s_hwDataCount) {
                    s_hwDataCount = 0;
                    s_tState = SDIO_WRITE_CRC;
                } else {
                    s_tState = SDIO_WRITE_DATA;
                    break;
                }
            }

        case SDIO_WRITE_CRC:
            if (fsm_rt_cpl != sdio_write_bit(s_hwCRC16, 16, SDIO_DATA)) {
                break;
            } 
            SET_SDIO_DATA();                //!< Write stop bit 
            s_tState = SDIO_WRITE_STOP;

        case SDIO_WRITE_STOP:
            if (fsm_rt_cpl != sdio_work_clk(SDIO_ONE_CLOCK * 3)) {
                break;
            }
            s_tState = SDIO_CHECK_CRC; 

        case SDIO_CHECK_CRC:
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwCRC16, 5)) {
                break;
            }
            //! start bit(always 0) + sdio card return 3 bit crc(only 010 is correct) + stop bit(always 1) = 00101
            if (0x05 != s_hwCRC16) {         
                SDIO_WRITE_DATA_RESET_FSM();
                return fsm_rt_err;
            }
            s_tState = SDIO_CHECK_BUSY;

        case SDIO_CHECK_BUSY:
            //! The s_hwCRC16 saves receive stop bit 
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwCRC16, 1)) {   
                break;
            } 
            if (0 == s_hwCRC16) {             //!< SDIO bus is busy
                s_tState = SDIO_WRITE_WAIT;
                break;
            } 
            if (LAST_BLOK == s_hwBlockNumbers) {
                SDIO_WRITE_DATA_RESET_FSM();
                return fsm_rt_cpl;  
            }
            s_hwBlockNumbers--;
            s_hwDataCount = 0;
            s_hwCRC16     = CRC16_MODEM_INIT;
            CLR_SDIO_DATA();
            s_tState = SDIO_WRITE_START;
            break;
            
        case SDIO_WRITE_WAIT:
            if (SDIO_BUSY_TIME <= ++s_hwDataCount) {
                SDIO_WRITE_DATA_RESET_FSM();
                return fsm_rt_err;
            }
            s_tState = SDIO_CHECK_BUSY;
            break;
    }
    
    return fsm_rt_on_going;

}


/*! \brief sdio read data
 *! \param ptSDIOStatus sdio read data status, refer to em_sdio_rt_t type
 *! \param tBlockSize set block size
 *! \param pchData data buffer pointer
 *! \param hwDataSize data size
 *! \return state machine status
 */

#define SDIO_READ_DATA_REAET_FSM()      do {s_tState = START;} while (0)

static fsm_rt_t sdio_read_data( em_sdio_rt_t  *ptSDIOStatus,
                                em_sdio_block_size_t tBlockSize, 
                                uint8_t    *pchData, 
                                uint16_t   hwDataSize )
{
    NO_INIT static uint8_t  *s_pchData;
    NO_INIT static uint16_t s_hwCRC16;    
    NO_INIT static uint16_t s_hwRestData;
    NO_INIT static uint16_t s_hwReadData;
    NO_INIT static uint16_t s_hwDataCount;
    NO_INIT static uint16_t s_hwBlockNumbers;
    static enum {
        START = 0,
        SDIO_READ_START,
        SDIO_READ_DATA,
        SDIO_READ_COUNT,
        SDIO_READ_CRC,
        SDIO_READ_STOP,
        SDIO_WAIT_SYNC,
    } s_tState = START;

    if ((NULL == ptSDIOStatus) || (NULL == pchData)) {
        return fsm_rt_err;
    }
    
    switch (s_tState) {
        case START:
            s_hwRestData     = 0;
            s_hwDataCount    = 0;
            s_hwBlockNumbers = 0;
            s_hwCRC16        = CRC16_MODEM_INIT;
            *ptSDIOStatus    = SDIO_DATA_PASSED;
            s_pchData        = pchData;
            if ((0 == ((uint16_t)tBlockSize) && (0 == hwDataSize))) {
                return fsm_rt_err;
            }
            if (0 == (hwDataSize % (uint16_t)tBlockSize)) {
                s_hwRestData     = (uint16_t)tBlockSize;
                s_hwBlockNumbers = hwDataSize / (uint16_t)tBlockSize;
            } else {
                s_hwRestData     = hwDataSize % (uint16_t)tBlockSize;
                s_hwBlockNumbers = hwDataSize / (uint16_t)tBlockSize + 1;
            }
            s_tState             = SDIO_READ_START;
            //break;

        case SDIO_READ_START:
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwReadData, 1)) {
                break;
            }
            if (0 == s_hwReadData) {          //!< read start bit(start bit always 0)
                s_hwDataCount = 0;
                s_tState = SDIO_READ_DATA;	
                break;   
            }
            if (SDIO_READ_TIME != ++s_hwDataCount) {
                break; 
            }
            if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                *ptSDIOStatus = SDIO_DATA_ERROR;
                s_chSDIOClkSync |= SDIO_DATA_SYNC;
                s_tState = SDIO_WAIT_SYNC;
                break;
            }
            *ptSDIOStatus = SDIO_DATA_TIMEOUT;
            SDIO_READ_DATA_REAET_FSM();   
            return fsm_rt_cpl;    

        case SDIO_READ_DATA:
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwReadData, 8)) {
                break;
            }
            *s_pchData++ = (uint8_t)s_hwReadData;
            crc16_modem_check(&s_hwCRC16, (uint8_t)s_hwReadData);
            s_hwDataCount++;
            s_tState = SDIO_READ_COUNT;
            
        case SDIO_READ_COUNT:
            if (LAST_BLOK == s_hwBlockNumbers) {
                if (s_hwRestData == s_hwDataCount) {
                    s_hwDataCount = 0;
                    s_tState = SDIO_READ_CRC;
                } else {
                    s_tState = SDIO_READ_DATA;
                    break;
                }
            } else {
                if (tBlockSize == s_hwDataCount) {
                    s_hwDataCount = 0;
                    s_tState = SDIO_READ_CRC;
                } else {
                    s_tState = SDIO_READ_DATA;
                    break;
                }
            }

        case SDIO_READ_CRC:
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwReadData, 16)) {
                break;
            }
            if (s_hwCRC16 != s_hwReadData) {
                if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                    *ptSDIOStatus = SDIO_DATA_ERROR;
                    s_chSDIOClkSync |= SDIO_DATA_SYNC;
                    s_tState = SDIO_WAIT_SYNC;
                    break;
                }
                *ptSDIOStatus = SDIO_DATA_CRCFALL;
                SDIO_READ_DATA_REAET_FSM();
                return fsm_rt_cpl;
            }
            s_tState = SDIO_READ_STOP;

        case SDIO_READ_STOP:
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwReadData, 1)) {
                break;
            }
            if (0 == s_hwReadData) {                //!< receive stop bit(The stop bit always 1)
                if (SDIO_CLK_SYNC & s_chSDIOClkSync) {
                    *ptSDIOStatus = SDIO_DATA_ERROR;
                    s_chSDIOClkSync |= SDIO_DATA_SYNC;
                    s_tState = SDIO_WAIT_SYNC;
                    break;
                }
                *ptSDIOStatus = SDIO_DATA_ERROR;
                SDIO_READ_DATA_REAET_FSM();
                return fsm_rt_cpl;
            }
            if (LAST_BLOK == s_hwBlockNumbers) {
                s_chSDIOClkSync |= SDIO_DATA_SYNC;
                s_tState = SDIO_WAIT_SYNC;
                break;
            }
            s_hwBlockNumbers--;
            s_hwCRC16 = CRC16_MODEM_INIT;
            s_tState  = SDIO_READ_START;
            break;

        case SDIO_WAIT_SYNC:
            if (fsm_rt_cpl != sdio_read_data_bit(&s_hwReadData, 1)) {
               break;
            } 
            if (SDIO_SYNC_RESET & s_chSDIOClkSync) {
                s_chSDIOClkSync = 0;
                SDIO_RECEIVE_CMD_RESET_FSM();
                return fsm_rt_cpl;  
            }
    }

    return fsm_rt_on_going;
}


/*! \brief sdio initialize
 *! \param ptSDIOcfg sdio_cfg_t type pointer
 *! \return state machine status
 */
#define SDIO_INIT_RESET_FSM()   do {s_tState = START;} while (0)

fsm_rt_t sdio_init(sdio_cfg_t *ptSDIOCfg)
{
    static enum {
        START = 0,
        INIT_CLOCK,
    } s_tState = START;

    if (NULL == ptSDIOCfg) {
        return fsm_rt_err;
    }

    switch (s_tState) {
        case START:
            SET_SDIO_DATA();
            SET_SDIO_CMD();
            SET_SDIO_CLK();
            s_chSDIOClkSync = 0;
            SAFE_ATOM_CODE (
                s_tSDIOCfg.chClockDiv = ptSDIOCfg->chClockDiv;
            );
            s_tState = INIT_CLOCK;
            //break;

        case INIT_CLOCK:
            if (fsm_rt_cpl != sdio_work_clk(SDIO_ONE_CLOCK * 74)) {
                break;
            }
            SDIO_INIT_RESET_FSM();
            return fsm_rt_cpl;
    }
    
    return fsm_rt_on_going;
}


/*! \brief sdio change clock frequency
 *! \param chClockDiv set sdio clock frequency
 *! \retval none
 */
void sdio_change_clock(uint8_t chClockDiv) 
{
    SAFE_ATOM_CODE (
        s_tSDIOCfg.chClockDiv = chClockDiv;
    );
}   


/*! \brief sdio command transfer
 *! \param chCMDIndex command index
 *! \param wArgument  command argument
 *! \param tResponseExpected response type, refer to em_sdio_response_t type
 *! \param pwResponse save response value 
 *! \return sdio state, refer to em_sdio_rt_t type
 */

#define SDIO_CMD_TRANSFER_RESET_FSM()   do {s_tState = START;} while (0)

em_sdio_rt_t sdio_cmd_transfer( uint8_t  chCMDIndex, 
                             uint32_t wArgument, 
                             em_sdio_response_t tResponseExpected, 
                             uint32_t *pwResponse )
{
    NO_INIT static em_sdio_rt_t s_tSDIOStatus;
    static enum {
        START = 0,
        SDIO_WRITE_CMD,         //!< SDIO cmd bus sent command
        SDIO_READ_CMD,          //!< Receive command response
        SDIO_WORK_CLK,          //!< SDIO card work needed clk
    } s_tState = START; 

    if (fsm_rt_cpl == sdio_work_sync()) {
        s_chSDIOClkSync |= SDIO_SYNC_RESET;
    }

    switch (s_tState) {
        case START:
            s_tState = SDIO_WRITE_CMD;
             //break;                   

        case SDIO_WRITE_CMD:
            if (fsm_rt_cpl != sdio_write_cmd(chCMDIndex, wArgument)) {
                break;
            }
            if (SDIO_NO_RESPONSE == tResponseExpected) {
                s_tState = SDIO_WORK_CLK;
                break;
            } else if (SDIO_SYNCHRONOUS_RECEIVE == tResponseExpected){
                s_chSDIOClkSync |= SDIO_CLK_SYNC;
            }
            s_tState = SDIO_READ_CMD;
            
        case SDIO_READ_CMD:
            if (fsm_rt_cpl != sdio_read_cmd(&s_tSDIOStatus, tResponseExpected, pwResponse)) {
                break;
            }
            s_tState = SDIO_WORK_CLK;

        case SDIO_WORK_CLK:
            if (SDIO_SYNCHRONOUS_RECEIVE != tResponseExpected) {
                if (fsm_rt_cpl != sdio_work_clk(SDIO_WORK_CLOCK)) {
                    break;
                }
            }
            SDIO_CMD_TRANSFER_RESET_FSM();
            if (SDIO_NO_RESPONSE == tResponseExpected) {
                return SDIO_CMD_SET;
            }
            return s_tSDIOStatus; 
    }

    return SDIO_ON_GOING;
}



/*! \brief sdio data transfer 
 *! \param tDirection sdio transfer direction, refer to em_sdio_direction_t type
 *! \param tBlockSize set block size
 *! \param pchData data buffer pointer
 *! \param hwDataSize data size
 *! \return sdio state, refer to em_sdio_rt_t type
 */


#define SDIO_DATA_TRANSFER_RESET_FSM()  do {s_tState = START;} while (0)

em_sdio_rt_t sdio_data_transfer( em_sdio_direction_t  tDirection, 
                              em_sdio_block_size_t tBlockSize, 
                              uint8_t *pchData, 
                              uint16_t hwDataSize ) 
{
    NO_INIT static em_sdio_rt_t s_tSDIOStatus;
    fsm_rt_t tFsmState;
    static enum {
        START = 0,
        SDIO_WRITE_DATA,
        SDIO_READ_DATA,
        SDIO_WORK_CLK,
    } s_tState = START;

    switch (s_tState) {
        case START:
            if (SDIO_WRITE == tDirection) {
                s_tState =  SDIO_WRITE_DATA;
            } else {
                s_tState = SDIO_READ_DATA; 
            }
            break;

        case SDIO_WRITE_DATA:
            tFsmState = sdio_write_data(tBlockSize, pchData, hwDataSize);
            if (fsm_rt_on_going == tFsmState) {
                break;
            } else if (fsm_rt_cpl == tFsmState) {
                s_tSDIOStatus = SDIO_DATA_PASSED;
            } else if (fsm_rt_err == tFsmState) {
                s_tSDIOStatus = SDIO_DATA_ERROR;                
            }
            s_tState = SDIO_WORK_CLK;
           // break;

        case SDIO_WORK_CLK:
            if (fsm_rt_cpl != sdio_work_clk(SDIO_WORK_CLOCK)) {
                break;
            }
            SDIO_DATA_TRANSFER_RESET_FSM();
            return s_tSDIOStatus; 

        case SDIO_READ_DATA:
            if (!(SDIO_CLK_SYNC & s_chSDIOClkSync)) {
                break;
            }
            if (fsm_rt_cpl != sdio_read_data(&s_tSDIOStatus, tBlockSize, pchData, hwDataSize)) {
                break;
            }  
            SDIO_DATA_TRANSFER_RESET_FSM();
            return s_tSDIOStatus; 
    }
    return SDIO_ON_GOING;    
}

#endif

/* EOF */
