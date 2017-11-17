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
#include "..\block_queue\block_queue.h"

#include <string.h>
/*============================ MACROS ========================================*/

#if USE_SERVICE_BLOCK_QUEUE != ENABLED
#error The Stream2Block service requires the Block Queue Service, please set USE_SERVICE_BLOCK_QUEUE to ENABLED in top app_cfg.h
#endif
#if USE_SERVICE_BLOCK != ENABLED
#error The Stream2Block service requires the Block Service, please set USE_SERVICE_BLOCK to ENABLED in top app_cfg.h
#endif


#ifndef __SB_ATOM_ACCESS
#   define __SB_ATOM_ACCESS(...)            SAFE_ATOM_CODE(__VA_ARGS__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


//! \note no thread safe queue is required
DEF_QUEUE_U8(StreamBufferQueue, uint_fast16_t, bool)
    
END_DEF_QUEUE_U8(StreamBufferQueue)

//! \brief stream buffer control block
//! @{
declare_class(stream_buffer_t)

typedef void stream_buffer_req_event_t(stream_buffer_t *ptThis);


def_class(stream_buffer_t, 
    which(   
        inherit(block_queue_t)                                          //!< inherit from block_queue_t
        inherit(block_pool_t)
        inherit(QUEUE(StreamBufferQueue))                               //!< inherit from queue StreamBufferQueue
    )) 

    bool                                    bIsOutput;                  //!< direction
    bool                                    bIsQueueInitialised;        //!< Indicate whether the queue has been inialised or not
    
    block_t                                *ptUsedByQueue;              //!< buffer block used by queue
    block_t                                *ptUsedByOutside;            //!< buffer block lent out  
    stream_buffer_req_event_t              *fnRequestSend;              //!< callback for triggering the first output transaction
    stream_buffer_req_event_t              *fnRequestReceive; 

end_def_class(stream_buffer_t)
//! @}

typedef struct {
    enum {
        INPUT_STREAM = 0,
        OUTPUT_STREAM
    } tDirection;
    
    stream_buffer_req_event_t              *fnRequestHandler;
    
}stream_buffer_cfg_t;

def_interface(i_stream_buffer_t)

    bool        (*Init)         (stream_buffer_t *, stream_buffer_cfg_t *);
    bool        (*AddBuffer)    (stream_buffer_t *, void *, uint_fast16_t , uint_fast16_t );
        
    struct {
        bool    (*Read)         (stream_buffer_t *, uint8_t *);
        bool    (*Write)        (stream_buffer_t *, uint8_t);
        void    (*Flush)        (stream_buffer_t *ptObj);
    } Stream;
    
    struct {
        block_t *(*Exchange)     (stream_buffer_t *, block_t *);
        block_t *(*GetNext)      (stream_buffer_t *);
        void    (*Return)       (stream_buffer_t *, block_t *);
    } Block;

end_def_interface(i_stream_buffer_t)
    

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
static block_t *request_next_buffer_block(
                                    stream_buffer_t *ptObj, 
                                    block_t *ptOld);
static block_t *get_next_block(        stream_buffer_t *ptObj);
static void return_block(           stream_buffer_t *ptObj, block_t *ptOld);
static void stream_flush(           stream_buffer_t *ptObj);
                                    
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
            .Exchange = &request_next_buffer_block,
            .GetNext =  &get_next_block,
            .Return =   &return_block,
        },
    };





static bool stream_buffer_init(stream_buffer_t *ptObj, stream_buffer_cfg_t *ptCFG)
{
    bool bResult = false;
    do {
        class_internal(ptObj, ptThis, stream_buffer_t);
        
        if (NULL == ptObj || NULL == ptCFG) {
            break;
        } 
        
        __SB_ATOM_ACCESS(
            do {
                this.bIsOutput = (ptCFG->tDirection == OUTPUT_STREAM);
                this.bIsQueueInitialised = false;

                this.ptUsedByQueue = NULL;
                this.ptUsedByOutside = NULL;
                if (this.bIsOutput) {
                    this.fnRequestSend = ptCFG->fnRequestHandler;
                } else {
                    this.fnRequestReceive = ptCFG->fnRequestHandler;
                }
                               
                block_queue_init(ref_obj_as(this, block_queue_t));
                BLOCK.Heap.Init(ref_obj_as(this, block_pool_t));
                bResult = true;
            } while(false);
        )
    } while(false);
    
    return bResult;
}

static bool stream_buffer_add_heap( stream_buffer_t *ptObj, 
                                    void *pBuffer, 
                                    uint_fast16_t hwSize, 
                                    uint_fast16_t hwItemSize)
{
    class_internal(ptObj, ptThis, stream_buffer_t);
    
    if (NULL == ptThis) {
        return false;
    }
    
    return BLOCK.Heap.Add(    ref_obj_as(this, block_pool_t), 
                                    pBuffer, 
                                    hwSize, 
                                    hwItemSize );
}



