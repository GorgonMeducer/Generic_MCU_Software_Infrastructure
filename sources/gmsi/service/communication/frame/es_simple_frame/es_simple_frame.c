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
#include <string.h>

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

def_structure(__es_simple_frame_fsm_internal)
    uint8_t *pchBuffer;
    uint16_t hwSize;
    uint16_t hwLength;
    uint16_t hwCounter;
    uint16_t hwCheckSUM;
end_def_structure(__es_simple_frame_fsm_internal)

simple_fsm(es_simple_frame_parser,
    def_params(
        i_byte_pipe_t *ptPipe;          //!< pipe
        frame_parser_t *fnParser;       //!< parser
        inherit(__es_simple_frame_fsm_internal)
    ))
    
simple_fsm(es_simple_frame_encoder,
    def_params(
        i_byte_pipe_t *ptPipe;          //!< pipe
        inherit(__es_simple_frame_fsm_internal)
    ))


declare_class(es_simple_frame_t)

//! \name class: e-snail simple frame
//! @{
def_class(es_simple_frame_t)

    //uint_fast8_t chState;
    inherit(fsm(es_simple_frame_parser))
    inherit(fsm(es_simple_frame_encoder))
end_def_class(es_simple_frame_t)
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
def_interface(i_es_simple_frame_t)
    bool (*Init)(es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
    fsm_rt_t (*Task)(es_simple_frame_t *ptFrame);
end_def_interface(i_es_simple_frame_t)
//! @}

/*============================ PROTOTYPES ====================================*/

extern_fsm_initialiser(es_simple_frame_parser,
    args(
        i_byte_pipe_t *ptPipe, 
        frame_parser_t *fnParser
    ))

extern_fsm_initialiser(es_simple_frame_encoder,
    args(
        i_byte_pipe_t *ptPipe
    ))

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
    CLASS(es_simple_frame_t) *ptThis = (CLASS(es_simple_frame_t) *)ptFrame;
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
    
    if (!init_fsm(es_simple_frame_parser, 
                    REF_OBJ_AS( this, fsm(es_simple_frame_parser)), 
                    args(ptCFG->ptPipe, ptCFG->fnParser))) {
        return false;
    }
    return init_fsm(es_simple_frame_encoder, 
                    REF_OBJ_AS( this, fsm(es_simple_frame_encoder)), 
                    args(ptCFG->ptPipe));   
}

fsm_initialiser(es_simple_frame_parser,
    args(
        i_byte_pipe_t *ptPipe, 
        frame_parser_t *fnParser
    ))
        
    init_body(
       
        if (NULL == ptPipe || NULL == fnParser) {
            abort_init();
        } else if ((NULL == ptPipe->ReadByte) || (NULL == ptPipe->WriteByte)) {
            abort_init();
        }
        memset(ref_obj_as(this, __es_simple_frame_fsm_internal), 0, sizeof(this));
        this.ptPipe = ptPipe;
        this.fnParser = fnParser;
    )
        



/*! \brief es simple frame task
 *! \param ptFrame es_simple_frame object
 *! \return FSM status
 */
