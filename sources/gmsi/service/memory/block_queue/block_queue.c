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

#if USE_SERVICE_BLOCK_QUEUE == ENABLED
#include "..\epool\epool.h"
#include <string.h>
/*============================ MACROS ========================================*/

#ifndef __BQ_ATOM_ACCESS
#   define __BQ_ATOM_ACCESS(...)            SAFE_ATOM_CODE(__VA_ARGS__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{
declare_class(block_t)
def_class(block_t)
    INHERIT(__single_list_node_t)
    uint32_t wBlockSize;
    union {
        uint32_t wSize;                                 //!< memory block
        uint32_t wBuffer;
    };
end_def_class(block_t);
//! @}


DEF_EPOOL(StreamBufferBlock, block_t)

END_DEF_EPOOL(StreamBufferBlock)


declare_class(block_queue_t)
def_class(block_queue_t,    which( inherit(pool_t)))                            //!< inherit from pool StreamBufferBlock
    block_t                  *ptListHead;                 //!< Queue Head
    block_t                  *ptListTail;                 //!< Queue Tail
end_def_class(block_queue_t,which( inherit(pool_t)))


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/                                    
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

void reset_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return ;
    }
    
    this.wSize = this.wBlockSize;
}

void *get_block_buffer(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return NULL;
    }
    
    return &this.wBuffer;
}

void set_block_size(block_t *ptObj, uint32_t wSize)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return ;
    }
    
    this.wSize = wSize;
}


uint32_t get_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return 0;
    }
    
    return this.wSize;
}


bool block_queue_init(block_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, block_queue_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        memset(ptThis, 0, sizeof(block_queue_t));
        
        //! initialise pool
        if (!pool_init(REF_OBJ_AS(this, pool_t))) {
            break;
        } 
        
        return true;
    } while(false);
    
    return false;
    
}

static void pool_item_init_event_handler(void *ptItem, uint_fast16_t hwItemSize)
{
    class_internal(ptItem, ptThis, block_t);
    if (NULL == ptThis) {
        return;
    }
    
    this.wBlockSize = hwItemSize - sizeof(block_t);
    this.wSize = hwItemSize;
}

bool block_queue_add_heap(   block_queue_t *ptObj, 
                                    void *pBuffer, 
                                    uint_fast16_t hwSize, 
                                    uint_fast16_t hwItemSize)
{
    bool bResult = false;
    class_internal(ptObj, ptThis, block_queue_t);
    
    do {
        if (    (NULL == ptThis) 
            ||  (NULL == pBuffer)
            ||  (hwSize < hwItemSize) 
            ||  (hwItemSize < sizeof(block_t))
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


void append_item_to_list(block_queue_t *ptObj, block_t *ptItem)
{
    class_internal(ptObj, ptThis, block_queue_t);
    
    if (NULL == ptThis || NULL == ptItem) {
        return;
    }

    __BQ_ATOM_ACCESS (
        LIST_QUEUE_ENQUEUE(this.ptListHead, this.ptListTail, ptItem);
    )
}

block_t *get_item_from_list(block_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, block_queue_t);
    block_t *ptResult;

    if (NULL == ptThis) {
        return NULL;
    }

    __BQ_ATOM_ACCESS (
        LIST_QUEUE_DEQUEUE(this.ptListHead, this.ptListTail, ptResult);
    )
    
    return ptResult;
}








#endif
/* EOF */