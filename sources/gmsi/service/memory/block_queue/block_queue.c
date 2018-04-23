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

#if USE_SERVICE_BLOCK != ENABLED
#error The block_queue service requires the Block Service, please set USE_SERVICE_BLOCK to ENABLED in top app_cfg.h
#endif

#include "..\block\block.h"
#include <string.h>
/*============================ MACROS ========================================*/

#ifndef __BQ_ATOM_ACCESS
#   define __BQ_ATOM_ACCESS(...)            SAFE_ATOM_CODE(__VA_ARGS__)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(block_queue_t)
def_class(block_queue_t)                            //!< inherit from pool StreamBufferBlock
    block_t                  *ptListHead;                 //!< Queue Head
    block_t                  *ptListTail;                 //!< Queue Tail
    uint32_t                 wCount;
end_def_class(block_queue_t)

def_interface(i_block_queue_t)
    bool        (*Init)     (block_queue_t *ptObj);
    bool        (*Enqueue)  (block_queue_t *ptObj, block_t *ptItem);
    block_t *   (*Dequeue)  (block_queue_t *ptObj);
    uint32_t    (*Count)    (block_queue_t *ptObj);
end_def_interface(i_block_queue_t)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/ 

private bool block_queue_init(block_queue_t *ptObj);
private bool append_item_to_list(block_queue_t *ptObj, block_t *ptItem);
private uint32_t get_item_count(block_queue_t *ptObj);
private block_t *get_item_from_list(block_queue_t *ptObj);

                                   
/*============================ IMPLEMENTATION ================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if defined(LIB_GENERATION)
ROOT
#endif
const i_block_queue_t BLOCK_QUEUE = {
    .Init =         &block_queue_init,
    .Enqueue =      &append_item_to_list,
    .Dequeue =      &get_item_from_list,
    .Count =        &get_item_count,
};



private bool block_queue_init(block_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, block_queue_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        memset(ptThis, 0, sizeof(block_queue_t));
        
        return true;
    } while(false);
    
    return false;
    
}

private bool append_item_to_list(block_queue_t *ptObj, block_t *ptItem)
{
    class_internal(ptObj, ptThis, block_queue_t);
    
    if (NULL == ptThis || NULL == ptItem) {
        return false;
    }

    __BQ_ATOM_ACCESS (
        LIST_QUEUE_ENQUEUE(this.ptListHead, this.ptListTail, ptItem);
        this.wCount++;
    )

    return true;
}

private uint32_t get_item_count(block_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, block_queue_t);
    uint32_t wResult;
    
    __BQ_ATOM_ACCESS (
        wResult = this.wCount;
    )
    
    return wResult;
}

private block_t *get_item_from_list(block_queue_t *ptObj)
{
    class_internal(ptObj, ptThis, block_queue_t);
    block_t *ptResult = NULL;

    if (NULL == ptThis) {
        return NULL;
    }

    __BQ_ATOM_ACCESS (
        if (NULL != this.ptListHead) {
            LIST_QUEUE_DEQUEUE(this.ptListHead, this.ptListTail, ptResult);
            this.wCount--;
        }
    )

    
    return ptResult;
}

#endif
/* EOF */
