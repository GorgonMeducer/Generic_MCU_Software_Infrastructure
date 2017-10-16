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
//! \brief import configurations
#include ".\app_cfg.h"
#include ".\interface.h"
#include "..\..\page\page.h"

#if USE_COMPONENT_MAL_SDF_W25QXXXX == ENABLED

/*============================ MACROS ========================================*/

//! \brief command: read data
#ifndef SDF_CMD_READ
#error no defined command code for reading data: SDF_CMD_READ
#endif

//! \brief command: page programming
#ifndef SDF_CMD_PROG_PAGE
#error no defined command code for page programming : SDF_CMD_PROG_PAGE
#endif

//! \brief command: disable write
#ifndef SDF_CMD_DISABLE_WRITE
#error no defined command code to disable write : SDF_CMD_DISABLE_WRITE
#endif

//! \brief command: read state register
#ifndef SDF_CMD_READ_STATUS_REG
#error no defined command code to read status register : SDF_CMD_READ_STATUS_REG
#endif

//! \brief command: enable write
#ifndef SDF_CMD_ENABLE_WRITE
#error no defined command code to enable write : SDF_CMD_ENABLE_WRITE
#endif

//! \brief command: erase sector
#ifndef SDF_CMD_SECTOR_ERASE
#error no defined command code to erase sector : SDF_CMD_SECTOR_ERASE
#endif

//! \brief command: read unique id
#ifndef SDF_CMD_READ_UNIQUE_ID
#error no defined command code to read serial data flash unique ID: SDF_CMD_READ_UNIQUE_ID
#endif

//! \note serial data flash page size infomation
//! @{
#ifndef SPI_FLASH_SIZE
#   define SPI_FLASH_SIZE           (0x100000ul)
#endif
     
#define SPI_FLASH_PAGE_SIZE         (4096ul)
#define PROGRAM_SIZE                (256ul)
#define SPI_FLASH_PAGE_SIZE_BIT     12
//! @}

#ifndef VERIFY_BUFFER_SIZE
#   define VERIFY_BUFFER_SIZE       (16ul)
#endif

//!< serial device id for A company
#ifndef SDF_DEVICE_ID
#warning no defined serial data flash device id for vendor, default value 0xEF is used.
#define SDF_DEVICE_ID               (0xEF)
#endif
     
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*============================ PROTOTYPES ====================================*/
/*! \brief initialize serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl initialize succeed.
 *! \retval fsm_rt_err initialize failed
 */
static fsm_rt_t sdf_init(mem_t *ptMal, void *ptCFG);

/*! \brief finish serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl serial data flash finish succeed.
 *! \retval fsm_rt_on_going serial data flash finish on-going
 */
static fsm_rt_t sdf_finish(mem_t *ptMal);

/*! \brief get serial data flash module infomation
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return memory infomation
 */
static mem_info_t sdf_info(mem_t *ptMal);

/*! \brief open serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl serial data flash open succeed.
 *! \retval fsm_rt_on_going serial data flash open on-going
 */
static fsm_rt_t sdf_open(mem_t *ptMal);

/*! \brief close serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl serial data flash finish succeed.
 *! \retval fsm_rt_on_going serial data flash finish on-going
 */
static fsm_rt_t sdf_close(mem_t *ptMal);

/*! \brief get serial data flash module status
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return serial data flash status
 */
static em_mem_status_t sdf_status(mem_t *ptMal);

/*! \brief write a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param ptBuffer page bufer 
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_write(mem_t *ptMal, uint32_t wPageAddress, void *ptBuffer);

/*! \brief read specified data
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_read(mem_t *ptMal, uint32_t wPageAddress, uint_fast16_t hwOffset, uint8_t *pchStream, uint_fast16_t hwSize);

/*! \brief verify specified data 
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_verify(mem_t *ptMal, uint32_t wPageAddress, uint_fast16_t hwOffset, uint8_t *pchStream, uint_fast16_t hwSize);

/*! \brief erase a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_erase(mem_t *ptMal, uint32_t wPageAddress);

/*! \brief chip erase
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_chip_erase(mem_t *ptMal);

extern bool SDF_SPI_INIT(void);                                   
extern fsm_rt_t SDF_SPI_OPEN(void);                                   
extern fsm_rt_t SDF_SPI_CLOSE(void);                                  
extern fsm_rt_t SDF_SPI_BYTE_EXCHANGE(uint8_t chByte, uint8_t *pchByte);
extern void SDF_CLR_CS(void);
extern void SDF_SET_CS(void);

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief serial data flash object
const i_mem_t ISDF = {
        .base__i_mcb_t = {
            .Init       = &sdf_init,
            .Finish     = &sdf_finish,
            .Info       = &sdf_info,
            .Open       = &sdf_open,
            .Close      = &sdf_close,
            .GetStatus  = &sdf_status
        },
        .base__i_mem_page_t = {
            .PageWrite  = &sdf_page_write,
            .PageRead   = &sdf_page_read,
            .PageVerify = &sdf_page_verify,
            .PageErase  = &sdf_page_erase,
            .Erase      = &sdf_chip_erase
        },
};

            
INIT_CLASS_OBJ (
        mem_t, 
        SDF,
        {
            .ptMethod = &ISDF
        }
    );

/*============================ LOCAL VARIABLES ===============================*/
//! \brief system state
static volatile em_mem_status_t s_CurrentState = MEM_NOT_READY;


