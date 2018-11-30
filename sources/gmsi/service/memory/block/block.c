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

#if USE_SERVICE_BLOCK == ENABLED
#include "..\epool\epool.h"
#include <string.h>

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \brief fixed memory block used as stream buffer
//! @{
declare_class(block_t)
def_class(block_t)
    implement(__single_list_node_t)
    uint8_t  *pchBuffer;                            //!< buffer address
    uint32_t IsReadOnly             : 1;
    uint32_t BlockSize              : 15;
    uint32_t Size                   : 16;           //!< memory block
end_def_class(block_t);
//! @}

declare_class(block_pool_t)

def_class(block_pool_t, which( inherit(pool_t) ))
end_def_class(block_pool_t, which( inherit(pool_t) ))


def_interface(i_block_t)
    
    struct {
        bool        (*Init) (block_pool_t *);
        bool        (*Add)(block_pool_t *,void *, uint_fast16_t, uint_fast16_t);
        block_t*    (*New)(block_pool_t *);
        void        (*Free)(block_pool_t *, block_t *);
        uint32_t    (*Count)(block_pool_t *ptObj);
    } Heap;
    block_t *       (*Init)(block_t *ptBlock, 
                            void *pBuffer, 
                            uint_fast16_t hwSize, 
                            bool bIsReadOnly);
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

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/   

private void reset_block_size(block_t *ptObj);
private void *get_block_buffer(block_t *ptObj);
private void set_block_size(block_t *ptObj, uint32_t wSize);
private uint32_t get_block_size(block_t *ptObj);
private uint32_t get_block_capability(block_t *ptObj);
private bool block_pool_init(block_pool_t *ptObj);
private block_t *new_block(block_pool_t *ptObj);
private void free_block(block_pool_t *ptObj, block_t *ptItem);
private bool block_pool_add_heap(  block_pool_t *ptObj, 
                            void *pBuffer, 
                            uint_fast16_t hwSize, 
                            uint_fast16_t hwItemSize);
private block_t *init(block_t *ptBlock, void *pBuffer, uint_fast16_t hwSize, bool bIsReadOnly);
private bool write_block_buffer( block_t *ptObj, 
                                const void *pchSrc, 
                                uint_fast16_t hwSize, 
                                uint_fast16_t hwOffsite);
private uint32_t get_free_block_count(block_pool_t *ptObj);
/*============================ GLOBAL VARIABLES ==============================*/
#if defined(LIB_GENERATION)
ROOT
#endif
const i_block_t BLOCK = {
    .Heap = {
        .Init =         &block_pool_init,
        .Add =          &block_pool_add_heap,
        .New =          &new_block,
        .Free =         &free_block,
        .Count =        &get_free_block_count,
    },
    .Init =             &init,
    .Size = {
        .Get =          &get_block_size,
        .Set =          &set_block_size,
        .Reset =        &reset_block_size,
        .Capability =   &get_block_capability,
    },
    .Buffer = {
        .Get =          &get_block_buffer,
        .Write =        &write_block_buffer,
    },
};

/*============================ IMPLEMENTATION ================================*/



private block_t *init(block_t *ptBlock, void *pBuffer, uint_fast16_t hwSize, bool bIsReadOnly)
{
    class_internal(ptBlock, ptThis, block_t);
    do {
        if (NULL == ptBlock || 0 == hwSize) {
            break;
        } else if ((NULL == pBuffer) && (hwSize < sizeof(this))) {
            break;
        }
        
        if (NULL != pBuffer) {
            this.pchBuffer = pBuffer;
            this.Size = hwSize;
        } else {
            this.pchBuffer = ((uint8_t *)&this)+sizeof(this);
            this.Size = hwSize - sizeof(this);
        }
        
        this.IsReadOnly = bIsReadOnly ? 1 : 0;
        this.BlockSize = this.Size >> 1;
        
    
    } while(false);
    
    return ptBlock;
}

private void reset_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return ;
    }
    
    this.Size = this.BlockSize << 1;
}

private void *get_block_buffer(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return NULL;
    }
    
    return this.pchBuffer;
}

private bool write_block_buffer( block_t *ptObj, 
                                const void *pchSrc, 
                                uint_fast16_t hwSize, 
                                uint_fast16_t hwOffsite)
{
    class_internal(ptObj, ptThis, block_t);
    bool bResult = false;
    
    do {
        if (NULL == ptThis || NULL == pchSrc || 0 == hwSize) {
            break;
        } else if (this.IsReadOnly) {
            break;
        }
        
        uint_fast16_t hwMaxSize = (this.BlockSize << 1) - hwOffsite;
        if (hwSize > hwMaxSize) {
            hwSize = hwMaxSize;
        }
        
        memcpy(((uint8_t *)get_block_buffer(ptObj))+hwOffsite, pchSrc, hwSize);
        
        this.Size = hwSize+hwOffsite;
        bResult = true;
    } while(false);
    
    return bResult;
}

private void set_block_size(block_t *ptObj, uint32_t wSize)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return ;
    }
    
    this.Size = MIN(wSize, (this.BlockSize << 1));
}

private uint32_t get_block_capability(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return 0;
    }
    
    return this.BlockSize << 1;
}

private uint32_t get_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return 0;
    }
    
    return this.Size;
}


private bool block_pool_init(block_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, block_pool_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        //! initialise pool
        if (!pool_init(ref_obj_as(this, pool_t))) {
            break;
        } 
        
        return true;
    } while(false);
    
    return false;
}

private uint32_t get_free_block_count(block_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, block_pool_t);
    if (NULL == ptThis) {
        return 0;
    }
    
    return pool_get_item_count(ref_obj_as(this, pool_t));
}

private block_t *new_block(block_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, block_pool_t);
    
    block_t *ptBlock = NULL;
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        ptBlock = (block_t *)pool_new( REF_OBJ_AS(this, pool_t));
        reset_block_size(ptBlock);
        
    } while(false);
    
    return ptBlock;
}

private void free_block(block_pool_t *ptObj, block_t *ptItem)
{
    class_internal(ptObj, ptThis, block_pool_t);
    class_internal(ptObj, ptTarget, block_t);
    do {
        if (NULL == ptThis || NULL == ptItem) {
            break;
        }
        
        if (target.IsReadOnly) {
            break;
        }
        
        pool_free( REF_OBJ_AS(this, pool_t), ptItem);
        
    } while(false);
}

private void pool_item_init_event_handler(void *ptItem, uint_fast16_t hwItemSize)
{
    
    class_internal(ptItem, ptThis, block_t);
    /*
    if (NULL == ptThis) {
        return;
    }
    
    this.BlockSize = (hwItemSize - sizeof(block_t)) >> 1;
    this.Size = hwItemSize;
    this.pchBuffer = ((uint8_t *)&this)+sizeof(this);
    */
    init(ptThis, NULL, hwItemSize, false);
}

private bool block_pool_add_heap(  block_pool_t *ptObj, 
                            void *pBuffer, 
                            uint_fast16_t hwSize, 
                            uint_fast16_t hwItemSize)
{
    bool bResult = false;
    class_internal(ptObj, ptThis, block_pool_t);
    
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
#endif
/* EOF */
