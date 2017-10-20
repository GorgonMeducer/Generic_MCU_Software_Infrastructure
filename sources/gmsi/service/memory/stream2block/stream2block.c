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

#ifndef __SB_ATOM_ACCESS
#   define __SB_ATOM_ACCESS(...)            SAFE_ATOM_CODE(__VA_ARGS__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{
DECLARE_CLASS(stream_buffer_block_t)
DEF_CLASS(stream_buffer_block_t)
    INHERIT(__single_list_node_t)
    uint32_t wBlockSize;
    union {
        uint32_t wSize;                                 //!< memory block
        uint32_t wBuffer;
    };
END_DEF_CLASS(stream_buffer_block_t);
//! @}


DEF_EPOOL(StreamBufferBlock, stream_buffer_block_t)

END_DEF_EPOOL(StreamBufferBlock)

//! \note no thread safe queue is required
DEF_QUEUE_U8(StreamBufferQueue, uint_fast16_t, bool)
    
END_DEF_QUEUE_U8(StreamBufferQueue)



//! \brief stream buffer control block
//! @{
DECLARE_CLASS(stream_buffer_t)

typedef void stream_buffer_req_transaction_event_t(stream_buffer_t *ptThis);


DEF_CLASS(stream_buffer_t, 
    WHICH(   
        INHERIT(pool_t)                                                 //!< inherit from pool StreamBufferBlock
        INHERIT(QUEUE(StreamBufferQueue))                               //!< inherit from queue StreamBufferQueue
    )) 

    bool                                    bIsOutput;                  //!< direction
    bool                                    bIsQueueInitialised;        //!< Indicate whether the queue has been inialised or not
    stream_buffer_block_t                  *ptListHead;                 //!< Queue Head
    stream_buffer_block_t                  *ptListTail;                 //!< Queue Tail
    stream_buffer_block_t                  *ptUsedByQueue;              //!< buffer block used by queue
    stream_buffer_block_t                  *ptUsedByOutside;            //!< buffer block lent out  
    stream_buffer_req_transaction_event_t  *fnRequestTransfer;

END_DEF_CLASS(stream_buffer_t)
//! @}

typedef struct {
    enum {
        INPUT_STREAM = 0,
        OUTPUT_STREAM
    } tDirection;
    
    stream_buffer_req_transaction_event_t   *fnRequestTransfer;
    
}stream_buffer_cfg_t;

DEF_INTERFACE(i_stream_buffer_t)

    bool (*Init)(stream_buffer_t *, stream_buffer_cfg_t *);
    bool (*AddBuffer)(stream_buffer_t *, void *, uint_fast16_t , uint_fast16_t );
        
    struct {
        bool (*Read)(stream_buffer_t *, uint8_t *);
        bool (*Write)(stream_buffer_t *, uint8_t);
        void (*Flush)(stream_buffer_t *ptObj);
    } Stream;
    
    struct {
        void *(*Exchange)(stream_buffer_t *, void *);
    } Block;

END_DEF_INTERFACE(i_stream_buffer_t)
    

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
    
static bool stream_buffer_init(     stream_buffer_t *ptObj, 
                                    stream_buffer_cfg_t *ptCFG);    
static bool stream_buffer_add_heap( stream_buffer_t *ptObj, 
                                    void *pBuffer, 
                                    uint_fast16_t hwSize, 
                                    uint_fast16_t hwItemSize);
static bool stream_read(            stream_buffer_t *ptObj, 
                                    uint8_t *pchData);
static bool stream_write(           stream_buffer_t *ptObj, 
                                    uint8_t chData);
static void *request_next_buffer_block(
                                    stream_buffer_t *ptObj, 
                                    void *ptOld);
static void stream_flush(stream_buffer_t *ptObj);
                                    
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/
    