/*============================ IMPLEMENTATION ================================*/

WEAK void SDF_CLR_WP(void){}
WEAK void SDF_SET_WP(void){}
WEAK void SDF_CLR_HOLD(void){}
WEAK void SDF_SET_HOLD(void){}

/******************************************************************************
 *                              Base Function                                 *
 ******************************************************************************/

//! \brief macro for exchanging multiple bytes with spi
#ifndef SDF_SPI_STREAM_EXCHANGE

//! \brief build-in stream exchange function
#define SDF_SPI_STREAM_EXCHANGE(__OUT, __IN, __LEN) \
            fsm_sdf_spi_stream_exchange((__OUT),(__IN),(__LEN))
            
#define SPI_STREAM_RESET_FSM()              \
    do{s_tState = SPI_STREAM_START;}while(false)

/*! \brief spi stream exchange with external interface SDF_SPI_BYTE_EXCHANGE()
 *! 
 *! \param pchOut output stream buffer (could be NULL)
 *! \param pchIn input stream buffer (could be NULL)
 *! \param wLength stream length
 *! 
 *! \retval true FSM should keep running
 *! \retval false FSM complete.
 */
static fsm_rt_t fsm_sdf_spi_stream_exchange(uint8_t *pchOut, uint8_t *pchIn, uint32_t wLength)
{
    static uint8_t *s_pchIn;
    static uint8_t *s_pchOut;
    static uint32_t s_wCounter;
    
    static enum {
        SPI_STREAM_START                = 0,
        SPI_STREAM_WRITE_ONLY,
        SPI_STREAM_READ_ONLY,
        SPI_STREAM_READ_WRITE,
    } s_tState = SPI_STREAM_START;
        
    switch (s_tState) {
        case SPI_STREAM_START:                      //!< FSM start
            //! check parameter
            if ((NULL == pchIn && NULL == pchOut) || (0 == wLength)) {
                return fsm_rt_cpl;                  //!< doing nothing at all
            } else if (NULL == pchIn) {
                //! write only
                s_pchOut = pchOut;                  //!< initialize output pointer
                s_tState = SPI_STREAM_WRITE_ONLY;
            } else if (NULL == pchOut) {
                //! read only
                s_pchIn = pchIn;                    //!< initialize input pointer
                s_tState = SPI_STREAM_READ_ONLY;
            } else {
                //! read & write
                s_pchIn = pchIn; 
                s_pchOut = pchOut;
                
                s_tState = SPI_STREAM_READ_WRITE;
            }
            s_wCounter = wLength;                   //!< initialize counter
            break;
    
        case SPI_STREAM_WRITE_ONLY: {               //!< FSM start
            uint8_t chDumyRead = 0xFF;
            if (fsm_rt_cpl == SDF_SPI_BYTE_EXCHANGE((*s_pchOut), &chDumyRead)) {
                //! success
                s_pchOut++;
                if (0 == --s_wCounter) {
                    SPI_STREAM_RESET_FSM();
                    return fsm_rt_cpl;
                }
            }
            break;
        }
    
        case SPI_STREAM_READ_ONLY: {                //!< FSM start
            uint8_t chDumyWrite = 0xFF;
            if (fsm_rt_cpl == SDF_SPI_BYTE_EXCHANGE(chDumyWrite, s_pchIn)) {
                //! success
                s_pchIn++;
                if (0 == --s_wCounter) {
                    SPI_STREAM_RESET_FSM();
                    return fsm_rt_cpl;
                }
            }
            break;
        }
    
        case SPI_STREAM_READ_WRITE:                 //!< FSM start
            if (fsm_rt_cpl == SDF_SPI_BYTE_EXCHANGE((*s_pchOut), s_pchIn)) {
                //! success
                s_pchIn++;
                s_pchOut++;
                if (0 == --s_wCounter) {
                    SPI_STREAM_RESET_FSM();
                    return fsm_rt_cpl;
                }
            }
            break;
    }

    return fsm_rt_on_going;                         //!< state machine keep running
}


#endif

#define CHECK_DEVICE_RESET_FSM()    	    \
    do{s_tState = CHECK_DEVICE_START;}while(0)

/*! \brief wait device busy flag release
 *! 
 *! \param none
 *! 
 *! \retval fsm_rt_on_going device is busy
 *! \retval fsm_rt_cpl device is free
 */
