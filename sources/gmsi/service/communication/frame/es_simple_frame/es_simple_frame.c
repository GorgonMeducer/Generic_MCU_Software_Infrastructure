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


/*============================ TYPES Part One ================================*/

def_structure(__es_simple_frame_fsm_internal)
    inherit(mem_block_t)
    
    uint16_t hwLength;
    uint16_t hwCounter;
    uint16_t hwCheckSUM;
end_def_structure(__es_simple_frame_fsm_internal)

simple_fsm(es_simple_frame_decoder,
    def_params(
        i_byte_pipe_t *ptPipe;          //!< pipe
        frame_parser_t *fnParser;       //!< parser
        bool bUnsupportFrame;
        inherit(__es_simple_frame_fsm_internal)
    ))
    
simple_fsm(es_simple_frame_encoder,
    def_params(
        i_byte_pipe_t *ptPipe;          //!< pipe
        inherit(__es_simple_frame_fsm_internal)
    ))

declare_class(es_simple_frame_t)

simple_fsm(es_simple_frame_decoder_wrapper,
    def_params(
        es_simple_frame_t *ptFrame;
    ))

simple_fsm(es_simple_frame_encoder_wrapper,
    def_params(
        es_simple_frame_t *ptFrame;
    ))
 

//! \name class: e-snail simple frame
//! @{
def_class(es_simple_frame_t)
    locker_t tMutex;
    
    inherit(fsm(es_simple_frame_decoder))
    inherit(fsm(es_simple_frame_decoder_wrapper))
    inherit(fsm(es_simple_frame_encoder))
    inherit(fsm(es_simple_frame_encoder_wrapper))
end_def_class(es_simple_frame_t)
//! @}

//! \name es-simple frame configuration structure
//! @{
typedef struct {
    i_byte_pipe_t   *ptPipe; 
    frame_parser_t  *fnParser;
    inherit(mem_block_t)
}es_simple_frame_cfg_t;
//! @}


/*============================ PROTOTYPES ====================================*/

extern_fsm_initialiser(es_simple_frame_decoder,
    args(
        i_byte_pipe_t *ptPipe, 
        frame_parser_t *fnParser,
        mem_block_t tMemory
    ))

extern_fsm_initialiser(es_simple_frame_encoder,
    args(
        i_byte_pipe_t *ptPipe
    ))
    
extern_fsm_initialiser(es_simple_frame_decoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))
    
extern_fsm_initialiser(es_simple_frame_encoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))

extern_fsm_implementation(es_simple_frame_encoder,
        args(
            uint8_t *pchData, uint_fast16_t hwSize
        ));

extern_fsm_implementation(es_simple_frame_decoder);

extern_fsm_implementation(es_simple_frame_encoder_wrapper,
    args(
        uint8_t *pchBuffer, uint_fast16_t hwSize
    ));

extern_fsm_implementation(es_simple_frame_decoder_wrapper);

