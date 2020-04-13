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
#include "..\..\..\memory\block\block.h"
#include "./es_simple_frame.h"
/*============================ MACROS ========================================*/

//! \brief es-simple-frame-head
#ifndef ES_SIMPLE_FRAME_HEAD
#   define ES_SIMPLE_FRAME_HEAD    0xAA
#endif
#ifndef ES_SIMPLE_FRAME_ERROR   
#   define ES_SIMPLE_FRAME_ERROR   0xF0
#endif

#ifndef this
#   define this         (*ptThis)
#endif
#ifndef base
#   define base         (*ptBase)
#endif
#ifndef target
#   define target       (*ptTarget)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES Part One ================================*/

def_simple_fsm(es_simple_frame_decoder,
    def_params(
        i_pipe_t *ptPipe;                   //!< pipe
        union {
            frame_parser_t *fnParser;       //!< parser
            frame_block_parser_t *fnBlockParser;
        };
        bool bUnsupportFrame;
        block_t *ptBlock;
        void *pTag;
        implement(__es_simple_frame_fsm_internal)
    ))
    
def_simple_fsm(es_simple_frame_encoder,
    def_params(
        i_pipe_t *ptPipe;                   //!< pipe
        implement(__es_simple_frame_fsm_internal)
    ))

def_simple_fsm(es_simple_frame_decoder_wrapper,
    def_params(
        es_simple_frame_t *ptFrame;
    ))

def_simple_fsm(es_simple_frame_encoder_wrapper,
    def_params(
        es_simple_frame_t *ptFrame;
    ))
 

//! \name class: e-snail simple frame
//! @{
def_class(es_simple_frame_t,,
    locker_t tMutex;
    inherit(fsm(es_simple_frame_decoder))
    inherit(fsm(es_simple_frame_decoder_wrapper))
    inherit(fsm(es_simple_frame_encoder))
    inherit(fsm(es_simple_frame_encoder_wrapper))
    bool    bDynamicBufferMode;
)
end_def_class(es_simple_frame_t)
//! @}


/*============================ PROTOTYPES ====================================*/

private extern_fsm_initialiser(es_simple_frame_decoder,
    args(
        i_pipe_t    *ptPipe, 
        void *fnParser,
        mem_block_t tMemory,
        block_t *ptBlock,
        void *pTag
    ))

private extern_fsm_initialiser(es_simple_frame_encoder,
    args(
        i_pipe_t    *ptPipe
    ))
    
private extern_fsm_initialiser(es_simple_frame_decoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))
    
private extern_fsm_initialiser(es_simple_frame_encoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))

private extern_fsm_implementation(es_simple_frame_encoder,
        args(
            uint8_t *pchData, uint_fast16_t hwSize
        ));

private extern_fsm_implementation(es_simple_frame_decoder);

private extern_fsm_implementation(es_simple_frame_encoder_wrapper,
    args(
        uint8_t *pchBuffer, uint_fast16_t hwSize
    ));

private extern_fsm_implementation(es_simple_frame_decoder_wrapper);

