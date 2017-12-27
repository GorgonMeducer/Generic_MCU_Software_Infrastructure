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

#if USE_SERVICE_XMODEM == ENABLED
/*============================ MACROS ========================================*/
//! \name Xmodem Protocol Control Character
//! @{
#define SOH                             0x01
#define STX                             0x02
#define EOT                             0x04
#define ACK                             0x06
#define NAK                             0x15
#define CAN                             0x18
#define CRC_C                           0x43
//! @}

//! \name Xmodem Protocol State Parameters
//! @{
#define MAX_TRY_AGAN                    (10ul)
#define XMODEM_DATA_BUFFER_SIZE         (128ul)
#define XMODEM_1K_DATA_BUFFER_SIZE      (1024ul)
//! @}

//! \name Xmodem Protocol Extern Parameters
//! @{
#ifndef TIMEOUT_ONE_SECOMD
#define TIMEOUT_ONE_SECOMD              (500ul)
#endif

#ifndef TIMEOUT_TEN_SECOMD
#define TIMEOUT_TEN_SECOMD              (5000ul)
#endif

#define fsm_rt_user_cancel              ((fsm_rt_t)-2)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name xmodem receive configuration
//! @{
typedef struct {
    uint8_t *pchBuffer;         //!< receive buffer
    enum {
        XMODEM_BLOCK_SZ_256  = 0, 
        XMODEM_BLOCK_SZ_1K,
    } tMode;
} xmodem_t;
//! @}

//! \name xmodem receiver package state
//! @{
typedef enum {
    PACKET_CPL                      = 0,    //!< fsm complete
    PACKET_ON_GOING                 = 1,    //!< fsm on-going
    PACKET_FAIL                     = 2,
    PACKET_CAN                      = 3,
    PACKET_NO_ERROR                 = 4,    //!< the Sender send a EOT
    PACKET_NO_CHAR                  = 5,    //!< received no char
    PACKET_TIMEOUT                  = 6,    //!< read timeout
    PACKET_INCORRECT_HEADER         = 7,    //!< incorrect head char
    PACKET_INCORRECT_PACKET_NUMBER  = 8,    //!< incorrect packet number
    PACKET_DUPLICATE_PACKET_NUMBER  = 9,    //!< duplicate packet number
    PACKET_INCORRECT_CHECKOUT       = 10,   //!< incorrect checkout
} xm_packet_t;
//! @}

//! \name xmodem receiver package
//! @{
typedef struct {
    uint8_t chHead;             //!< receiver package header
    uint8_t chBlk;              //!< receiver package block counter
    uint8_t chNBlk;             //!< receiver package block counter nverse code
    uint8_t chCheck[2];         //!< receiver package block checksum
} packet_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
//! buffer pointer
NO_INIT static uint8_t *s_pchBuffer;
//! receiver package header
NO_INIT static uint8_t s_chHead;
//! receiver package size
NO_INIT static uint16_t s_hwBufferSize;
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 *              DEPENDENT INTERFACE                                           *
 *----------------------------------------------------------------------------*/

/*! \brief xmodem lower layer byte write
 *! \param chByte xmodem usart write byte
 *! \return none
 */
extern bool xmodem_lower_layer_write(uint8_t chByte);

/*! \brief xmodem lower layer byte read
 *! \param pchByte xmodem usart write byte
 *! \return none
 */
extern bool xmodem_lower_layer_read(uint8_t *pchByte);

/*! \brief update next buffer or control transfer speed
 *! \param hwLength buffer length
 *! \return buffer address
 */
extern uint8_t * xmodem_call_back_receive(uint16_t hwLength);

/*----------------------------------------------------------------------------*
 *              BASE SUBROUTINE                                               *
 *----------------------------------------------------------------------------*/

/*! \brief xmodem read with timeout
 *! \param pchByte read return value pointer
 *! \retval XM_ON_GOING  FSM on-going
 *! \retval XM_CPL  read data succeed
 *! \retval XM_TIMEOUT  read data fail(timeout)
 */
static xm_packet_t xmodem_read_with_timeout(uint8_t * pchByte, uint32_t wTimeout);

/*! \brief Cyclic Redundancy Check by 16 bit 
 *! \param crc history crc value
 *! \retval CRC Check Value
 */
static uint16_t CRC_16_CCITT(uint16_t hwCRCValue, uint8_t chData);