fsm_rt_t es_simple_frame_task(es_simple_frame_t *ptFrame)
{
    class_internal(ptFrame, ptESFrame, es_simple_frame_t);
    class_internal(REF_OBJ_AS( (*ptESFrame), fsm(es_simple_frame_parser)), ptThis, fsm(es_simple_frame_parser));

    
    def_states (
         WAIT_FOR_HEAD  = 0,
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
    )
    
    if (NULL == ptESFrame) {
        return fsm_rt_cpl;
    } else if (NULL == this.ptPipe) {
        return fsm_rt_cpl;
    } else if (NULL == this.ptPipe->ReadByte) {
        return fsm_rt_cpl;
    }
    
    do {
        
        uint8_t chByte;
        //! \note this is a byte driven fsm
        if (this.chState < SEND_REPLY_HEAD) {

            do {
                if (!this.ptPipe->ReadByte(&chByte)) {
                    return fsm_rt_wait_for_obj;
                }
            
                //! wait for frame head
                if (WAIT_FOR_HEAD == this.chState) {
                    
                    if (ES_SIMPLE_FRAME_HEAD == chByte) {
                        //! initialize xor check sum
                        this.hwCheckSUM = CRC_INIT;

                        //! transfer to state: wait for length low byte
                        this.chState = WAIT_FOR_LENGTH_L;
                    } 

                } else if (WAIT_FOR_LENGTH_L == this.chState) {
                    
                    this.hwLength = chByte;
                    //! update check sum
                    CRC(this.hwCheckSUM, chByte);
            
                    //! transfer to state: wait for length high byte
                    this.chState = WAIT_FOR_LENGTH_H;

                } else if (WAIT_FOR_LENGTH_H == this.chState) {
                    this.hwLength |= (((uint16_t)chByte) << 8);

                    if (this.hwLength > this.hwSize) {
                        //! illegal data length
                        this.chState = WAIT_FOR_HEAD;
                    } else {
                        //! update check sum
                        CRC(this.hwCheckSUM, chByte);
                        
                        if (this.hwLength) {
                            //! initialize data counter
                            this.hwCounter = 0;
                            
                            //! transfer to state: wait for data
                            this.chState = WAIT_FOR_DATA;
                        } else {
                            //! transfer to state: wait for xor check sum
                            this.chState = WAIT_FOR_CHECK_SUM_L;
                        }
                    }
                } else if (WAIT_FOR_DATA == this.chState) {
                    //! save data to buffer
                    this.pchBuffer[this.hwCounter++] = chByte;
                    //! update check sum
                    CRC(this.hwCheckSUM, chByte);

                    //! check received data number
                    if (this.hwCounter >= this.hwLength) {
                        //! transfer to state: wait for xor check sum
                        this.chState = WAIT_FOR_CHECK_SUM_L;
                    }
                } else if (WAIT_FOR_CHECK_SUM_L == this.chState) {
                    if ((this.hwCheckSUM & 0xFF) == chByte) {
                        this.chState = WAIT_FOR_CHECK_SUM_H;
                    } else {
                        //! reset fsm
                        this.chState = WAIT_FOR_HEAD;
                    }
                } else if (WAIT_FOR_CHECK_SUM_H == this.chState) {
                    if ((this.hwCheckSUM >> 8) == chByte) {

                        if (NULL != this.ptPipe->WriteByte) {
                            this.chState = SEND_REPLY_HEAD;
                        } else {
                            //! reset fsm
                            this.chState = WAIT_FOR_HEAD;
                        }

                        if (NULL != this.fnParser) {  
                            //! call handler
                            uint_fast16_t hwResult = this.fnParser(
                                            this.pchBuffer, this.hwLength);
                            if (hwResult) {
                                //! frame parsing success, reply data stored in buffer
                                this.hwLength = hwResult;
                                break;
                            } 
                        }
                        
                        //! send default reply: error
                        do {
                            this.pchBuffer[0] = ES_SIMPLE_FRAME_ERROR;
                            this.hwLength = 1;
                        } while (false);
                        break;
                    } else {
                        //! reset fsm
                        this.chState = WAIT_FOR_HEAD;
                    }
            
                }  
                
            } while (true);
        } else {
            
            if (SEND_REPLY_HEAD == this.chState) {
                //! write frame head
                if (this.ptPipe->WriteByte(ES_SIMPLE_FRAME_HEAD)) {
                    this.hwCheckSUM = CRC_INIT;
                    this.chState = SEND_REPLY_LENGTH_LOW;
                }
            } 
            if (SEND_REPLY_LENGTH_LOW == this.chState) {
                //! write frame length low
                uint8_t chData = this.hwLength & 0xFF;
                if (this.ptPipe->WriteByte(chData)) {
                    //! calculate check sum
                    CRC(this.hwCheckSUM, chData);
                    this.chState = SEND_REPLY_LENGTH_HIGH;
                }
            }
            if (SEND_REPLY_LENGTH_HIGH == this.chState) {
                //! write frame length high
                uint8_t chData = this.hwLength >> 8;
                if (this.ptPipe->WriteByte(chData)) {
                    //! calculate check sum
                    CRC(this.hwCheckSUM, chData);
                    this.hwCounter = 0;
                    this.chState = SEND_REPLY_DATA;
                }
            }
            if (SEND_REPLY_DATA == this.chState) {
                do {
                    //! write data
                    uint8_t chData = this.pchBuffer[this.hwCounter];
                    if (this.ptPipe->WriteByte(chData)) {
                        //! calculate check sum
                        CRC(this.hwCheckSUM, chData);

                        this.hwCounter++;
                        if (this.hwCounter >= this.hwLength) {
                            this.chState = SEND_REPLY_CHECK_SUM_L;
                            break;
                        }                    
                    } else {
                        break;
                    }
                } while (true);
            }
            if (SEND_REPLY_CHECK_SUM_L == this.chState) {
                //! write check sum low
                if (this.ptPipe->WriteByte(this.hwCheckSUM & 0xFF)) {
                    this.chState = SEND_REPLY_CHECK_SUM_H;
                }
            }
            if (SEND_REPLY_CHECK_SUM_H == this.chState) {
                //! write check sum high
                if (this.ptPipe->WriteByte(this.hwCheckSUM >> 8)) {
                    this.chState = WAIT_FOR_HEAD;
                    return fsm_rt_cpl;
                }
            }
        }
    } while (false);
    
    return fsm_rt_on_going;
}


