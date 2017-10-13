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
#include "..\iframe.h"
#include "..\..\crc\crc.h"

#if USE_SERVICE_ES_SIMPLE_FRAME == ENABLED 

/*============================ MACROS ========================================*/

//! \brief es-simple-frame-head
#ifndef ES_SIMPLE_FRAME_HEAD
#   define ES_SIMPLE_FRAME_HEAD    0xAA
#endif
#ifndef ES_SIMPLE_FRAME_ERROR   
#   define ES_SIMPLE_FRAME_ERROR   0xF0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

DECLARE_CLASS(es_simple_frame_t)

//! \name class: e-snail simple frame
//! @{
DEF_CLASS(es_simple_frame_t)
    i_byte_pipe_t *ptPipe;          //!< pipe
    frame_parser_t *fnParser;       //!< parser
    
    enum {
         WAIT_FOR_HEAD           = 0,
         WAIT_FOR_LENGTH_L       ,
         WAIT_FOR_LENGTH_H       ,
         WAIT_FOR_DATA           ,
         WAIT_FOR_CHECK_SUM_L    ,
         WAIT_FOR_CHECK_SUM_H    ,
         SEND_REPLY_HEAD         ,
         SEND_REPLY_LENGTH_LOW   ,
         SEND_REPLY_LENGTH_HIGH  ,
         SEND_REPLY_DATA         ,
         SEND_REPLY_CHECK_SUM_L  ,
         SEND_REPLY_CHECK_SUM_H  ,
    } tStatus;
    uint8_t *pchBuffer;
    uint16_t hwSize;
    uint16_t hwLength;
    uint16_t hwCounter;
    uint16_t hwCheckSUM;

END_DEF_CLASS(es_simple_frame_t)
//! @}

//! \name es-simple frame configuration structure
//! @{
typedef struct {
    i_byte_pipe_t   *ptPipe; 
    uint8_t         *pchBuffer;
    uint_fast16_t   hwSize;
    frame_parser_t  *fnParser;
}es_simple_frame_cfg_t;
//! @}