/*----------------------------------------------------------------------------*
 *              RECEIVE SUBROUTINE                                            *
 *----------------------------------------------------------------------------*/

/*! \brief xmodem receive 128 byte data
 *! \param none
 *! \retval XM_ON_GOING  FSM on-going
 *! \retval XM_CPL FSM complete
 *! \retval XM_TIMEOUT timeout
 */
static xm_packet_t xmodem_rec_data(uint8_t * pchBuffer, bool chMode, uint16_t * pchCheck);

/*! \brief xmodem receive checkout 
 *! \param none
 *! \retval XM_ON_GOING  FSM on-going
 *! \retval XM_CPL FSM complete
 *! \retval XM_TIMEOUT timeout
 */
static xm_packet_t xmodem_rec_checkout(uint8_t * pchBuffer, bool chMode);

/*! \brief xmodem receive a package 
 *! \param pchBuffer receive data buffer
 *! \param chMode CRC Mode or CKSUN Mode
 *! \retval PACKET_ON_GOING                 Receiving
 *! \retval PACKET_CPL                      Received
 *! \retval PACKET_NO_CHAR                  Not Char to be received
 *! \retval PACKET_TIMEOUT                  Receive Timeout
 *! \retval PACKET_INCORRECT_HEADER         Incorrect Head Char
 *! \retval PACKET_INCORRECT_PACKET_NUMBER  Incorrect Packet Number
 *! \retval PACKET_DUPLICATE_PACKET_NUMBER  Duplicate Packet Number
 *! \retval PACKET_INCORRECT_CHECKOUT       Incorrect Checkout
 */
static xm_packet_t xmodem_rec_package(uint8_t *pchBuffer, bool bCRCMode);
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 *              XMODEM INTERFACE                                              *
 *----------------------------------------------------------------------------*/

/*! \brief initialize xmodem receive service
 *! \param pchBuffer xmodem receive configuration
 *  \retval true xmodem receive service initialization succeeded
 *  \retval false xmodem receive service initialization failed
 */
bool xmodem_init(xmodem_t *ptCFG)
{
    if ((NULL == ptCFG) || (NULL == ptCFG->pchBuffer)) {
        return false;
    }

    //! initialize buffer
    s_pchBuffer = ptCFG->pchBuffer;
    //! switch xmode mode
    if (XMODEM_BLOCK_SZ_256 == ptCFG->tMode ) {
        s_chHead = SOH;
    } else {
        s_chHead = STX;
    }

    return true;
}

#define XMODEM_REC_START 				            0
#define XMODEM_REC_RESET_FSM()    		            do{s_chState = 0;}while(0)
#define XMODEM_MODE_SEND				            1
#define XMODEM_MODE_REC_PACKET				        2

#define C_TRYING_COUNT                              3

#define MODE_HS_CRC                                 1
#define MODE_HS_CKSUM                               2
#define MODE_HS_FINSH                               3

/*! \brief xmodem task
 *! \retrun fsm_rt_cpl          xmodem transfer finish
 *! \retrun fsm_rt_on_going     xmodem transfer on-going
 *! \retrun fsm_rt_on_err       xmodem transfer error
 *! \retrun fsm_rt_user_cancel  user cancel transfer
 */