static fsm_rt_t check_device_state(void)
{    
    static enum {
        CHECK_DEVICE_START                  = 0,
        CHECK_DEVICE_WRITE_COMMAND,
        CHECK_DEVICE_READ_STATUS,
        CHECK_DEVICE_SEND_DISABLE_WRITE,
    } s_tState = CHECK_DEVICE_START;
    
    switch (s_tState) {
        case CHECK_DEVICE_START:                    //!< FSM start
            SDF_CLR_CS();                           //!< set #CS low
            s_tState = CHECK_DEVICE_WRITE_COMMAND;
            //break;
            
        case CHECK_DEVICE_WRITE_COMMAND:            //!< write command
            if (fsm_rt_on_going == 
                    SDF_SPI_BYTE_EXCHANGE(SDF_CMD_READ_STATUS_REG, NULL)) {
                break;
            }
            
            //! complete
            s_tState = CHECK_DEVICE_READ_STATUS;
            //break;
            
        case CHECK_DEVICE_READ_STATUS: {            //!< read status register
                uint8_t chStatus;
                if (fsm_rt_on_going == SDF_SPI_BYTE_EXCHANGE(0, &chStatus)) {
                    break;
                } 

                //! get status
                if (chStatus & BUSY) {          //!< check busy
                    break;
                } else {
                    SDF_SET_CS();               //!< set #CS high
                    //! system is free                
                    if ((chStatus & WEL)) {     //!< check write enable latch
                        //! we should disable write
                        SDF_CLR_CS();           //!< set #CS low
                        s_tState = CHECK_DEVICE_SEND_DISABLE_WRITE;
                    } else {
                        //! device is ready
                        CHECK_DEVICE_RESET_FSM();//!< reset FSM
                        return fsm_rt_cpl;
                    }
                }

            }
            //break;
            
        case CHECK_DEVICE_SEND_DISABLE_WRITE:
            if (!SDF_SPI_BYTE_EXCHANGE(SDF_CMD_DISABLE_WRITE, NULL)) {
                //! complete
                SDF_SET_CS();                           //!< set #CS high       
                CHECK_DEVICE_RESET_FSM();               //!< reset FSM                
                //! warning: do not return false here
            }
            break;
    }

    return fsm_rt_on_going;
}


#define W_CMD_AND_ADDR_RESET_FSM()          \
    do{s_tState = W_CMD_AND_ADDR_START;}while(0)

/*! \brief common routine for write a 8bits command a 24bits address
 *! 
 *! \param chCommand 8bits command
 *! \param wAddress 32bits address
 *! 
 *! \retval fsm_rt_on_going FSM should keep running
 *! \retval fsm_rt_cpl FSM is completed.
 */
static fsm_rt_t write_command_and_address(uint8_t chCommand, uint32_t wAddress)
{
    static uint8_t s_chAddressCounter;
    
    static enum {
        W_CMD_AND_ADDR_START            = 0,
        W_CMD_AND_ADDR_WRITE_CMD,
        W_CMD_AND_ADDR_WRITE_ADDRESS,
    } s_tState = W_CMD_AND_ADDR_START;
    
    switch (s_tState) {
        case W_CMD_AND_ADDR_START:                      //!< FSM start
            s_tState = W_CMD_AND_ADDR_WRITE_CMD;
            //break;
            
        case W_CMD_AND_ADDR_WRITE_CMD:                  //!< write command
            if (fsm_rt_on_going == SDF_SPI_BYTE_EXCHANGE(chCommand, NULL)) {
                break;   
            } 
            s_chAddressCounter = 2;
            //! write command complete
            s_tState = W_CMD_AND_ADDR_WRITE_ADDRESS;
            //break;
            
        case W_CMD_AND_ADDR_WRITE_ADDRESS:              //!< write address
            if (fsm_rt_cpl == SDF_SPI_BYTE_EXCHANGE(((uint8_t *)&wAddress)[s_chAddressCounter], NULL)) {
                if (0 == s_chAddressCounter) {
                    //! write address complete
                    W_CMD_AND_ADDR_RESET_FSM();         //!< reset FSM
                    return fsm_rt_cpl;
                } else {
                    s_chAddressCounter--;
                }
            }
            break;
    }
   
    return fsm_rt_on_going;
}


#define READ_DID_RESET_FSM()                do{s_tState = READ_DID_START;}while(0)

/*! \brief read device ID
 *! 
 *! \param pdwUID device ID
 *! 
 *! \retval fsm_rt_err access failed or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY serial data flash is not ready
 *! \retval fsm_rt_on_going current access is on going.
 */