const i_stream_buffer_t STREAM_BUFFER = {
        .Init =         &stream_buffer_init,
        .AddBuffer =    &stream_buffer_add_heap,
        .Stream = {
            .Read =     &stream_read,
            .Write =    &stream_write,
            .Flush =    &stream_flush,
        },
        .Block = {
            .Exchange =  &request_next_buffer_block,
        },
    };





static bool stream_buffer_init(stream_buffer_t *ptObj, stream_buffer_cfg_t *ptCFG)
{
    bool bResult = false;
    do {
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        
        if (NULL == ptObj || NULL == ptCFG) {
            break;
        } 
        
        __SB_ATOM_ACCESS(
            do {
                this.bIsOutput = (ptCFG->tDirection == OUTPUT_STREAM);
                this.bIsQueueInitialised = false;
                this.ptListHead = NULL;
                this.ptListTail = NULL;
                this.ptUsedByQueue = NULL;
                
                this.fnRequestTransfer = ptCFG->fnRequestTransfer;
                
                //! initialise pool
                if (!pool_init(REF_OBJ_AS(this, pool_t))) {
                    break;
                }                
                
                bResult = true;
            } while(false);
        )
    } while(false);
    
    return bResult;
}

static void pool_item_init_event_handler(void *ptItem, uint_fast16_t hwItemSize)
{
    CLASS(stream_buffer_block_t) *ptThis = (CLASS(stream_buffer_block_t) *)ptItem;
    if (NULL == ptItem) {
        return;
    }
    
    this.wBlockSize = hwItemSize - sizeof(stream_buffer_block_t);
    this.wSize = hwItemSize;
}

static bool stream_buffer_add_heap( stream_buffer_t *ptObj, 
                                    void *pBuffer, 
                                    uint_fast16_t hwSize, 
                                    uint_fast16_t hwItemSize)
{
    bool bResult = false;
    do {
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        
        if (    (NULL == ptObj) 
            ||  (NULL == pBuffer)
            ||  (hwSize < hwItemSize) 
            ||  (hwItemSize < sizeof(stream_buffer_block_t))
            ||  (0 == hwItemSize)) {
            break;
        } 
        
        bResult =   pool_add_heap_ex (
                        REF_OBJ_AS(this, pool_t), 
                        pBuffer, 
                        hwSize, 
                        hwItemSize,
                        &pool_item_init_event_handler
                    );
        
    } while(false);
    
    return bResult;
}

static void append_item_to_list(stream_buffer_t *ptObj, stream_buffer_block_t *ptItem)
{
    CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
    
#if false
    if (NULL == ptObj || NULL == ptItem) {
        return;
    }
#endif
    __SB_ATOM_ACCESS (
        LIST_QUEUE_ENQUEUE(this.ptListHead, this.ptListTail, ptItem);
    )
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
    

    __SB_ATOM_ACCESS (
        LIST_QUEUE_DEQUEUE(this.ptListHead, this.ptListTail, ptResult);
    )
    
    return ptResult;
}



static void *request_next_buffer_block(stream_buffer_t *ptObj, void *ptOld)
{
    void *pBlock = NULL;
    
    do {
        if (NULL == ptObj) {
            break;
        }
        CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
        stream_buffer_block_t *ptItem = NULL;
        stream_buffer_block_t *ptPrevious;
        if (NULL != ptOld) {
            ptPrevious = (stream_buffer_block_t *)((size_t)ptOld -                      //!< get the block header
                            (sizeof(stream_buffer_block_t) - sizeof(uint32_t)));        //!< calculate offset for stream_buffer_block_t
            
        }
        
        if (this.bIsOutput) {
            
            if (NULL != ptOld) {
                //! reset block size
                TYPE_CONVERT( ptPrevious, CLASS(stream_buffer_block_t) ).wSize = 
                    TYPE_CONVERT( ptPrevious, CLASS(stream_buffer_block_t) ).wBlockSize;
                //! stream is used for output
                pool_free(REF_OBJ_AS(this, pool_t), ptPrevious);
            }
            //! find the next block from the list
            ptItem = get_item_from_list(ptObj);
        } else {
            if (NULL != ptOld) {
                //! stream is used for input
                //! add block to the list
                append_item_to_list(ptObj, ptPrevious);
            }
            //! get a new block
            ptItem = (stream_buffer_block_t *)pool_new( REF_OBJ_AS(this, pool_t));
            
            if (NULL != ptItem) {
                //! reset block size
                TYPE_CONVERT( ptItem, CLASS(stream_buffer_block_t) ).wSize = 
                    TYPE_CONVERT( ptItem, CLASS(stream_buffer_block_t) ).wBlockSize;
            }
        }
        
        this.ptUsedByOutside = ptItem;
        if (NULL != ptItem) {
            pBlock = (void *)&(((CLASS(stream_buffer_block_t) *)ptItem)->wBuffer);
        }
    } while(false);

    return pBlock;
}