//! \name frame interface
//! @{
DEF_INTERFACE(i_es_simple_frame_t)
    bool (*Init)(es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
    fsm_rt_t (*Task)(es_simple_frame_t *ptFrame);
END_DEF_INTERFACE(i_es_simple_frame_t)
//! @}

/*============================ PROTOTYPES ====================================*/
extern bool es_simple_frame_init(  
    es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
extern fsm_rt_t es_simple_frame_task(es_simple_frame_t *ptFrame);
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initlialize es_simple_frame
 *! \param ptFrame es_simple_frame object
 *! \param ptCFG configuration object
 *! \retval true initliazation is successful
 *! \retval false failed in initialization
 */
bool es_simple_frame_init(  
    es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG)
{
    CLASS(es_simple_frame_t) *ptESFrame = (CLASS(es_simple_frame_t) *)ptFrame;
    if (NULL == ptFrame || NULL == ptCFG) {
        return false;
    } else if (    
            (NULL == ptFrame) 
        ||  (NULL == ptCFG->ptPipe)
        ||  (NULL == ptCFG->pchBuffer)
        //|| NULL == fnParser
        || 0 == ptCFG->hwSize ) {
        return false;
    } else if ((NULL == ptCFG->ptPipe->ReadByte) || (NULL == ptCFG->ptPipe->WriteByte)) {
        return false;
    }
    ptESFrame->ptPipe =     ptCFG->ptPipe;
    ptESFrame->fnParser =   ptCFG->fnParser;
    ptESFrame->pchBuffer =  ptCFG->pchBuffer;
    ptESFrame->hwSize =     ptCFG->hwSize;
    ptESFrame->tStatus =   WAIT_FOR_HEAD;
    
    return true;
}

/*! \brief es simple frame task
 *! \param ptFrame es_simple_frame object
 *! \return FSM status
 */
fsm_rt_t es_simple_frame_task(es_simple_frame_t *ptFrame)
{
    CLASS(es_simple_frame_t) *ptESFrame = (CLASS(es_simple_frame_t) *)ptFrame;
    
    if (NULL == ptESFrame) {
        return fsm_rt_cpl;
    } else if (NULL == ptESFrame->ptPipe) {
        return fsm_rt_cpl;
    } else if (NULL == ptESFrame->ptPipe->ReadByte) {
        return fsm_rt_cpl;
    }
    
    do {
        
        uint8_t chByte;
        //! \note this is a byte driven fsm
        if (ptESFrame->tStatus < SEND_REPLY_HEAD) {

            do {
                if (!ptESFrame->ptPipe->ReadByte(&chByte)) {
                    return fsm_rt_wait_for_obj;
                }
            
                //! wait for frame head
                if (WAIT_FOR_HEAD == ptESFrame->tStatus) {
                    
                    if (ES_SIMPLE_FRAME_HEAD == chByte) {
                        //! initialize xor check sum
                        ptESFrame->hwCheckSUM = CRC_INIT;

                        //! transfer to state: wait for length low byte
                        ptESFrame->tStatus = WAIT_FOR_LENGTH_L;
                    } 

                } else if (WAIT_FOR_LENGTH_L == ptESFrame->tStatus) {
                    
                    ptESFrame->hwLength = chByte;
                    //! update check sum
                    CRC(ptESFrame->hwCheckSUM, chByte);
            
                    //! transfer to state: wait for length high byte
                    ptESFrame->tStatus = WAIT_FOR_LENGTH_H;

                } else if (WAIT_FOR_LENGTH_H == ptESFrame->tStatus) {
                    ptESFrame->hwLength |= (((uint16_t)chByte) << 8);

                    if (ptESFrame->hwLength > ptESFrame->hwSize) {
                        //! illegal data length
                        ptESFrame->tStatus = WAIT_FOR_HEAD;
                    } else {
                        //! update check sum
                        CRC(ptESFrame->hwCheckSUM, chByte);
                        
                        if (ptESFrame->hwLength) {
                            //! initialize data counter
                            ptESFrame->hwCounter = 0;
                            
                            //! transfer to state: wait for data
                            ptESFrame->tStatus = WAIT_FOR_DATA;
                        } else {
                            //! transfer to state: wait for xor check sum
                            ptESFrame->tStatus = WAIT_FOR_CHECK_SUM_L;
                        }
                    }
                } else if (WAIT_FOR_DATA == ptESFrame->tStatus) {
                    //! save data to buffer
                    ptESFrame->pchBuffer[ptESFrame->hwCounter++] = chByte;
                    //! update check sum
                    CRC(ptESFrame->hwCheckSUM, chByte);

                    //! check received data number
                    if (ptESFrame->hwCounter >= ptESFrame->hwLength) {
                        //! transfer to state: wait for xor check sum
                        ptESFrame->tStatus = WAIT_FOR_CHECK_SUM_L;
                    }
                } else if (WAIT_FOR_CHECK_SUM_L == ptESFrame->tStatus) {
                    if ((ptESFrame->hwCheckSUM & 0xFF) == chByte) {
                        ptESFrame->tStatus = WAIT_FOR_CHECK_SUM_H;
                    } else {
                        //! reset fsm
                        ptESFrame->tStatus = WAIT_FOR_HEAD;
                    }
                } else if (WAIT_FOR_CHECK_SUM_H == ptESFrame->tStatus) {
                    if ((ptESFrame->hwCheckSUM >> 8) == chByte) {

                        if (NULL != ptESFrame->ptPipe->WriteByte) {
                            ptESFrame->tStatus = SEND_REPLY_HEAD;
                        } else {
                            //! reset fsm
                            ptESFrame->tStatus = WAIT_FOR_HEAD;
                        }

                        if (NULL != ptESFrame->fnParser) {  
                            //! call handler
                            uint_fast16_t hwResult = ptESFrame->fnParser(
                                            ptESFrame->pchBuffer, ptESFrame->hwLength);
                            if (hwResult) {
                                //! frame parsing success, reply data stored in buffer
                                ptESFrame->hwLength = hwResult;
                                break;
                            } 
                        }
                        
                        //! send default reply: error
                        do {
                            ptESFrame->pchBuffer[0] = ES_SIMPLE_FRAME_ERROR;
                            ptESFrame->hwLength = 1;
                        } while (false);
                        break;
                    } else {
                        //! reset fsm
                        ptESFrame->tStatus = WAIT_FOR_HEAD;
                    }
            
                }  
                
            } while (true);
        } else {
            
            if (SEND_REPLY_HEAD == ptESFrame->tStatus) {
                //! write frame head
                if (ptESFrame->ptPipe->WriteByte(ES_SIMPLE_FRAME_HEAD)) {
                    ptESFrame->hwCheckSUM = CRC_INIT;
                    ptESFrame->tStatus = SEND_REPLY_LENGTH_LOW;
                }
            } 
            if (SEND_REPLY_LENGTH_LOW == ptESFrame->tStatus) {
                //! write frame length low
                uint8_t chData = ptESFrame->hwLength & 0xFF;
                if (ptESFrame->ptPipe->WriteByte(chData)) {
                    //! calculate check sum
                    CRC(ptESFrame->hwCheckSUM, chData);
                    ptESFrame->tStatus = SEND_REPLY_LENGTH_HIGH;
                }
            }
            if (SEND_REPLY_LENGTH_HIGH == ptESFrame->tStatus) {
                //! write frame length high
                uint8_t chData = ptESFrame->hwLength >> 8;
                if (ptESFrame->ptPipe->WriteByte(chData)) {
                    //! calculate check sum
                    CRC(ptESFrame->hwCheckSUM, chData);
                    ptESFrame->hwCounter = 0;
                    ptESFrame->tStatus = SEND_REPLY_DATA;
                }
            }
            if (SEND_REPLY_DATA == ptESFrame->tStatus) {
                do {
                    //! write data
                    uint8_t chData = ptESFrame->pchBuffer[ptESFrame->hwCounter];
                    if (ptESFrame->ptPipe->WriteByte(chData)) {
                        //! calculate check sum
                        CRC(ptESFrame->hwCheckSUM, chData);

                        ptESFrame->hwCounter++;
                        if (ptESFrame->hwCounter >= ptESFrame->hwLength) {
                            ptESFrame->tStatus = SEND_REPLY_CHECK_SUM_L;
                            break;
                        }                    
                    } else {
                        break;
                    }
                } while (true);
            }
            if (SEND_REPLY_CHECK_SUM_L == ptESFrame->tStatus) {
                //! write check sum low
                if (ptESFrame->ptPipe->WriteByte(ptESFrame->hwCheckSUM & 0xFF)) {
                    ptESFrame->tStatus = SEND_REPLY_CHECK_SUM_H;
                }
            }
            if (SEND_REPLY_CHECK_SUM_H == ptESFrame->tStatus) {
                //! write check sum high
                if (ptESFrame->ptPipe->WriteByte(ptESFrame->hwCheckSUM >> 8)) {
                    ptESFrame->tStatus = WAIT_FOR_HEAD;
                    return fsm_rt_cpl;
                }
            }
        }
    } while (false);
    
    return fsm_rt_on_going;
}


#endif

/* EOF */