fsm_rt_t fsm_sdf_read_device_ID(uint16_t *phwDID)
{    
    NO_INIT static uint16_t s_hwDID;

    static enum {
        READ_DID_START                  = 0,
        READ_DID_CHECK_DEVICE,
        READ_DID_WRITE_COMMAND_AND_ADDRESS,
        READ_DID,
    } s_tState = READ_DID_START; 
    
    switch (s_tState) {
        case READ_DID_START:
            if (NULL == phwDID) {
                return fsm_rt_err;
            } 
            
            SAFE_ATOM_CODE(
                //! whether system is initialized
                if (MEM_NOT_READY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return fsm_rt_err;
                } else if (MEM_READY_BUSY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return (fsm_rt_t)GSF_ERR_NOT_READY;
                }     
                //! set current state
                s_CurrentState = MEM_READY_BUSY;            
                
            )                        
            s_tState = READ_DID_CHECK_DEVICE;
            //break;
            
        case READ_DID_CHECK_DEVICE:
            if (fsm_rt_on_going == check_device_state()) {
                break;
            } 
            SDF_CLR_CS();                       //!< clr CS#
            //! flash is free
            s_tState = READ_DID_WRITE_COMMAND_AND_ADDRESS;            
            //break;

        case READ_DID_WRITE_COMMAND_AND_ADDRESS: {
            uint32_t wDumyWrite = 0x00000000;
            if (fsm_rt_cpl == write_command_and_address(SDF_CMD_READ_DEVICE_ID, wDumyWrite)) {
                s_tState = READ_DID;            
            }
            break;    
        }

        case READ_DID:
            if (fsm_rt_cpl == SDF_SPI_STREAM_EXCHANGE(NULL,(uint8_t *)&s_hwDID, 2)) {
                *phwDID = s_hwDID;
                SDF_SET_CS();                       //!< set #CS high
                SAFE_ATOM_CODE(
                    //! set idle state
                    s_CurrentState = MEM_READY_IDLE;                            
                )
                READ_DID_RESET_FSM();           //!< reset FSM
                return fsm_rt_cpl;                  //!< success
            }
            break;     
    }
 
    return fsm_rt_on_going;
}


#define ENABLE_WRITE_RESET_FSM()            do{s_tState = ENABLE_WRITE_START;}while(false)

/*! \brief enable write 
 *! 
 *! \param none
 *! 
 *! \retval true device is busy
 *! \retval false device is free
 */
static fsm_rt_t enable_write(void)
{       
    static enum {
        ENABLE_WRITE_START              = 0,
        ENABLE_WRITE_SEND_COMMAND,
    } s_tState = ENABLE_WRITE_START;
    
    switch (s_tState) {
        case ENABLE_WRITE_START:
            SDF_CLR_CS();                           //!< set #CS low
            s_tState = ENABLE_WRITE_SEND_COMMAND;
            //break;
            
        case ENABLE_WRITE_SEND_COMMAND:
            if (fsm_rt_cpl == SDF_SPI_BYTE_EXCHANGE(SDF_CMD_ENABLE_WRITE, NULL)) {
                //! write command complete
                SDF_SET_CS();                       //!< set #CS high
                ENABLE_WRITE_RESET_FSM();           //!< reset FSM
                return fsm_rt_cpl;
            }
            break;
    }    
    
    return fsm_rt_on_going;
}


#define READ_UID_RESET_FSM()                do {s_tState = READ_UID_START;}while(false)

/*! \brief read unique ID
 *! 
 *! \param pdwUID unique ID
 *! 
 *! \retval fsm_rt_err access failed or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY serial data flash is not ready
 *! \retval fsm_rt_on_going current access is on going.
 */
fsm_rt_t fsm_sdf_read_unique_ID(uint64_t *pdwUID)
{
    NO_INIT static uint64_t s_dwUID;

    static enum {
        READ_UID_START                  = 0,
        READ_UID_WRITE_COMMAND_AND_ADDRESS,
        READ_UID_CHECK_DEVICE,
        READ_UID,
    } s_tState = READ_UID_START;
    
    switch (s_tState) {
        case READ_UID_START:
            if (NULL == pdwUID) {
                return fsm_rt_err;
            } 
            
            SAFE_ATOM_CODE (
                //! whether system is initialized
                if (MEM_NOT_READY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return fsm_rt_err;
                } else if (MEM_READY_BUSY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return (fsm_rt_t)GSF_ERR_NOT_READY;
                }     
                
                s_CurrentState = MEM_READY_BUSY;    //!< set current state
            )
            
            s_tState = READ_UID_CHECK_DEVICE;
            //break;
            
        case READ_UID_CHECK_DEVICE:
            if (fsm_rt_on_going == check_device_state()) {
                break;
            } 
            SDF_CLR_CS();                       //!< clr CS#
            //! flash is free
            s_tState = READ_UID_WRITE_COMMAND_AND_ADDRESS;            
            //break;
    
        case READ_UID_WRITE_COMMAND_AND_ADDRESS: {
            uint32_t wDumyWrite = 0x00000000;
            if (fsm_rt_cpl == write_command_and_address(SDF_CMD_READ_UNIQUE_ID, wDumyWrite)) {
                s_tState = READ_UID;            
            }
            break;
        }
    
        case READ_UID:
            if (fsm_rt_cpl == SDF_SPI_STREAM_EXCHANGE(NULL,(uint8_t *)&s_dwUID, 8)) {
                *pdwUID = s_dwUID;
                SDF_SET_CS();                       //!< set #CS high
                SAFE_ATOM_CODE (
                    //! set idle state
                    s_CurrentState = MEM_READY_IDLE;        
                    
                )
                READ_UID_RESET_FSM();           //!< reset FSM
                return fsm_rt_cpl;                  //!< success
            }
            break;
    }
     
    return fsm_rt_on_going;
}


