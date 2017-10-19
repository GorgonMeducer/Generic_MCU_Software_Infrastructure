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

#if USE_SERVICE_STREAM_TO_BLOCK == ENABLED
#include "..\epool\epool.h"

/*============================ MACROS ========================================*/

#ifndef STREAM_BUFFER_BLOCK_SIZE 
#   warning No defined STREAM_BUFFER_BLOCK_SIZE, default value 8 bytes is used
#   define STREAM_BUFFER_BLOCK_SIZE        (8)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{
DEF_STRUCTURE(stream_buffer_block_t)
    uint8_t chBuffer[STREAM_BUFFER_BLOCK_SIZE];                     //!< memory block
    stream_buffer_block_t *ptNext;                                  //!< list pointer 
END_DEF_STRUCTURE(stream_buffer_block_t);
//! @}

DEF_EPOOL(StreamBufferBlock, stream_buffer_block_t)

END_DEF_EPOOL(StreamBufferBlock)

DEF_SAFE_QUEUE_U8(StreamBufferQueue, uint_fast16_t, bool)
    
END_DEF_SAFE_QUEUE_U8(StreamBufferQueue)



//! \brief stream buffer control block
//! @{
DECLARE_CLASS(stream_buffer_t)
DEF_CLASS(stream_buffer_t, 
    WHICH(   
        INHERIT(EPOOL(StreamBufferBlock))                           //!< inherit from pool StreamBufferBlock
        INHERIT(QUEUE(StreamBufferQueue))                           //!< inherit from queue StreamBufferQueue
    )) 

    bool                                bIsOutput;                  //!< direction
    bool                                bIsQueueInitialised;        //!< Indicate whether the queue has been inialised or not
    stream_buffer_block_t              *ptListHead;                 //!< Queue Head
    stream_buffer_block_t              *ptListTail;                 //!< Queue Tail
    stream_buffer_block_t              *ptCurrent;                  

END_DEF_CLASS(stream_buffer_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


typedef struct {
    enum {
        INPUT_STREAM = 0,
        OUTPUT_STREAM
    } tDirection;
    
}stream_buffer_cfg_t;


static bool stream_buffer_init(stream_buffer_t *ptObj, stream_buffer_cfg_t *ptCFG)
{
    do {
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        if (NULL == ptObj || NULL == ptCFG) {
            break;
        }
        
        this.bIsOutput = (ptCFG->tDirection == OUTPUT_STREAM);
        this.bIsQueueInitialised = false;
        this.ptListHead = NULL;
        this.ptListTail = NULL;
        this.ptCurrent = NULL;
        
        /* TODO ... */
        return true;
    } while(false);
    
    return false;
}

static void append_item_to_list(stream_buffer_t *ptObj, stream_buffer_block_t *ptItem)
{
    CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
    
#if false
    if (NULL == ptObj || NULL == ptItem) {
        return;
    }
#endif

    
    if (NULL == this.ptListTail) {
        //! queue is empty
        this.ptListTail = ptItem;
        ptItem->ptNext = NULL;
        this.ptListHead = ptItem;
    } else {
        
        this.ptListTail->ptNext = ptItem;
        ptItem->ptNext = NULL;
        this.ptListTail = ptItem;
        
    } 
}

static stream_buffer_block_t *get_item_from_list(stream_buffer_t *ptObj)
{
    CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
    stream_buffer_block_t *ptResult;
#if false
    if (NULL == ptObj) {
        return;
    }
#endif
    do {
        ptResult = this.ptListHead;
        if (NULL == this.ptListHead) {
            break;
        }
        
        this.ptListHead = this.ptListHead->ptNext;
        if (NULL == this.ptListHead) {
            //! reach the final item
            this.ptListTail = NULL;
        }
        
    } while(false);
    
    return ptResult;
}



static stream_buffer_block_t *request_next_buffer_block(stream_buffer_t *ptObj, stream_buffer_block_t *ptOld)
{
    stream_buffer_block_t *ptBlock = NULL;
    
    do {
        if (NULL == ptObj || NULL == ptOld) {
            break;
        }
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        
        if (this.bIsOutput) {
            //! stream is used for output
            EPOOL_FREE( StreamBufferBlock, REF_OBJ_AS(this, EPOOL(StreamBufferBlock)), ptOld);
            
            //! find the next block from the list
            ptBlock = get_item_from_list(ptObj);
        } else {
            //! stream is used for input
            //! add block to the list
            append_item_to_list(ptObj, ptOld);
            
            //! get a new block
            ptBlock = EPOOL_NEW( StreamBufferBlock, REF_OBJ_AS(this, EPOOL(StreamBufferBlock)));
        }
        
    } while(false);
    
    return ptBlock;
}

static bool queue_init(stream_buffer_t *ptObj, bool bSerialised)
{
    CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
    stream_buffer_block_t *ptBlock;
    
    if (bSerialised) {
        
        EPOOL_FREE( StreamBufferBlock, REF_OBJ_AS(this, EPOOL(StreamBufferBlock)), this.ptCurrent);
        
        //! fetch a block from list
        ptBlock = get_item_from_list(ptObj);
    } else {
        //! get a new block from heap
        ptBlock = EPOOL_NEW( StreamBufferBlock, REF_OBJ_AS(this, EPOOL(StreamBufferBlock)));
    }
    
    this.ptCurrent = ptBlock;
        
    if (NULL == ptBlock) {
        //! queue is empty
        return false;
    }
    
    QUEUE_INIT_EX( StreamBufferQueue, REF_OBJ_AS(this, QUEUE(StreamBufferQueue)), ptBlock->chBuffer, sizeof(ptBlock->chBuffer), bSerialised);
    
    
    this.bIsQueueInitialised = true;
    
    return true;
}



static bool stream_read(stream_buffer_t *ptObj, uint8_t *pchData)
{
    do {
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        if (NULL == ptObj || NULL == pchData) {
            break;
        } else if (this.bIsOutput) {
            break;
        }
        
        do {
            if (!this.bIsQueueInitialised) {
                if (!queue_init(ptObj, true)) {
                    //! queue is empty
                    return false;
                }
            }
            
            if (!DEQUEUE( StreamBufferQueue, REF_OBJ_AS(this, QUEUE(StreamBufferQueue)), pchData)) {
                this.bIsQueueInitialised = false;
            } else {
                return true;
            }
        } while(true);
        
    } while(false);
    
    return false;
}

static bool stream_write(stream_buffer_t *ptObj, uint8_t chData)
{
    do {
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        if (NULL == ptObj) {
            break;
        } else if (!this.bIsOutput) {
            break;
        }
        
        do {
            if (!this.bIsQueueInitialised) {
                if (!queue_init(ptObj, false)) {
                    //! queue is empty
                    return false;
                }
            }
            
            if (!ENQUEUE( StreamBufferQueue, REF_OBJ_AS(this, QUEUE(StreamBufferQueue)), chData)) {
                this.bIsQueueInitialised = false;
            } else {
                return true;
            }
        } while(true);
        
    } while(false);
    
    return false;
}




#endif
/* EOF */