fsm_rt_t xmodem_task(void)
{
    static uint8_t      s_chState = XMODEM_REC_START;
    static uint8_t      s_chTryCount = 0;
    static uint8_t      s_chSend = CRC_C;
    static uint8_t      s_bMode = true;     //!< true: CRC false:CKSUM
    static xm_packet_t  s_tRec = PACKET_ON_GOING;
    static uint8_t      s_chHandShake = MODE_HS_CRC;
    
    switch (s_chState) {
        case XMODEM_REC_START:
            if (NULL == s_pchBuffer) {
                return fsm_rt_err;
            }
            s_chTryCount = 0;
            s_chSend = CRC_C;
            s_bMode = true;
            s_tRec = PACKET_ON_GOING;
            s_chState = XMODEM_MODE_SEND;
            s_chHandShake = MODE_HS_CRC;
            //break;

        case XMODEM_MODE_SEND:
            if (xmodem_lower_layer_write(s_chSend)) {          //!< send a char
                if (PACKET_CPL == s_tRec) {             //!< xmodem receive CPL
                    XMODEM_REC_RESET_FSM();
                    return fsm_rt_cpl;
                } else if (PACKET_CAN == s_tRec) {     //!< xmodem receive fail
                    XMODEM_REC_RESET_FSM();
                    return fsm_rt_user_cancel;
                } else if (PACKET_FAIL == s_tRec) {     //!< xmodem receive fail
                    XMODEM_REC_RESET_FSM();
                    return fsm_rt_err;
                } else {                                //!< xmodem receiving
                    s_chState = XMODEM_MODE_REC_PACKET; 
                }                
            }
            break;
  
        case XMODEM_MODE_REC_PACKET:
            s_tRec = xmodem_rec_package(s_pchBuffer, s_bMode);    //!< receive a frame
            switch (s_tRec) {
                /*
                case PACKET_ON_GOING:                   //!< xmodem receiving 
                    break;
                */

                case PACKET_NO_ERROR:                   //!< xmodem receive a frame with no error                    
                    s_chTryCount = 0;
                    //break;

                case PACKET_DUPLICATE_PACKET_NUMBER:
                    s_chHandShake = MODE_HS_FINSH;
                    s_chSend = ACK;
                    s_chState = XMODEM_MODE_SEND; 
                    break;

                case PACKET_CAN:                        //!< xmodem send cancel
                    //break;
                    
                case PACKET_CPL:                        //!< xmodem reveive CPL
                    s_chSend = ACK;
                    s_chState = XMODEM_MODE_SEND;                  
                    break;

                case PACKET_FAIL:                       //!< xmodem receive fail
                    s_chSend = CAN;
                    s_chState = XMODEM_MODE_SEND;                  
                    break;                    

                case PACKET_TIMEOUT:                    //!< xmode receive timeout
                    //break;

                case PACKET_INCORRECT_HEADER:           //!< xmode receive incorrect header
                    //break;

                case PACKET_INCORRECT_PACKET_NUMBER:    //!< xmode receive incorrect packet number
                    //break;

                case PACKET_INCORRECT_CHECKOUT:         //!< xmode receive incorrect checkout
                    s_chHandShake = MODE_HS_FINSH;
                    s_chTryCount++;
                    if (MAX_TRY_AGAN > s_chTryCount) {
                        s_chSend = NAK;
                        s_chState = XMODEM_MODE_SEND;           
                    } else {
                        XMODEM_REC_RESET_FSM();
                        return fsm_rt_err;     
                    } 
                    break;
                    
                case PACKET_NO_CHAR:                    //!< no char to be received 
                    s_chTryCount++;
                    //! switch CRC mode or CKSUM mode
                    if (MODE_HS_CRC == s_chHandShake) {     
                        if (C_TRYING_COUNT == s_chTryCount) {
                            s_chSend = NAK; 
                            s_bMode = false;
                            s_chHandShake = MODE_HS_CKSUM;
                        } else {
                            s_chSend = CRC_C;
                        }       
                    } else if (MODE_HS_CKSUM == s_chHandShake) {
                        s_chSend = NAK;
                        if (MAX_TRY_AGAN == s_chTryCount) {
                            s_chHandShake = MODE_HS_FINSH;
                            XMODEM_REC_RESET_FSM();
                            return fsm_rt_err;
                        }
                    } else {     
                        s_chSend = NAK;
                        if (MAX_TRY_AGAN == s_chTryCount) {
                            XMODEM_REC_RESET_FSM();
                            return fsm_rt_err;     
                        } 
                    }                     
                    s_chState = XMODEM_MODE_SEND;  
                    break;                     
            }
            break;                      
    }         
                
    return fsm_rt_on_going;
}


/*----------------------------------------------------------------------------*
 *              BACE   SUBROUTINE                                             *
 *----------------------------------------------------------------------------*/

/*! \brief update next buffer or control transfer speed
 *! \param hwLength buffer length
 *! \return buffer address
 */
WEAK uint8_t * xmodem_call_back_receive(uint16_t hwLength)
{
    return s_pchBuffer;
}

#define XMODEM_READ_START 				        0
#define XMODEM_READ_RESET_FSM()    		        do{s_chState = 0;}while(0)
#define XMODEM_READ_DOING				        1
#define XMODEM_READ_DELAY 				        2

/*! \brief xmodem read with timeout
 *! \param pchByte read return value pointer
 *! \retval xm_on_going  FSM on-going
 *! \retval xm_cpl  read data succeed
 *! \retval xm_timeout  read data fail(timeout)
 */
