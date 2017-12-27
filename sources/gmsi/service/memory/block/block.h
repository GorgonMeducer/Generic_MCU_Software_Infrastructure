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

#ifndef __BLOCK_H__
#define __BLOCK_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_SERVICE_BLOCK == ENABLED
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

declare_class(block_pool_t)

extern_class(block_pool_t, which( inherit(pool_t) ))
    // nothing here...
end_extern_class(block_pool_t, which( inherit(pool_t) ))

def_interface(i_block_t)
    
    struct {
        bool        (*Init) (block_pool_t *);
        bool        (*Add)(block_pool_t *,void *, uint_fast16_t, uint_fast16_t);
        block_t*    (*New)(block_pool_t *);
        void        (*Free)(block_pool_t *, block_t *);
        uint32_t    (*Count)(block_pool_t *ptObj);
    } Heap;
    block_t *       (*Init)(block_t *ptBlock, uint_fast16_t hwSize);
    struct {
        uint32_t    (*Get)(block_t *);
        void        (*Set)(block_t *, uint32_t);
        void        (*Reset)(block_t *);
        uint32_t    (*Capability)(block_t *);
    } Size;
    struct {
        void *      (*Get)(block_t *);
        bool        (*Write)    (   block_t *ptObj, 
                                    const void *pchSrc, 
                                    uint_fast16_t hwSize, 
                                    uint_fast16_t hwOffsite);
    } Buffer;
    
end_def_interface(i_block_t)

/*============================ GLOBAL VARIABLES ==============================*/

extern i_block_t BLOCK;

/*============================ PROTOTYPES ====================================*/


#endif
#endif
/* EOF */