extern bool es_simple_frame_init(  
    es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
extern fsm_rt_t es_simple_frame_task(es_simple_frame_t *ptFrame);

static fsm_rt_t encoder(es_simple_frame_t *ptFrame, uint8_t *pchBuffer, uint_fast16_t hwSize);
static fsm_rt_t task(es_simple_frame_t *ptFrame);
static fsm_rt_t decoder(es_simple_frame_t *ptFrame);
/*============================ TYPES Part Two ================================*/
//! \name frame interface
//! @{
def_interface(i_es_simple_frame_t)
    bool (*Init)(es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
    fsm_rt_t (*Task)(es_simple_frame_t *ptFrame);
    fsm_rt_t (*Decoder)(es_simple_frame_t *ptFrame);
    fsm_rt_t (*Encoder)(es_simple_frame_t *ptFrame, uint8_t *pchData, uint_fast16_t hwSize);
end_def_interface(i_es_simple_frame_t)
//! @}

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
    
const i_es_simple_frame_t ES_SIMPLE_FRAME = {
        .Init =         &es_simple_frame_init,
        .Task =         &task,
        .Decoder =      &decoder,
        .Encoder =      &encoder,
    
    };
    
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
    class_internal(ptFrame, ptThis, es_simple_frame_t);
    
    if (NULL == ptFrame || NULL == ptCFG) {
        return false;
    } else if (    
            (NULL == ptFrame) 
        ||  (NULL == ptCFG->ptPipe)
        ||  (NULL == ptCFG->pchBuffer)
        ||  (NULL == ptCFG->fnParser)
        || (0 == ptCFG->hwSize )) {
        return false;
    } else if ((NULL == ptCFG->ptPipe->ReadByte) || (NULL == ptCFG->ptPipe->WriteByte)) {
        return false;
    }
    
    init_lock(&this.tMutex);
    
    do {
        if (!init_fsm(es_simple_frame_decoder, 
                        ref_obj_as( this, fsm(es_simple_frame_decoder)), 
                        args(ptCFG->ptPipe, ptCFG->fnParser),
                        obj_convert_as((*ptCFG), mem_block_t))) {
            break;
        }
        if (!init_fsm(es_simple_frame_encoder, 
                    ref_obj_as( this, fsm(es_simple_frame_encoder)), 
                    args(ptCFG->ptPipe))) {
            break;
        }             
        if (!init_fsm(es_simple_frame_encoder_wrapper, 
                    ref_obj_as( this, fsm(es_simple_frame_encoder_wrapper)), 
                    args(ptFrame))) {
            break;
        } 
        if (!init_fsm(es_simple_frame_decoder_wrapper, 
                    ref_obj_as( this, fsm(es_simple_frame_decoder_wrapper)), 
                    args(ptFrame))) {
            break;
        } 
        
        return true;
    } while(false);

    return false;
}

fsm_initialiser(es_simple_frame_decoder,
    args(
        i_byte_pipe_t *ptPipe, 
        frame_parser_t *fnParser,
        mem_block_t tMemory
    ))
        
    init_body(
       
        if (NULL == ptPipe || NULL == fnParser) {
            abort_init();
        } else if ((NULL == ptPipe->ReadByte) || (NULL == ptPipe->WriteByte)) {
            abort_init();
        }
        memset(ref_obj_as(this, __es_simple_frame_fsm_internal), 0, sizeof(this));
        obj_convert_as(this, mem_block_t) = tMemory;
        this.ptPipe = ptPipe;
        this.fnParser = fnParser;
    )
        
static fsm_rt_t decoder(es_simple_frame_t *ptFrame)
{
    class_internal( ptFrame, ptThis, es_simple_frame_t);
    if (NULL == ptFrame) {
        fsm_report(GSF_ERR_INVALID_PTR);
    }
    
    return call_fsm(es_simple_frame_decoder, ref_obj_as(this, fsm(es_simple_frame_decoder)));
} 
        
fsm_implementation(es_simple_frame_decoder)
        
    def_states (
         WAIT_FOR_HEAD,
         WAIT_FOR_LENGTH_L,
         WAIT_FOR_LENGTH_H,
         WAIT_FOR_DATA,
         WAIT_FOR_CHECK_SUM_L,
         WAIT_FOR_CHECK_SUM_H    
    )    
    uint8_t chData;
        
    body (
        on_start(
            if (NULL == this.ptPipe) {
                fsm_report(GSF_ERR_IO)
            } else if (NULL == this.ptPipe->ReadByte) {
                fsm_report(GSF_ERR_IO)
            } else if (NULL == this.pchBuffer) {
                fsm_report(GSF_ERR_INVALID_PTR);
            } else if (0 == this.hwSize) {
                fsm_report(GSF_ERR_INVALID_PARAMETER);
            }
            this.hwCheckSUM = CRC_INIT;
            this.bUnsupportFrame = false;
        )
            
        privilege_group(

            state(WAIT_FOR_HEAD,
                if (!this.ptPipe->ReadByte(&chData)) {
                    fsm_report(fsm_rt_wait_for_obj);
                }
                
                if (ES_SIMPLE_FRAME_HEAD == chData) {
                    update_state_to(WAIT_FOR_LENGTH_L);
                } else {
                    fsm_on_going();
                }
            )
            
            state(WAIT_FOR_LENGTH_L,
                
                if (!this.ptPipe->ReadByte(&chData)) {
                    fsm_report(fsm_rt_wait_for_obj);
                }
                
                CRC(this.hwCheckSUM, chData);
                update_state_to(WAIT_FOR_LENGTH_H);
                
                ((uint8_t *)&this.hwLength)[0] = chData;
            )
                
            state(WAIT_FOR_LENGTH_H,
                
                if (!this.ptPipe->ReadByte(&chData)) {
                    fsm_report(fsm_rt_wait_for_obj);
                }
                
                CRC(this.hwCheckSUM, chData);
                ((uint8_t *)&this.hwLength)[1] = chData;
                
                if (0 == this.hwLength){
                    /* no data */
                    transfer_to(WAIT_FOR_CHECK_SUM_L);
                } else if (this.hwLength > this.hwSize) {
                    //! data is too big 
                    this.bUnsupportFrame = true;
                } 
                
                update_state_to(WAIT_FOR_DATA);
                this.hwCounter = 0;
            )
                
            state(WAIT_FOR_DATA,
                if (!this.ptPipe->ReadByte(&chData)) {
                    fsm_report(fsm_rt_wait_for_obj);
                }
                
                CRC(this.hwCheckSUM, chData);
                if (!this.bUnsupportFrame) {
                    this.pchBuffer[this.hwCounter++] = chData;
                }
                
                if (this.hwCounter >= this.hwLength) {
                    transfer_to(WAIT_FOR_CHECK_SUM_L);
                }
                
                fsm_continue();
            )
                
            state(WAIT_FOR_CHECK_SUM_L,
                
                if (!this.ptPipe->ReadByte(&chData)) {
                    fsm_report(fsm_rt_wait_for_obj);
                }
                
                if (!(((uint8_t *)&this.hwCheckSUM)[0] == chData)) {
                    reset_fsm();
                    fsm_on_going();
                }
                
                update_state_to(WAIT_FOR_CHECK_SUM_H);
            )
                
            state(WAIT_FOR_CHECK_SUM_H,
                
                if (!this.ptPipe->ReadByte(&chData)) {
                    fsm_report(fsm_rt_wait_for_obj);
                }
                
                
                if (!(((uint8_t *)&this.hwCheckSUM)[1] == chData)) {
                    reset_fsm();
                    fsm_on_going();
                }
                
                if (this.bUnsupportFrame) {
                    //! report unsupported frame
                    this.hwLength = 1;
                    this.pchBuffer[0] = ES_SIMPLE_FRAME_ERROR;
                } else {
                    //! call parser
                    this.hwLength = 
                        this.fnParser(  obj_convert_as(this, mem_block_t), 
                                        this.hwLength);
                }
                fsm_cpl();
            )
        
        )
    )
                
fsm_initialiser(es_simple_frame_decoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))
    init_body(
        if (NULL == ptFrame) {
            abort_init();
        }
        
        this.ptFrame = ptFrame;
    )          

