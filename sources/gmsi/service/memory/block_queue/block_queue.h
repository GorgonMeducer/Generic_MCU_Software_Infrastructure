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
#include "..\block\block.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
    
/*============================ TYPES =========================================*/

//! \brief stream buffer control block
//! @{
declare_class(stream_buffer_t)

declare_class(block_queue_t)
extern_class(block_queue_t)                   //!< inherit from pool StreamBufferBlock
    block_t                  *ptListHead;                                       //!< Queue Head
    block_t                  *ptListTail;                                       //!< Queue Tail
    uint32_t                 wCount;
end_extern_class(block_queue_t)

def_interface(i_block_queue_t)
    bool        (*Init)     (block_queue_t *ptObj);
    bool        (*Enqueue)  (block_queue_t *ptObj, block_t *ptItem);
    block_t *   (*Dequeue)  (block_queue_t *ptObj);
    uint32_t    (*Count)    (block_queue_t *ptObj);
end_def_interface(i_block_queue_t)

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_block_queue_t BLOCK_QUEUE;

/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