static block_t *get_next_block(stream_buffer_t *ptObj)
{
    block_t *ptItem = NULL;
    class_internal(ptObj, ptThis, stream_buffer_t);
    
    do {
        
        if (NULL == ptThis) {
            break;
        }

        
        
        if (this.bIsOutput) {
            //! find the next block from the list
            ptItem = get_item_from_list( ref_obj_as(this, block_queue_t));
        } else {
            
            //! get a new block
            ptItem = BLOCK.Heap.New( ref_obj_as(this, block_pool_t));
            
            if (NULL != ptItem) {
                //! reset block size
                BLOCK.Size.Reset(ptItem);
                
            }
        }
        
        this.ptUsedByOutside = ptItem;
        
    } while(false);

    return ptItem;
}

static void return_block(stream_buffer_t *ptObj, block_t *ptItem)
{
    class_internal(ptObj, ptThis, stream_buffer_t);
    do {
        if (NULL == ptThis || NULL == ptItem) {
            break;
        }
       
            
        if (this.bIsOutput) {
            
            //! reset block size
            BLOCK.Size.Reset(ptItem);
            //! stream is used for output
            BLOCK.Heap.Free(ref_obj_as(this, block_pool_t), ptItem);
            
        } else {

            //! stream is used for input
            //! add block to the list
            append_item_to_list( ref_obj_as(this, block_queue_t), ptItem);
        }
    } while(false);

}

static block_t *request_next_buffer_block(stream_buffer_t *ptObj, block_t *ptOld)
{
    block_t *ptItem = NULL;
    class_internal(ptObj, ptThis, stream_buffer_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        if (this.bIsOutput) {
            
            if (NULL != ptOld) {
                //! reset block size
                BLOCK.Size.Reset(ptOld);
                //! stream is used for output
                BLOCK.Heap.Free(ref_obj_as(this, block_pool_t), ptOld);
            }
            //! find the next block from the list
            ptItem = get_item_from_list( ref_obj_as(this, block_queue_t));
        } else {
            if (NULL != ptOld) {
                //! stream is used for input
                //! add block to the list
                append_item_to_list(ref_obj_as(this, block_queue_t), ptOld);
            }
            //! get a new block
            ptItem = BLOCK.Heap.New( ref_obj_as(this, block_pool_t));
            
            if (NULL != ptItem) {
                //! reset block size
                BLOCK.Size.Reset(ptItem);
            }
        }
        
        this.ptUsedByOutside = ptItem;
        
    } while(false);

    return ptItem;
}

static bool queue_init(stream_buffer_t *ptObj, bool bIsStreamForRead)
{
    class_internal(ptObj, ptThis, stream_buffer_t);
    block_t *ptBlock;
    
    if (bIsStreamForRead) {
        
        if (NULL != this.ptUsedByQueue) {
            BLOCK.Size.Reset(this.ptUsedByQueue);

            BLOCK.Heap.Free( ref_obj_as(this, block_pool_t), this.ptUsedByQueue);
        }
        //! fetch a block from list, and initialise it as a full queue
        ptBlock = get_item_from_list( ref_obj_as(this, block_queue_t) );
        
        if ( NULL == this.ptUsedByOutside) {
            if (NULL != this.fnRequestReceive) {
                (*this.fnRequestReceive)(ptObj);
            }
        }
    } else {
        
        //! add buffer to block list ( which is used as output list)
        do {
            if (NULL == this.ptUsedByQueue) {
                break;
            }
            
            //! use queue count as actual size
            BLOCK.Size.Set( this.ptUsedByQueue, 
                            GET_QUEUE_COUNT( StreamBufferQueue, 
                                             REF_OBJ_AS(this, 
                                                QUEUE(StreamBufferQueue))));
            
            append_item_to_list( ref_obj_as(this, block_queue_t), this.ptUsedByQueue);
            
            
            if (NULL == this.ptUsedByOutside ) {
                //! this is no transaction, we need to trigger one
                if (NULL != this.fnRequestSend) {
                    (*this.fnRequestSend)(ptObj);
                }
            }
        } while(false);
        
        
        //! get a new block from heap and initialise it as an empty queue
        ptBlock = BLOCK.Heap.New( ref_obj_as(this, block_pool_t));
        
        if (NULL != ptBlock) {
            //! reset block size
            BLOCK.Size.Reset(ptBlock);
        }
        
    }

    if (NULL == ptBlock) {
        //! queue is empty
        return false;
    }
    
    this.ptUsedByQueue = (block_t *)ptBlock;
    
    QUEUE_INIT_EX(  StreamBufferQueue,                                          //!< queue name
                    REF_OBJ_AS(this, QUEUE(StreamBufferQueue)),                 //!< queue obj
                    (uint8_t *)(BLOCK.Buffer.Get(ptBlock)),                     //!< buffer
                    BLOCK.Size.Get(ptBlock),                                    //!< buffer size
                    bIsStreamForRead);                                          //!< intialize method (initialise as full or initialise as empty)
    
    
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
        if (0 == GET_QUEUE_COUNT(StreamBufferQueue, 
                            REF_OBJ_AS(this, QUEUE(StreamBufferQueue)))) {
            return ;
        }
        if (!queue_init(ptObj, false)) {
            //! queue is empty
            this.bIsQueueInitialised = false;
        }
        while( NULL != TYPE_CONVERT( &(this.ptUsedByOutside), block_t * volatile));
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