static xm_packet_t xmodem_read_with_timeout(uint8_t * pchByte, uint32_t wTimeout)
{
    static uint8_t  s_chState = XMODEM_READ_START;
    static uint32_t s_wTimeCount = 0;

	switch (s_chState) {
		case XMODEM_READ_START:
            s_wTimeCount = 0;
            s_chState = XMODEM_READ_DOING;
			//break;

		case XMODEM_READ_DOING:
            if (xmodem_lower_layer_read(pchByte)) {    //!< read succeed
                XMODEM_READ_RESET_FSM();
                return PACKET_CPL;
            } else {                            //!< read fail
                s_chState = XMODEM_READ_DELAY;
            }
			break;

		case XMODEM_READ_DELAY:                 //! wait timeout
            if (wTimeout > s_wTimeCount) {
                s_wTimeCount++;
                s_chState = XMODEM_READ_DOING;
            } else {
                XMODEM_READ_RESET_FSM();
                return PACKET_TIMEOUT;
            }
			break;			
	}
			
	return PACKET_ON_GOING;
} 

/*! \brief Cyclic Redundancy Check by CRC-16-CCITT
 *! \param crc history crc value
 *! \retval CRC Check Value
 */
static uint16_t CRC_16_CCITT(uint16_t hwCRCValue, uint8_t chData)
{ 
    uint8_t chTemp = 8; 

    hwCRCValue = hwCRCValue ^ chData << 8; 

    do 
    { 
        if (hwCRCValue & 0x8000) {
            hwCRCValue = hwCRCValue << 1 ^ 0x1021; 
        } else {
            hwCRCValue = hwCRCValue << 1; 
        }        
    }while (--chTemp); 

    return hwCRCValue; 
} 
/*----------------------------------------------------------------------------*
 *              RECEIVE SUBROUTINE                                            *
 *----------------------------------------------------------------------------*/

#define XMODEM_REC_START 				            0
#define XMODEM_REC_RESET_FSM()    		            do{s_chState = 0;}while(0)
#define XMODEM_REC_HEAD					            2
#define XMODEM_REC_BLK 				                3
#define XMODEM_REC_NBLK 				            4
#define XMODEM_REC_DATA				                5
#define XMODEM_REC_CHECKOUT                         6   	                    
#define XMODEM_CHECK_FRAME                          7
#define XMODEM_SAVE                                 8

#define REC_FRAME_SIZE                              (1024 + 5)
#define REC_HEAD                                    0
#define REC_BLK                                     1
#define REC_NBLK                                    2
#define REC_DATA                                    3
#define REC_CHECK                                   (1024 + 3)

/*! \brief xmodem receive a package 
 *! \param pchBuffer receive data buffer
 *! \param chMode CRC Mode or CKSUN Mode
 *! \retval PACKET_ON_GOING                 Receiving
 *! \retval PACKET_CPL                      Received
 *! \retval PACKET_NO_CHAR                  Not Char to be received
 *! \retval PACKET_TIMEOUT                  Receive Timeout
 *! \retval PACKET_INCORRECT_HEADER         Incorrect Head Char
 *! \retval PACKET_INCORRECT_PACKET_NUMBER  Incorrect Packet Number
 *! \retval PACKET_DUPLICATE_PACKET_NUMBER  Duplicate Packet Number
 *! \retval PACKET_INCORRECT_CHECKOUT       Incorrect Checkout
 */