/******************************************************************************
 *                              Interface                                     *
 ******************************************************************************/




#define PAGE_INIT_RESET_FSM()           do{s_tState = PAGE_INIT_START;}while(0)

/*! \brief initialize serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl initialize succeed.
 *! \retval fsm_rt_err initialize failed
 */
static fsm_rt_t sdf_init(mem_t *ptMal, void *ptCFG)
{
    static enum {
        PAGE_INIT_START         = 0,
        SDF_INIT_SPI_OPEN,
        PAGE_INIT_READ,
    } s_tState = PAGE_INIT_START;

    NO_INIT static istate_t s_tIntState;

    switch (s_tState) {
        case PAGE_INIT_START:
            s_tIntState = GET_GLOBAL_INTERRUPT_STATE();
            DISABLE_GLOBAL_INTERRUPT();

            //! initialize spi interface
            if (!SDF_SPI_INIT()) {
                SET_GLOBAL_INTERRUPT_STATE(s_tIntState);
                return fsm_rt_err;
            }
            s_tState = SDF_INIT_SPI_OPEN;
            //break;

        case SDF_INIT_SPI_OPEN: {
                fsm_rt_t tFSM = SDF_SPI_OPEN();
                if (IS_FSM_ERR(tFSM)) {
                    PAGE_INIT_RESET_FSM();
                    SET_GLOBAL_INTERRUPT_STATE(s_tIntState);
                    return fsm_rt_err;
                } else if (fsm_rt_cpl == tFSM) {
                    SDF_SET_HOLD();                             //!< always high
                    SDF_CLR_WP();                               //!< set protect  
                    s_CurrentState = MEM_READY_IDLE;            //!< set current state
                    SET_GLOBAL_INTERRUPT_STATE(s_tIntState);

                    s_tState = PAGE_INIT_READ;
                } else {
                    break;
                }
            }

        case PAGE_INIT_READ: {
            uint16_t hwID = 0;
            fsm_rt_t tFSM = fsm_sdf_read_device_ID(&hwID);

            if (IS_FSM_ERR(tFSM)) {
                PAGE_INIT_RESET_FSM();
                return fsm_rt_err;                
            } else if (fsm_rt_cpl == tFSM){
                PAGE_INIT_RESET_FSM();
                if (0 == hwID) {
                    return fsm_rt_err;
                } else {
                    return fsm_rt_cpl;
                }
            }
        }
        break;

    }
    
    return fsm_rt_on_going;
}

/*! \brief finish serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl serial data flash finish succeed.
 *! \retval fsm_rt_on_going serial data flash finish on-going
 */
static fsm_rt_t sdf_finish(mem_t *ptMal)
{    
    if (fsm_rt_cpl == sdf_close(ptMal)) {
        SAFE_ATOM_CODE (
            s_CurrentState = MEM_NOT_READY; //!< set idle state
        )
        
        return fsm_rt_cpl;                  //!< success
    } else {
        return fsm_rt_on_going;   
    }    
}

/*! \brief get serial data flash module infomation
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return memory infomation
 */
static mem_info_t sdf_info(mem_t *ptMal)
{
	static const mem_info_t tReturn = {
        .hwProperty = MEM_BLOCK_ACCESS,
        .hwPageSize = SPI_FLASH_PAGE_SIZE,
        .wPageCount = SPI_FLASH_SIZE / SPI_FLASH_PAGE_SIZE,
        .chPageSizeBit = SPI_FLASH_PAGE_SIZE_BIT
    };
    
    return tReturn;
}


/*! \brief open serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl serial data flash open succeed.
 *! \retval fsm_rt_on_going serial data flash open on-going
 */
static fsm_rt_t sdf_open(mem_t *ptMal)
{
    return SDF_SPI_OPEN();  
}

/*! \brief close serial data flash module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl serial data flash finish succeed.
 *! \retval fsm_rt_on_going serial data flash finish on-going
 */
static fsm_rt_t sdf_close(mem_t *ptMal)
{   
    return SDF_SPI_CLOSE();
}

/*! \brief get serial data flash module status
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return serial data flash status
 */
static em_mem_status_t sdf_status(mem_t *ptMal)
{
    em_mem_status_t tResult;
    
    SAFE_ATOM_CODE(
        tResult = s_CurrentState;
    )
        
    return tResult;
}


#define PAGE_PROGRAM_RESET_FSM()              \
    do{s_tState = PAGE_PROGRAM_START;}while(false)

/*! \brief write a single page
 *! 
 *! \param wAddress page address
 *! \param pchStream data buffer
 *! \param hwLength data length (0 means 256)
 *! 
 *! \retval fsm_rt_cpl serial data flash program succeed.
 *! \retval fsm_rt_on_going serial data program finish on-going
 */