private bool es_simple_frame_init(  
    es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
//private fsm_rt_t es_simple_frame_task(es_simple_frame_t *ptFrame);

private fsm_rt_t encoder(es_simple_frame_t *ptFrame, uint8_t *pchBuffer, uint_fast16_t hwSize);
private fsm_rt_t task(es_simple_frame_t *ptFrame);
private fsm_rt_t decoder(es_simple_frame_t *ptFrame);
private uint_fast16_t get_buffer_size ( es_simple_frame_t *ptFrame);

/*============================ TYPES Part Two ================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
#if defined(LIB_GENERATION)
ROOT
#endif
const i_es_simple_frame_t ES_SIMPLE_FRAME = {
        .Init =         &es_simple_frame_init,
        .Task =         &task,
        .Decoder =      &decoder,
        .Encoder =      &encoder,
        .Buffer = {
            .GetSize =  &get_buffer_size,
        },
    };
    
/*============================ IMPLEMENTATION ================================*/


private uint_fast16_t get_buffer_size ( es_simple_frame_t *ptFrame)
{
    uint_fast32_t wSize = 0;
    class_internal(ptFrame, ptThis, es_simple_frame_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        class_internal( ref_obj_as(this, fsm(es_simple_frame_decoder)), 
                        ptTarget, 
                        fsm(es_simple_frame_decoder));
        
        if (NULL == target.ptBlock ) {
            wSize = target.nSize;
        } else {
            wSize = BLOCK.Size.Capability(target.ptBlock);
        }
    } while(false);
     
    return wSize; 
}


/*! \brief initlialize es_simple_frame
 *! \param ptFrame es_simple_frame object
 *! \param ptCFG configuration object
 *! \retval true initliazation is successful
 *! \retval false failed in initialization
 */
private bool es_simple_frame_init(  
    es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG)
{
    class_internal(ptFrame, ptThis, es_simple_frame_t);
    
    if (NULL == ptFrame || NULL == ptCFG) {
        return false;
    } else if (    
            (NULL == ptFrame) 
        ||  (NULL == ptCFG->ptPipe)
        //||  (NULL == ptCFG->pchBuffer)
        //|| (0 == ptCFG->hwSize )
        ||  (NULL == ptCFG->fnParser)) {
        return false;
    } else if (     (NULL == ptCFG->ptPipe->ReadByte) 
                ||  (NULL == ptCFG->ptPipe->WriteByte)
                ||  (NULL == ptCFG->ptPipe->Stream.Read)
                ||  (NULL == ptCFG->ptPipe->Stream.Write)) {
        return false;
    }
    
    memset((void *)ptFrame, 0, sizeof(es_simple_frame_t));
    
    init_lock(&this.tMutex);
    
    do {
        if (!ptCFG->bStaticBufferMode) {
            this.bDynamicBufferMode = true;
            //! dynamic buffer mode
            if (NULL == ptCFG->ptBlock) {
                break;
            } else if (!BLOCK.Size.Get(ptCFG->ptBlock)) {
                //! empty memory block
                break;
            }
            
            if (NULL == init_fsm(es_simple_frame_decoder, 
                            ref_obj_as( this, fsm(es_simple_frame_decoder)), 
                            args(ptCFG->ptPipe, ptCFG->fnParser),
                            (mem_block_t){NULL ,0},
                            ptCFG->ptBlock, ptCFG->pTag)) {
                break;
            }
            
        } else if (0 == ptCFG->nSize) {
            //! static buffer mode, empty buffer detected
            break;
        } else {        
            this.bDynamicBufferMode = false;
            if (NULL == init_fsm(es_simple_frame_decoder, 
                            ref_obj_as( this, fsm(es_simple_frame_decoder)), 
                            args(ptCFG->ptPipe, ptCFG->fnParser),
                            obj_convert_as((*ptCFG), mem_block_t),
                            NULL, ptCFG->pTag)) {
                break;
            }
        }

        if (NULL == init_fsm(es_simple_frame_encoder, 
                    ref_obj_as( this, fsm(es_simple_frame_encoder)), 
                    args(ptCFG->ptPipe))) {
            break;
        }             
        if (NULL == init_fsm(es_simple_frame_encoder_wrapper, 
                    ref_obj_as( this, fsm(es_simple_frame_encoder_wrapper)), 
                    args(ptFrame))) {
            break;
        } 
        if (NULL == init_fsm(es_simple_frame_decoder_wrapper, 
                    ref_obj_as( this, fsm(es_simple_frame_decoder_wrapper)), 
                    args(ptFrame))) {
            break;
        } 
        
        return true;
    } while(false);

    return false;
}

private fsm_initialiser(es_simple_frame_decoder,
    args(
        i_pipe_t *ptPipe, 
        void *fnParser,
        mem_block_t tMemory,
        block_t *ptBlock,
        void *pTag
    ))
        
    init_body(
       
        if (NULL == ptPipe || NULL == fnParser) {
            abort_init();
        } else if ((NULL == ptPipe->ReadByte) || (NULL == ptPipe->WriteByte)) {
            abort_init();
        } else if (    (NULL == ptBlock) 
                    && (    (NULL == tMemory.pchBuffer) 
                        ||  (0 == tMemory.nSize)) ) {
            abort_init();
        }
        
        memset( ref_obj_as(this, __es_simple_frame_fsm_internal), 0, 
                sizeof(__es_simple_frame_fsm_internal));
                
        if (NULL == ptBlock) {
            obj_convert_as(this, mem_block_t) = tMemory;
            this.fnParser = (frame_parser_t *)fnParser;
        } else {
            this.ptBlock = ptBlock;
            this.fnBlockParser = (frame_block_parser_t *)fnParser;
        }
        
        this.ptPipe = ptPipe;
        this.pTag = pTag; 
        
    )

        
private fsm_rt_t decoder(es_simple_frame_t *ptFrame)
{
    class_internal( ptFrame, ptThis, es_simple_frame_t);
    if (NULL == ptFrame) {
        return (fsm_rt_t)(GSF_ERR_INVALID_PTR);
    }
    
    return call_fsm(es_simple_frame_decoder, ref_obj_as(this, fsm(es_simple_frame_decoder)));
} 
        
        
private fsm_implementation(es_simple_frame_decoder) 
{
        
    def_states (
         WAIT_FOR_HEAD,
         WAIT_FOR_LENGTH_L,
         WAIT_FOR_LENGTH_H,
         WAIT_FOR_DATA,
         WAIT_FOR_CHECK_SUM_L,
         WAIT_FOR_CHECK_SUM_H    
    )    
    uint8_t chData;
      
    body_begin();
    
        on_start(
        
            if (NULL != this.ptBlock) {
                this.pchBuffer = BLOCK.Buffer.Get(this.ptBlock);
                BLOCK.Size.Reset(this.ptBlock);
                this.nSize = BLOCK.Size.Get(this.ptBlock);
            }
        
            if (NULL == this.ptPipe) {
                fsm_report(GSF_ERR_IO)
            } else if (NULL == this.ptPipe->ReadByte) {
                fsm_report(GSF_ERR_IO)
            } else if (NULL == this.pchBuffer) {
                fsm_report(GSF_ERR_INVALID_PTR);
            } else if (0 == this.nSize) {
                fsm_report(GSF_ERR_INVALID_PARAMETER);
            }
            this.hwCheckSUM = CRC_INIT;
            this.bUnsupportFrame = false;
            
            
        )
            
        privilege_state(WAIT_FOR_HEAD, 
            if (!this.ptPipe->ReadByte(&chData)) {
                fsm_wait_for_obj();
            }
            
            if (ES_SIMPLE_FRAME_HEAD == chData) {
                update_state_to(WAIT_FOR_LENGTH_L);
            }
        )        
        
        state(WAIT_FOR_LENGTH_L) {
            
            if (!this.ptPipe->ReadByte(&chData)) {
                fsm_wait_for_obj();
            }
            
            GMSI_CRC(this.hwCheckSUM, chData);
            ((uint8_t *)&this.hwLength)[0] = chData;
            
            update_state_to(WAIT_FOR_LENGTH_H);
        }
            
        state(WAIT_FOR_LENGTH_H) {
            
            if (!this.ptPipe->ReadByte(&chData)) {
                fsm_wait_for_obj();
            }
            
            GMSI_CRC(this.hwCheckSUM, chData);
            ((uint8_t *)&this.hwLength)[1] = chData;
            
            if (0 == this.hwLength){
                /* no data */
                transfer_to(WAIT_FOR_CHECK_SUM_L);
            } else if (this.hwLength > this.nSize) {
                //! data is too big 
                this.bUnsupportFrame = true;
            } 
            this.hwCounter = 0;
            
            update_state_to(WAIT_FOR_DATA);
        }

        state(WAIT_FOR_DATA) {
            int32_t nRead = 
            this.ptPipe->Stream.Read(this.pchBuffer + this.hwCounter, 
                                     this.hwLength - this.hwCounter);
            if ( nRead > 0) {
                
                crc16_stream_check(&this.hwCheckSUM, 
                                    this.pchBuffer + this.hwCounter, 
                                    nRead);
                                    
                this.hwCounter += nRead;
                
                if (this.hwCounter >= this.hwLength) {
                    //! get all data
                    update_state_to(WAIT_FOR_CHECK_SUM_L);
                }
            }
            
            fsm_wait_for_obj();
        }

        state(WAIT_FOR_CHECK_SUM_L) {
            
            if (!this.ptPipe->ReadByte(&chData)) {
                fsm_wait_for_obj();
            }
            if (!(((uint8_t *)&this.hwCheckSUM)[0] == chData)) {
                reset_fsm();
                fsm_on_going();
            }
            
            update_state_to(WAIT_FOR_CHECK_SUM_H);
        }
        
            
        
        state(WAIT_FOR_CHECK_SUM_H) {
            
            if (!this.ptPipe->ReadByte(&chData)) {
                fsm_wait_for_obj();
            }
            if (!(((uint8_t *)&this.hwCheckSUM)[1] == chData)) {
                reset_fsm();
                fsm_on_going();
            }
                
            if (this.bUnsupportFrame) {
                //! report unsupported frame
                this.hwLength = 1;
                this.pchBuffer[0] = ES_SIMPLE_FRAME_ERROR;
            } else if (NULL == this.ptBlock){
                //! static buffer mode
                //! call parser
                this.hwLength = 
                    this.fnParser(  obj_convert_as(this, mem_block_t), 
                                    this.hwLength);
            } else {
                block_t *ptBlock = this.ptBlock;
                
                BLOCK.Size.Set(ptBlock, this.hwLength);
                
                //! dynamic buffer mode
                ptBlock = this.fnBlockParser(ptBlock, this.pTag);

                do {
                    if (NULL == ptBlock) {
                        this.hwLength = 0;
                        break;
                    } 
                    
                    //! get reply 
                    this.ptBlock = ptBlock;
                    this.hwLength = BLOCK.Size.Get(ptBlock);
                    this.pchBuffer = BLOCK.Buffer.Get(ptBlock);
                } while(false);
                 
            }
            fsm_cpl();
        }
            
        
    body_end();
}
                
private fsm_initialiser(es_simple_frame_decoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))
    init_body(
        if (NULL == ptFrame) {
            abort_init();
        }
        
        this.ptFrame = ptFrame;
    )          

