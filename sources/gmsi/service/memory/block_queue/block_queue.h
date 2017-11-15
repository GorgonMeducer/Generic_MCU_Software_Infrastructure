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

#ifndef __BLOCK_QUEUE_H__
#define __BLOCK_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_SERVICE_BLOCK_QUEUE == ENABLED
#include "..\epool\epool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
    
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{
declare_class(block_t)
extern_class(block_t)
    inherit(__single_list_node_t)
    uint32_t wBlockSize;
    union {
        uint32_t wSize;                                                         //!< memory block
        uint32_t wBuffer;
    };
end_extern_class(block_t)
//! @}


EXTERN_EPOOL(StreamBufferBlock, block_t)

END_EXTERN_EPOOL(StreamBufferBlock)

//! \brief stream buffer control block
//! @{
declare_class(stream_buffer_t)

declare_class(block_queue_t)
extern_class(block_queue_t,    which( inherit(pool_t)))                         //!< inherit from pool StreamBufferBlock
    block_t                  *ptListHead;                                       //!< Queue Head
    block_t                  *ptListTail;                                       //!< Queue Tail
end_extern_class(block_queue_t,which( inherit(pool_t)))


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern bool block_queue_init(block_queue_t *ptObj);
extern bool block_queue_add_heap(   block_queue_t *ptObj, 
                                    void *pBuffer, 
                                    uint_fast16_t hwSize, 
                                    uint_fast16_t hwItemSize);
extern void append_item_to_list(block_queue_t *ptObj, block_t *ptItem);
extern block_t *get_item_from_list(block_queue_t *ptObj);
extern void reset_block_size(block_t *ptObj);
extern void *get_block_buffer(block_t *ptObj);
extern void set_block_size(block_t *ptObj, uint32_t wSize);
extern uint32_t get_block_size(block_t *ptObj);


#endif
#endif
/* EOF */