static fsm_rt_t sdf_page_program(uint32_t wPageAddress, uint8_t *pchStream)
{    
    static enum {
        PAGE_PROGRAM_START                  = 0,
        PAGE_PROGRAM_WRITE_COMMAND_ADDRESS,
        PAGE_PROGRAM_WRITE_DATA,
        PAGE_PROGRAM_CHECK,
    } s_tState = PAGE_PROGRAM_START;
    
    switch (s_tState) {
        case PAGE_PROGRAM_START:
            if (fsm_rt_on_going == enable_write()) {        //!< enable write
                break;
            } 
            SDF_SET_WP();                               //!< relase protect
            SDF_CLR_CS();                               //!< clr CS#
            s_tState = PAGE_PROGRAM_WRITE_COMMAND_ADDRESS;
            //break;
            
        case PAGE_PROGRAM_WRITE_COMMAND_ADDRESS:
            if (fsm_rt_on_going == write_command_and_address
                    (SDF_CMD_PROG_PAGE, wPageAddress * PROGRAM_SIZE)) {
                break;
            } 
            s_tState = PAGE_PROGRAM_WRITE_DATA;
            //break;
                        
        case PAGE_PROGRAM_WRITE_DATA:
            if (fsm_rt_on_going == SDF_SPI_STREAM_EXCHANGE(pchStream, NULL, PROGRAM_SIZE)) {
                break;
            }
            //! write complete
            SDF_SET_CS();                               //! set CS high
            SDF_CLR_WP();                               //!< set protect 
            s_tState = PAGE_PROGRAM_CHECK;        

            //break;
                                 
        case PAGE_PROGRAM_CHECK:
            if (fsm_rt_cpl == check_device_state()) {       //!< check state                
                //! flash is free                
                PAGE_PROGRAM_RESET_FSM();
                return fsm_rt_cpl;  
            }
            break;                 
    }    
    
    return fsm_rt_on_going;
}


#define PAGE_WRITE_RESET_FSM()              \
    do{s_tState = PAGE_WRITE_START;}while(0)

/*! \brief write a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param ptBuffer page bufer 
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_write(mem_t *ptMal, uint32_t wPageAddress, void *ptBuffer)
{
    static uint32_t s_wProgramIndex;
    static uint8_t *s_pchStream;    
    
    static enum {
        PAGE_WRITE_START        = 0,
        PAGE_WRITE_WRITE_PAGE,
        PAGE_WRITE_FINALIZE
    } s_tState = PAGE_WRITE_START;
           
    switch (s_tState) {
        case PAGE_WRITE_START:
            if (    (NULL == ptBuffer) 
                ||  (wPageAddress >= (SPI_FLASH_SIZE / SPI_FLASH_PAGE_SIZE))) {
                return fsm_rt_err;
            } 

            SAFE_ATOM_CODE(           
                //! whether system is initialized
                if (MEM_NOT_READY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return fsm_rt_err;
                } else if (MEM_READY_BUSY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return (fsm_rt_t)GSF_ERR_NOT_READY;
                }     
                s_CurrentState = MEM_READY_BUSY;        //!< set current state
            )
                
            s_wProgramIndex = 0;
            s_pchStream = ptBuffer;                
            
            
            s_tState = PAGE_WRITE_WRITE_PAGE;
            
            //break;            
            
        case PAGE_WRITE_WRITE_PAGE:
            if (fsm_rt_on_going == sdf_page_program(
                        wPageAddress * (SPI_FLASH_PAGE_SIZE / PROGRAM_SIZE)
                      + s_wProgramIndex, s_pchStream)) {
                break;
            } 
            //! write complete
            s_wProgramIndex++;
            s_pchStream += PROGRAM_SIZE;
            
            if (s_wProgramIndex >= (SPI_FLASH_PAGE_SIZE / PROGRAM_SIZE)) {
                SAFE_ATOM_CODE (
                    s_CurrentState = MEM_READY_IDLE;    //!< set idle state
                )
                PAGE_WRITE_RESET_FSM();                 //!< reset FSM
                return fsm_rt_cpl;
            }
            break;
    }
    
    return fsm_rt_on_going;
}

#define SDF_READ_RANDROM_RESET_FSM()    \
            do { s_tState = SDF_READ_RANDROM_START; } while(0)
/*! \brief random read
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wAddress address
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_read_random(mem_t *ptMal, uint32_t wAddress, 
    uint8_t *pchStream, uint_fast16_t hwSize)
{
    static enum {
        SDF_READ_RANDROM_START          = 0,               
        SDF_READ_RANDROM_CHECK_DEVICE,               
        SDF_READ_RANDROM_WRITE_COMMAND_AND_ADDRESS,  
        SDF_READ_RANDROM_FETCH_DATA                 
    } s_tState = SDF_READ_RANDROM_START;        
    
    switch (s_tState) {
        case SDF_READ_RANDROM_START:
            if (    (NULL == pchStream) 
                ||  (wAddress + hwSize > SPI_FLASH_SIZE)) {
                return fsm_rt_err;
            } else if (0 == hwSize) {
                return fsm_rt_cpl;
            }
            
            SAFE_ATOM_CODE (
                //! whether system is initialized
                if (MEM_NOT_READY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return fsm_rt_err;
                } else if (MEM_READY_BUSY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return (fsm_rt_t)GSF_ERR_NOT_READY;
                }     
                s_CurrentState = MEM_READY_BUSY;    //!< set current state
            )
            
            s_tState = SDF_READ_RANDROM_CHECK_DEVICE;
            //break;

        case SDF_READ_RANDROM_CHECK_DEVICE:
            if (fsm_rt_on_going == check_device_state()) {
                break;
            } 
            
            SDF_CLR_CS();                       //!< clr CS#
            //! flash is free
            s_tState = SDF_READ_RANDROM_WRITE_COMMAND_AND_ADDRESS;
            
            //break;

        case SDF_READ_RANDROM_WRITE_COMMAND_AND_ADDRESS:
            if (fsm_rt_on_going == 
                    write_command_and_address(SDF_CMD_READ, wAddress)) {
                break;
            } 
            
            s_tState = SDF_READ_RANDROM_FETCH_DATA;
            //break;

        case SDF_READ_RANDROM_FETCH_DATA:
            if (fsm_rt_cpl == SDF_SPI_STREAM_EXCHANGE(
                                  NULL,pchStream, hwSize)) {
                //! complete
                SDF_SET_CS();                       //! set CS high
                
                SAFE_ATOM_CODE(
                    s_CurrentState = MEM_READY_IDLE;//!< set idle state
                )
                    
                SDF_READ_RANDROM_RESET_FSM();           //!< reset FSM
                return fsm_rt_cpl;                  //!< success
            }
            break;
            
    }
    
    return fsm_rt_on_going;
}

#define SDF_PAGEREAD_RESET_FSM()    \
            do { s_tState = SDF_PAGEREAD_START; } while(0)

/*! \brief read a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_read(mem_t *ptMal, uint32_t wPageAddress, 
    uint_fast16_t hwOffset, uint8_t *pchStream, uint_fast16_t hwSize)
{
    return sdf_read_random(ptMal, (wPageAddress * SPI_FLASH_PAGE_SIZE 
                           + hwOffset), pchStream, hwSize);
}


/*! \brief verify specified data 
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param pStream0 data stream 0
 *! \param pStream0 data stream 0
 *! \param hwSize data stream length
 *! 
 *! \retval true data stream is same
 *! \retval false data stream is different
 */