fsm_initialiser(es_simple_frame_encoder,
    args(
        i_byte_pipe_t *ptPipe
    ))
        
    init_body(
       
        if (NULL == ptPipe) {
            abort_init();
        } else if ((NULL == ptPipe->ReadByte) || (NULL == ptPipe->WriteByte)) {
            abort_init();
        }
        memset(ref_obj_as(this, __es_simple_frame_fsm_internal), 0, sizeof(this));
        this.ptPipe = ptPipe;
    )

        
fsm_implementation(es_simple_frame_encoder,
    args(
        uint8_t *pchData, uint_fast16_t hwSize
    ))
    def_states(SEND_HEAD, SEND_LENGTH_L, SEND_LENGTH_H, SEND_DATA, SEND_CRC_L, SEND_CRC_H)
    
    uint8_t chData;
        
    body (
        on_start(
            if (NULL == pchData || 0 == hwSize) {
                fsm_report(GSF_ERR_INVALID_PARAMETER);
            } else if (NULL == this.ptPipe) {
                fsm_report(GSF_ERR_IO)
            } else if (NULL == this.ptPipe->WriteByte) {
                fsm_report(GSF_ERR_IO)
            }
            this.pchBuffer = pchData;
            this.hwLength = hwSize;
            this.hwCounter = hwSize;
            this.hwCheckSUM = CRC_INIT;
        )
            
            
        state(SEND_HEAD,
            if (!this.ptPipe->WriteByte(ES_SIMPLE_FRAME_HEAD)) {
                fsm_on_going();
            }
            
            update_state_to(SEND_LENGTH_L);
        )
            
        state(SEND_LENGTH_L,
            chData = ((uint8_t *)&this.hwLength)[0];
            if (!this.ptPipe->WriteByte(chData)) {
                fsm_on_going();
            }
            CRC(this.hwCheckSUM, chData);
            update_state_to(SEND_LENGTH_H);
        )
            
        state(SEND_LENGTH_H,
            chData = ((uint8_t *)&this.hwLength)[1];
            if (!this.ptPipe->WriteByte(chData)) {
                fsm_on_going();
            }
            CRC(this.hwCheckSUM, chData);
            update_state_to(SEND_DATA);
        )
            
        state(SEND_DATA,
            chData = *this.pchBuffer;
            if (!this.ptPipe->WriteByte(chData)) {
                fsm_on_going();
            }
            CRC(this.hwCheckSUM, chData);
            
            this.pchBuffer++;
            this.hwCounter--;
            if (0 == this.hwCounter) {
                update_state_to(SEND_CRC_L);
            } else {
                fsm_on_going();
            }
        )
            
            
        state(SEND_CRC_L,
            if (!this.ptPipe->WriteByte(((uint8_t *)&this.hwCheckSUM)[0])) {
                fsm_on_going();
            }
            
            update_state_to(SEND_CRC_H);
        )
            
        state(SEND_CRC_H,
            if (!this.ptPipe->WriteByte(((uint8_t *)&this.hwCheckSUM)[1])) {
                fsm_on_going();
            }
            
            fsm_cpl();
        )
        
    )

#endif

/* EOF */