private fsm_rt_t task(es_simple_frame_t *ptFrame)
{
    class_internal( ptFrame, ptThis, es_simple_frame_t);
    if (NULL == ptFrame) {
        return (fsm_rt_t)(GSF_ERR_INVALID_PTR);
    }
    
    return call_fsm(es_simple_frame_decoder_wrapper, 
                    &base_obj(fsm(es_simple_frame_decoder_wrapper)));
} 
                
private fsm_implementation(es_simple_frame_decoder_wrapper)
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
                    &&  (ptDecoder->hwLength <= ptDecoder->nSize)) {
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

            
            
private fsm_initialiser(es_simple_frame_encoder,
    args(
        i_pipe_t *ptPipe
    ))
        
    init_body(
       
        if (NULL == ptPipe) {
            abort_init();
        } else if ((NULL == ptPipe->ReadByte) || (NULL == ptPipe->WriteByte)) {
            abort_init();
        }
        memset(ref_obj_as(this, __es_simple_frame_fsm_internal), 0, sizeof(__es_simple_frame_fsm_internal));
        this.ptPipe = ptPipe;
    )

        

        
private fsm_implementation(es_simple_frame_encoder,
    args(
        uint8_t *pchData, uint_fast16_t hwSize
    ))
{
    def_states(SEND_HEAD, SEND_LENGTH_L, SEND_LENGTH_H, SEND_DATA, SEND_CRC_L, SEND_CRC_H)
    
    uint8_t chData;
        
    body_begin()
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
            this.hwCounter = 0;
            this.hwCheckSUM = CRC_INIT;
            
            update_state_to(SEND_HEAD);
        )
            
        state(SEND_HEAD) {
            if (!this.ptPipe->WriteByte(ES_SIMPLE_FRAME_HEAD)) {
                fsm_on_going();
            }
            
            update_state_to(SEND_LENGTH_L);
        }
            
        state(SEND_LENGTH_L) {
            chData = ((uint8_t *)&this.hwLength)[0];
            if (!this.ptPipe->WriteByte(chData)) {
                fsm_on_going();
            }
            GMSI_CRC(this.hwCheckSUM, chData);
            update_state_to(SEND_LENGTH_H);
        }
            
        state(SEND_LENGTH_H) {
            chData = ((uint8_t *)&this.hwLength)[1];
            if (!this.ptPipe->WriteByte(chData)) {
                fsm_on_going();
            }
            GMSI_CRC(this.hwCheckSUM, chData);
            update_state_to(SEND_DATA);
        }

        state(SEND_DATA) {
            int32_t nWrite = this.ptPipe->Stream.Write( this.pchBuffer + this.hwCounter, 
                                                        this.hwLength - this.hwCounter);
            if ( nWrite > 0) {
                
                crc16_stream_check(&this.hwCheckSUM, 
                                    this.pchBuffer + this.hwCounter, 
                                    nWrite);
                                    
                this.hwCounter += nWrite;
                
                if (this.hwCounter >= this.hwLength) {
                    //! get all data
                    update_state_to(SEND_CRC_L);
                }
            }
            
            fsm_wait_for_obj();
        }
            
        state(SEND_CRC_L) {
            if (!this.ptPipe->WriteByte(((uint8_t *)&this.hwCheckSUM)[0])) {
                fsm_on_going();
            }
            
            update_state_to(SEND_CRC_H);
        }
            
        state(SEND_CRC_H) {
            if (!this.ptPipe->WriteByte(((uint8_t *)&this.hwCheckSUM)[1])) {
                fsm_on_going();
            }
            
            fsm_cpl();
        }
        
    body_end();
}
            
   
            
        
private fsm_initialiser(es_simple_frame_encoder_wrapper,
    args(
        es_simple_frame_t *ptFrame
    ))
    init_body(
        if (NULL == ptFrame) {
            abort_init();
        }
        
        this.ptFrame = ptFrame;
    )
            
        
private fsm_rt_t encoder(es_simple_frame_t *ptFrame, uint8_t *pchBuffer, uint_fast16_t hwSize)
{
    class_internal( ptFrame, ptThis, es_simple_frame_t);
    if (NULL == ptFrame) {
        return (fsm_rt_t)(GSF_ERR_INVALID_PTR);
    }
    
    return call_fsm(es_simple_frame_encoder_wrapper, 
                    &base_obj(fsm(es_simple_frame_encoder_wrapper)), 
                    args(pchBuffer, hwSize));
}         
        
private fsm_implementation(es_simple_frame_encoder_wrapper,
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