static fsm_rt_t task(es_simple_frame_t *ptFrame)
{
    class_internal( ptFrame, ptThis, es_simple_frame_t);
    if (NULL == ptFrame) {
        fsm_report(GSF_ERR_INVALID_PTR);
    }
    
    return call_fsm(es_simple_frame_decoder_wrapper, 
                    &base_obj(fsm(es_simple_frame_decoder_wrapper)));
} 
                
fsm_implementation(es_simple_frame_decoder_wrapper)
    def_states(DECODING, TRY_TO_LOCK, REPLY)
                
    fsm_rt_t tFSMReply;
    class_internal( this.ptFrame, ptBase, es_simple_frame_t);
    class_internal( ref_obj_as(base, fsm(es_simple_frame_decoder)), 
                    ptDecoder, 
                    fsm(es_simple_frame_decoder));
                
    body(
        on_start(
            if (NULL == this.ptFrame) {
                fsm_report(fsm_rt_err)
            }
        )
        
        state(DECODING,
                
            tFSMReply = call_fsm(es_simple_frame_decoder, 
                                 ref_obj_as(base, fsm(es_simple_frame_decoder)));
            if (is_fsm_err(tFSMReply)) {
                fsm_report(tFSMReply);
            } else if (fsm_rt_cpl == tFSMReply) {
                
                
                if (    (ptDecoder->hwLength > 0) 
                    &&  (ptDecoder->hwLength <= ptDecoder->hwSize)) {
                    /* get something to reply */
                    transfer_to(TRY_TO_LOCK)
                }
                    
                fsm_cpl();
            }
            
            fsm_on_going();
        )
            
        state(TRY_TO_LOCK,
            if (!enter_lock(&base.tMutex)) {
                fsm_on_going();
            }
            
            update_state_to(REPLY);
        )
            
        state(REPLY,

            tFSMReply = call_fsm(   es_simple_frame_encoder, 
                                    ref_obj_as(base, fsm(es_simple_frame_encoder)),
                                    args(
                                        ptDecoder->pchBuffer,
                                        ptDecoder->hwLength
                                    ));
            if (is_fsm_err(tFSMReply)) {
                leave_lock(&base.tMutex);
                fsm_report(tFSMReply);
            } else if (fsm_rt_cpl == tFSMReply) {
                leave_lock(&base.tMutex);
                fsm_cpl();
            }
            
            fsm_on_going();
            
        )
    )

            
            
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

        privilege_state(SEND_DATA,
            
            chData = *this.pchBuffer;
            if (!this.ptPipe->WriteByte(chData)) {
                fsm_on_going();
            }
            CRC(this.hwCheckSUM, chData);
            
            this.pchBuffer++;
            this.hwCounter--;
            if (0 == this.hwCounter) {
                update_state_to(SEND_CRC_L);
            } /*else {
                fsm_on_going();
            } */
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
            
            
   
            
        
fsm_initialiser(es_simple_frame_encoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))
    init_body(
        if (NULL == ptFrame) {
            abort_init();
        }
        
        this.ptFrame = ptFrame;
    )
            
        
