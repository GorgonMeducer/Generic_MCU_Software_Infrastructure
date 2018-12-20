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

#define __VSF_CLASS_IMPLEMENT 
#include "./__class_block.h"
#undef __VSF_CLASS_IMPLEMENT 

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
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
private block_t *init(block_t *ptBlock, block_cfg_t *ptCFG);
private bool write_block_buffer( block_t *ptObj, 
                                const void *pchSrc, 
                                uint_fast16_t hwSize, 
                                uint_fast16_t hwOffsite);
private uint32_t get_free_block_count(block_pool_t *ptObj);
private void __free_block(void *pTarget, block_t *ptBlock);

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

private const i_block_methods_t c_iDefaultMethods = {
    .Free = &__free_block,
};


private block_t *init(block_t *ptBlock, block_cfg_t *ptCFG)
{
    class_internal(ptBlock, ptThis, block_t);
    do {
        if (NULL == ptBlock || NULL == ptCFG) {
            break;
        } else if ((NULL == ptCFG->pBuffer) && (ptCFG->hwSize < sizeof(this))) {
            break;
        }


        this.IsReadOnly = ptCFG->bIsReadOnly ? 1 : 0;

        if (NULL != ptCFG->pBuffer) {
            this.pchBuffer = ptCFG->pBuffer;
            this.hwSize = ptCFG->hwSize;
        } else {
            this.pchBuffer = ((uint8_t *)&this)+sizeof(this);
            this.hwSize = ptCFG->hwSize - sizeof(this);
        }
        
        this.BlockSize = this.hwSize >> 1;
        this.ptAdapter = ptCFG->pAdapter;
        
    
    } while(false);
    
    return ptBlock;
}

private void reset_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return ;
    }
    if (!this.IsReadOnly) {
        this.hwSize = this.BlockSize << 1;
    }
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
        
        this.hwSize = hwSize+hwOffsite;
        bResult = true;
    } while(false);
    
    return bResult;
}

private void set_block_size(block_t *ptObj, uint32_t wSize)
{
    class_internal(ptObj, ptThis, block_t);
    
    if (NULL == ptThis) {
        return ;
    } else if (this.IsReadOnly) {
        return ;
    }
    
    this.hwSize = MIN(wSize, (this.BlockSize << 1));
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
    uint32_t wSize = 0;
    if (NULL == ptThis) {
        return 0;
    }
    
    if (this.IsReadOnly) {
        wSize = this.BlockSize << 1;
    } else {
        wSize = this.hwSize;
    }

    return wSize;
}


static void __free_block(void *pTarget, block_t *ptBlock)
{
    class_internal(pTarget, ptThis, block_pool_t);
    pool_free( REF_OBJ_AS(this, pool_t), ptBlock);
}



private bool block_pool_init(block_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, block_pool_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        this.tDefaultAdapter.pTarget = ptThis;
        this.tDefaultAdapter.ptMethods = &c_iDefaultMethods;
        
        //! initialise pool
        if (!pool_init(ref_obj_as(this, pool_t), ptThis)) {
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
    class_internal(ptItem, ptTarget, block_t);
    do {
        assert(ptThis != NULL);
        
        if ((NULL == ptItem) || (NULL == target.ptAdapter)) {
            break;
        }
          
        assert( (target.ptAdapter->ptMethods != NULL));
        
        if (NULL != target.ptAdapter->ptMethods->Free) {
            target.ptAdapter->ptMethods->Free(target.ptAdapter->pTarget, ptItem);
        };
        
    } while(false);
}


private void pool_item_init_event_handler(  void *pTarget, 
                                            void *ptItem, 
                                            uint_fast16_t hwItemSize)
{
    
    class_internal(pTarget, ptThis, block_pool_t);

    block_cfg_t tCFG = {
        NULL,                   //!< use the rest memory of the block as buffer
        hwItemSize,             //!< total block size
        false,                  //!< not readonly
        &this.tDefaultAdapter   //!< use default adapter
    };

    init((block_t *)ptItem, &tCFG);
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