static bool byte_compare(uint8_t *pStream0, uint8_t *pStream1, uint16_t hwSize)
{
    if (NULL == pStream0 || NULL == pStream1) {
        return false;
    }

    while (hwSize--) {
        if (*pStream0++ != *pStream1++) {
            return false;
        }
    }

    return true;
}


#define SDF_READ_VERIFY_RESET_FSM()    \
            do { s_tState = SDF_VERIFY_START; } while(0)
/*! \brief verify specified data 
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_verify(mem_t *ptMal, uint32_t wPageAddress, 
    uint_fast16_t hwOffset, uint8_t *pchStream, uint_fast16_t hwSize)
{
    NO_INIT static uint8_t s_chVerifyBuffer[VERIFY_BUFFER_SIZE];
    NO_INIT static uint32_t s_wAddress;
    NO_INIT static uint16_t s_hwSize;
    NO_INIT static uint16_t s_hwVerifySize;
    NO_INIT static uint8_t* s_pchStream;

    static enum {
        SDF_VERIFY_START          = 0,               
        SDF_VERIFY_CHECK_RANGE,               
        SDF_VERIFY_READ,  
    } s_tState = SDF_VERIFY_START;        

    switch (s_tState) {
        case SDF_VERIFY_START:
            s_wAddress = wPageAddress * SPI_FLASH_PAGE_SIZE + hwOffset;
            if ((NULL == pchStream) || (s_wAddress + hwSize > SPI_FLASH_SIZE)) {
                return fsm_rt_err;
            } else if (0 == hwSize) {
                return fsm_rt_cpl;
            }
            s_hwSize = hwSize;
            s_hwVerifySize = 0;
            s_pchStream = pchStream;
            s_tState = SDF_VERIFY_CHECK_RANGE;
            //break;

        case SDF_VERIFY_CHECK_RANGE:
            if (0 == s_hwSize) {
                SDF_READ_VERIFY_RESET_FSM();
                return fsm_rt_cpl;
            } 

            if (s_hwSize > UBOUND(s_chVerifyBuffer)) {
                s_hwVerifySize = UBOUND(s_chVerifyBuffer);
                s_hwSize -= UBOUND(s_chVerifyBuffer);
            } else {
                s_hwVerifySize = s_hwSize;
                s_hwSize = 0;
            }
            s_tState = SDF_VERIFY_READ;
            //break;

        case SDF_VERIFY_READ: {
                fsm_rt_t tReturn = sdf_read_random(NULL, s_wAddress, s_chVerifyBuffer, s_hwVerifySize);
            
                if (fsm_rt_cpl == tReturn) {
                    if (byte_compare(s_chVerifyBuffer, s_pchStream, s_hwVerifySize)) {
                        s_pchStream += s_hwVerifySize;
                        s_wAddress += s_hwVerifySize;
                        s_tState = SDF_VERIFY_CHECK_RANGE;
                    } else {
                        SDF_READ_VERIFY_RESET_FSM();
                        return fsm_rt_err;
                    }
                } else if (fsm_rt_on_going != tReturn) {
                    return tReturn;
                } 
            }
            break;        

    }
    /*! add code here */

    return fsm_rt_on_going;
}