static fsm_rt_t encoder(es_simple_frame_t *ptFrame, uint8_t *pchBuffer, uint_fast16_t hwSize)
{
    class_internal( ptFrame, ptThis, es_simple_frame_t);
    if (NULL == ptFrame) {
        fsm_report(GSF_ERR_INVALID_PTR);
    }
    
    return call_fsm(es_simple_frame_encoder_wrapper, 
                    &base_obj(fsm(es_simple_frame_encoder_wrapper)), 
                    args(pchBuffer, hwSize));
}         
        
fsm_implementation(es_simple_frame_encoder_wrapper,
    args(
        uint8_t *pchBuffer, uint_fast16_t hwSize
    ))
    def_states(TRY_TO_LOCK, ENCODING)
                
    fsm_rt_t tFSMReply;
    class_internal( this.ptFrame, ptBase, es_simple_frame_t);
                
    body(
        on_start(
            if (    (NULL == this.ptFrame)
                ||  (NULL == pchBuffer)
                ||  (0 == hwSize)) {
                fsm_report(GSF_ERR_INVALID_PARAMETER)
            }
        )
  
        state(TRY_TO_LOCK,
            if (!enter_lock(&base.tMutex)) {
                fsm_on_going();
            }
            
            update_state_to(ENCODING);
        )
            
        state(ENCODING,

            tFSMReply = call_fsm(   es_simple_frame_encoder, 
                                    ref_obj_as(base, fsm(es_simple_frame_encoder)),
                                    args(
                                        pchBuffer, hwSize
                                    ));
            if (is_fsm_err(tFSMReply)) {
                leave_lock(&base.tMutex);
                fsm_report(tFSMReply);
            } else if (fsm_rt_cpl == tFSMReply) {
                leave_lock(&base.tMutex);
                fsm_cpl();
            }
            
            fsm_on_going();
            
        )
    )


#endif

/* EOF */