static xm_packet_t xmodem_rec_package(uint8_t *pchBuffer, bool bCRCMode)
{
    static uint8_t s_chState = XMODEM_REC_START;    
    static uint16_t s_hwCheck = 0;
    static uint8_t s_chPacketNumber = 1;
    xm_packet_t tRead;
    //NO_INIT static uint8_t s_chFrame[REC_FRAME_SIZE]; 
    NO_INIT static packet_t s_tFrame;
    NO_INIT static uint8_t chByte;
    
    switch (s_chState) {
        case XMODEM_REC_START:
            s_hwCheck = 0;
            s_chState = XMODEM_REC_HEAD;
            //break;
            
        case XMODEM_REC_HEAD:                       //!< receive header
            tRead = xmodem_read_with_timeout(&chByte, TIMEOUT_TEN_SECOMD);
            if (PACKET_CPL == tRead) {
                if (EOT == chByte) {
                    s_chPacketNumber = 1;
                    XMODEM_REC_RESET_FSM();
                    return PACKET_CPL;
                } else if (CAN == chByte) {
                    s_chPacketNumber = 1;
                    XMODEM_REC_RESET_FSM();
                    return PACKET_CAN;
                } else if ((STX == s_chHead) && (STX == chByte)) {
                    s_hwBufferSize = XMODEM_1K_DATA_BUFFER_SIZE;
                } else {
                    s_hwBufferSize = XMODEM_DATA_BUFFER_SIZE;
                }
                s_tFrame.chHead = chByte;
                s_chState = XMODEM_REC_BLK;
            } else if (PACKET_TIMEOUT == tRead) {
                XMODEM_REC_RESET_FSM();
                return PACKET_NO_CHAR;
            } /*else {
                //! on-gong
            } */            
            break;

        case XMODEM_REC_BLK:                        //!< receive blk
            tRead = xmodem_read_with_timeout(&chByte, TIMEOUT_ONE_SECOMD);
            if (PACKET_CPL == tRead) {
                s_tFrame.chBlk = chByte;
                s_chState = XMODEM_REC_NBLK;
            } else if (PACKET_TIMEOUT == tRead) {
                XMODEM_REC_RESET_FSM();
                return PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */
            break;

        case XMODEM_REC_NBLK:                       //!< receive 255 - blk
            tRead = xmodem_read_with_timeout(&chByte, TIMEOUT_ONE_SECOMD);
            if (PACKET_CPL == tRead) {
                s_tFrame.chNBlk = chByte;
                s_chState = XMODEM_REC_DATA;
            } else if (PACKET_TIMEOUT == tRead) {
                XMODEM_REC_RESET_FSM();
                return PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */
            break;

        case XMODEM_REC_DATA:                       //!< receive 128 byte data
            tRead = xmodem_rec_data(s_pchBuffer, bCRCMode, &s_hwCheck);
            if (PACKET_CPL == tRead) {
                s_chState = XMODEM_REC_CHECKOUT;
            } else if (PACKET_TIMEOUT == tRead) {
                XMODEM_REC_RESET_FSM();
                return PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */
            break;        
            
        case XMODEM_REC_CHECKOUT:
            tRead = xmodem_rec_checkout(s_tFrame.chCheck, bCRCMode);
            if (PACKET_CPL == tRead) {
                s_chState = XMODEM_CHECK_FRAME;
            } else if (PACKET_TIMEOUT == tRead) {
                XMODEM_REC_RESET_FSM();
                return PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */
            break;

        case XMODEM_CHECK_FRAME:
            //if (s_chHead != s_tFrame.chHead) {
            if ((STX != s_tFrame.chHead) && (SOH != s_tFrame.chHead) ) {
                XMODEM_REC_RESET_FSM();
                return PACKET_INCORRECT_HEADER;
            }
            if (0xFF != (s_tFrame.chBlk ^ s_tFrame.chNBlk)) {
                XMODEM_REC_RESET_FSM();
                return PACKET_INCORRECT_PACKET_NUMBER;
            }
            if ((s_chPacketNumber - 1) == s_tFrame.chBlk ) {
                XMODEM_REC_RESET_FSM();
                return PACKET_DUPLICATE_PACKET_NUMBER;
            }
            if (s_chPacketNumber != s_tFrame.chBlk ) {
                XMODEM_REC_RESET_FSM();
                return PACKET_FAIL;
            }
            if (bCRCMode) {
                if (s_hwCheck != ((uint16_t)s_tFrame.chCheck[0] * 256 + (uint16_t)s_tFrame.chCheck[1])) {
                    XMODEM_REC_RESET_FSM();
                    return PACKET_INCORRECT_CHECKOUT;
                }
            } else {
                if ((uint8_t)s_hwCheck != s_tFrame.chCheck[0]) {
                    XMODEM_REC_RESET_FSM();
                    return PACKET_INCORRECT_CHECKOUT;
                }
            }
            s_chPacketNumber++;
            s_chState = XMODEM_SAVE;
            break;

        case XMODEM_SAVE: {
                uint8_t * pchNext = xmodem_call_back_receive(s_hwBufferSize);
                if (NULL != pchNext) {
                    s_pchBuffer = pchNext;
                    XMODEM_REC_RESET_FSM();
                    return PACKET_NO_ERROR;
                }
            }
            break;
    }
    
    return PACKET_ON_GOING;
}    


#define XMODEM_REC_DATA_START 				        0
#define XMODEM_REC_DATA_RESET_FSM()    		        do{s_chState = 0;}while(0)
#define XMODEM_REC_DATA_READ			            1
#define XMODEM_REC_DATA_NEXT                        2

/*! \brief xmodem receive 128 byte data
 *! \param none
 *! \retval XM_ON_GOING  FSM on-going
 *! \retval XM_CPL FSM complete
 *! \retval XM_TIMEOUT timeout
 */
static xm_packet_t xmodem_rec_data(uint8_t * pchBuffer, bool chMode, uint16_t * pchCheck)
{
    static  uint8_t s_chState = XMODEM_REC_DATA_START;  
    static  uint16_t s_hwIndex = 0;

    xm_packet_t tRead;  
    xm_packet_t tRetVal = PACKET_ON_GOING;
    uint8_t chByte;
    
    switch (s_chState) {
        case XMODEM_REC_DATA_START:
            s_hwIndex = 0;
            s_chState = XMODEM_REC_DATA_READ;
            //break;
            
        case XMODEM_REC_DATA_READ:
            tRead = xmodem_read_with_timeout(&chByte, TIMEOUT_ONE_SECOMD);            
            if (PACKET_CPL == tRead) {                         //!< read succeed
                pchBuffer[s_hwIndex] = chByte;
                if (chMode) {
                    *pchCheck = CRC_16_CCITT(*pchCheck, chByte);
                } else {
                    *pchCheck += chByte;
                }                
                s_chState = XMODEM_REC_DATA_NEXT;
            } else if (PACKET_TIMEOUT == tRead) {              //!< timeout
                XMODEM_REC_DATA_RESET_FSM();
                tRetVal = PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */      
            break;        
            
        case XMODEM_REC_DATA_NEXT:
            s_hwIndex++;
            if (s_hwBufferSize > s_hwIndex) {                
                s_chState = XMODEM_REC_DATA_READ;
            } else {
                XMODEM_REC_DATA_RESET_FSM();
                return PACKET_CPL;            
            }
            break;
    }
    
    return tRetVal;
}

#define XMODEM_REC_CHECKOUT_START 				        0
#define XMODEM_REC_CHECKOUT_RESET_FSM()    		        do{s_chState = 0;}while(0)
#define XMODEM_REC_CHECKOUT_HIGH			            1
#define XMODEM_REC_CHECKOUT_LOW 			            2
#define XMODEM_REC_CHECKOUT_SAVE			            3
#define XMODEM_REC_CHECKOUT_ACK 			            4

/*! \brief xmodem receive checkout 
 *! \param none
 *! \retval XM_ON_GOING  FSM on-going
 *! \retval XM_CPL FSM complete
 *! \retval XM_TIMEOUT timeout
 */
static xm_packet_t xmodem_rec_checkout(uint8_t * pchBuffer, bool chMode)
{
    static  uint8_t s_chState =XMODEM_REC_CHECKOUT_START;
    xm_packet_t tRunState;
    uint8_t chByte;
    
    switch (s_chState) {
        case XMODEM_REC_CHECKOUT_START:
            s_chState = XMODEM_REC_CHECKOUT_HIGH;
            //break;
            
        case XMODEM_REC_CHECKOUT_HIGH:
            tRunState = xmodem_read_with_timeout(&chByte, TIMEOUT_ONE_SECOMD);            
            if (PACKET_CPL == tRunState) {                         //!< read succeed
                pchBuffer[0] = chByte;
                if (chMode) {
                    s_chState = XMODEM_REC_CHECKOUT_LOW;
                } else {
                    XMODEM_REC_CHECKOUT_RESET_FSM();
                    return PACKET_CPL;
                }   
            } else if (PACKET_TIMEOUT == tRunState) {              //!< timeout
                XMODEM_REC_CHECKOUT_RESET_FSM();
                return PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */  
            break;    
    
        case XMODEM_REC_CHECKOUT_LOW:
            tRunState = xmodem_read_with_timeout(&chByte, TIMEOUT_ONE_SECOMD);            
            if (PACKET_CPL == tRunState) {                         //!< read succeed
                pchBuffer[1] = chByte;
                XMODEM_REC_CHECKOUT_RESET_FSM();
                return PACKET_CPL;
            } else if (PACKET_TIMEOUT == tRunState) {              //!< timeout
                XMODEM_REC_CHECKOUT_RESET_FSM();
                return PACKET_TIMEOUT;
            } /*else {
                //! on-gong
            } */  
            break;        
    }
    
    return PACKET_ON_GOING;
}
#endif
/*EOF*/