#define PAGE_ERASE_RESET_FSM()              \
    do{s_tState = PAGE_ERASE_START;}while(false)

/*! \brief write a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_page_erase(mem_t *ptMal, uint32_t wPageAddress)
{
    static enum {
        PAGE_ERASE_START                = 0,    
        PAGE_ERASE_ENABLE,
        PAGE_ERASE_WRITE_COMMAND_ADDRESS,
        PAGE_ERASE_WRITE_READY,
    } s_tState = PAGE_ERASE_START;

    switch (s_tState) {
        case PAGE_ERASE_START:
            if (wPageAddress >= (SPI_FLASH_SIZE / SPI_FLASH_PAGE_SIZE)) {
                return fsm_rt_err;
            }          
            
            SAFE_ATOM_CODE (          
                //! whether system is initialized
                if (MEM_NOT_READY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return fsm_rt_err;
                } else if (MEM_READY_BUSY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return (fsm_rt_t)GSF_ERR_NOT_READY;
                }     
                
                s_CurrentState = MEM_READY_BUSY;        //!< set current state
            )
            s_tState = PAGE_ERASE_ENABLE;
            //break;
            
        case PAGE_ERASE_ENABLE:
            if (fsm_rt_on_going == enable_write()) {    //!< enable write
                break;
            }
            SDF_SET_WP();                           //!< relase protect
            SDF_CLR_CS();                           //!< clr CS#
            s_tState = PAGE_ERASE_WRITE_COMMAND_ADDRESS;
            //break;

        case PAGE_ERASE_WRITE_COMMAND_ADDRESS:
            if (fsm_rt_on_going == write_command_and_address(
                                 SDF_CMD_SECTOR_ERASE, 
                                 wPageAddress * SPI_FLASH_PAGE_SIZE)) {
                break;
            }
            //! complete
            SDF_SET_CS();                       //!< set CS#
            SDF_CLR_WP();                       //!< set protect
            s_tState = PAGE_ERASE_WRITE_READY;

            //break;
            
        case PAGE_ERASE_WRITE_READY:
            if (fsm_rt_cpl == check_device_state()) {
                //! complete
                SAFE_ATOM_CODE(
                    s_CurrentState = MEM_READY_IDLE;//!< set idle state
                )
                PAGE_ERASE_RESET_FSM();         //!< reset FSM
                return fsm_rt_cpl;
            }
            break;            
    }    

    return fsm_rt_on_going;
}

#define CHIP_ERASE_RESET_FSM()                  \
    do{s_tState = CHIP_ERASE_START;}while(false)

/*! \brief chip erase
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sdf_chip_erase(mem_t *ptMal)
{
    static enum {
        CHIP_ERASE_START                = 0,
        CHIP_ERASE_ENABLE,
        CHIP_ERASE_WRITE_COMMAND_ADDRESS,
        CHIP_ERASE_WRITE_READY,
    } s_tState = CHIP_ERASE_START;

    switch (s_tState) {
        case CHIP_ERASE_START:    
            SAFE_ATOM_CODE(
                //! whether system is initialized
                if (MEM_NOT_READY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return fsm_rt_err;
                } else if (MEM_READY_BUSY == s_CurrentState) {
                    EXIT_SAFE_ATOM_CODE();
                    return (fsm_rt_t)GSF_ERR_NOT_READY;
                }                 
                s_CurrentState = MEM_READY_BUSY;        //!< set current state
                
            )
            s_tState = CHIP_ERASE_ENABLE;
            //break;
            
        case CHIP_ERASE_ENABLE:
            if (fsm_rt_on_going == enable_write()) {    //!< enable write
                break;
            }
            SDF_SET_WP();                               //!< relase protect
            SDF_CLR_CS();                               //!< clr CS#
            s_tState = CHIP_ERASE_WRITE_COMMAND_ADDRESS;

            //break;

        case CHIP_ERASE_WRITE_COMMAND_ADDRESS:
            if (fsm_rt_on_going == 
                    SDF_SPI_BYTE_EXCHANGE(SDF_CMD_CHIP_ERASE, NULL)) {
                break;
            }
            //! complete
            SDF_SET_CS();                               //!< set CS#
            SDF_CLR_WP();                               //!< set protect
            s_tState = CHIP_ERASE_WRITE_READY;

            //break;
            
        case CHIP_ERASE_WRITE_READY:
            if (fsm_rt_cpl == check_device_state()) {
                //! complete
                SAFE_ATOM_CODE(
                    s_CurrentState = MEM_READY_IDLE;    //!< set idle state
                )
                CHIP_ERASE_RESET_FSM();                 //!< reset FSM
                return fsm_rt_cpl;
            }
            break;            
    }    

    return fsm_rt_on_going;
}

/* End of File */

#endif