static bool queue_init(stream_buffer_t *ptObj, bool bIsStreamForRead)
{
    CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
    CLASS(stream_buffer_block_t) *ptBlock;
    
    if (bIsStreamForRead) {
        
        TYPE_CONVERT( this.ptUsedByQueue, CLASS(stream_buffer_block_t) ).wSize = 
                TYPE_CONVERT( this.ptUsedByQueue, CLASS(stream_buffer_block_t) ).wBlockSize;
        pool_free( REF_OBJ_AS(this, pool_t), this.ptUsedByQueue);
        
        //! fetch a block from list, and initialise it as a full queue
        ptBlock = (CLASS(stream_buffer_block_t) *)get_item_from_list(ptObj);
    } else {
        
        //! add buffer to block list ( which is used as output list)
        do {
            if (NULL == this.ptUsedByQueue) {
                break;
            }
            
            //! use queue count as actual size
            TYPE_CONVERT(this.ptUsedByQueue, CLASS(stream_buffer_block_t)).wSize = 
                GET_QUEUE_COUNT( StreamBufferQueue, REF_OBJ_AS(this, QUEUE(StreamBufferQueue)));
    
            append_item_to_list( ptObj, this.ptUsedByQueue);
            
            
            if (NULL == this.ptUsedByOutside ) {
                //! this is no transaction, we need to trigger one
                if (NULL != this.fnRequestTransfer) {
                    this.fnRequestTransfer(ptObj);
                }
            }
        } while(false);
        
        
        //! get a new block from heap and initialise it as an empty queue
        ptBlock = (CLASS(stream_buffer_block_t) *)pool_new( REF_OBJ_AS(this, pool_t));
        if (NULL != ptBlock) {
            //! reset block size
            ptBlock->wSize = ptBlock->wBlockSize;
        }
        
    }

    if (NULL == ptBlock) {
        //! queue is empty
        return false;
    }
    
    this.ptUsedByQueue = (stream_buffer_block_t *)ptBlock;
    
    QUEUE_INIT_EX(  StreamBufferQueue,                              //!< queue name
                    REF_OBJ_AS(this, QUEUE(StreamBufferQueue)),     //!< queue obj
                    (uint8_t *)(&(ptBlock->wBuffer)+1),             //!< buffer
                    ptBlock->wSize,                                 //!< buffer size
                    bIsStreamForRead);                              //!< intialize method (initialise as full or initialise as empty)
    
    
    this.bIsQueueInitialised = true;
    
    return true;
}

static void stream_flush(stream_buffer_t *ptObj)
{
    CLASS(stream_buffer_t) *ptThis = (CLASS(stream_buffer_t) *)ptObj;
    if (NULL == ptObj) {
        return ;
    }
    if (this.bIsOutput) {
        if (!queue_init(ptObj, false)) {
            //! queue is empty
            this.bIsQueueInitialised = false;
        }
    }
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
            
            if (!DEQUEUE(   StreamBufferQueue, 
                            REF_OBJ_AS(this, QUEUE(StreamBufferQueue)), pchData)) {
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