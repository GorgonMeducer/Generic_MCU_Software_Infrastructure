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

static struct {
    block_adapter_t *ptAdapters;
    uint_fast8_t chLength;
}s_tBlockControl = {NULL, 0};

/*============================ PROTOTYPES ====================================*/   

private void reset_block_size(block_t *ptObj);
private void *get_block_buffer(block_t *ptObj);
private void set_block_size(block_t *ptObj, uint_fast32_t wSize);
private uint_fast32_t get_block_size(block_t *ptObj);
private uint_fast32_t get_block_capability(block_t *ptObj);
private bool block_pool_init(block_pool_t *ptObj, block_pool_cfg_t *ptCFG);
private block_t *new_block(block_pool_t *ptObj);
private void free_block(block_pool_t *ptObj, block_t *ptItem);
private bool block_pool_add_heap(  block_pool_t *ptObj, 
                            void *pBuffer, 
                            uint_fast32_t wSize, 
                            uint_fast32_t wItemSize);
private block_t *init(block_t *ptBlock, block_cfg_t *ptCFG);
private bool write_block_buffer(block_t *ptObj, 
                                const void *pchSrc, 
                                int_fast32_t nSize, 
                                uint_fast32_t wOffsite);
private mem_block_t  read_block_buffer(block_t *ptObj, 
                                void *pchSrc, 
                                int_fast32_t nSize, 
                                uint_fast32_t wOffsite);
private uint_fast32_t get_free_block_count(block_pool_t *ptObj);
private void __free_block(void *pTarget, block_t *ptBlock);
private void register_adaptors(block_adapter_t *ptAdaptors, uint_fast8_t chSize);

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
    .Adapter = {
        .Register =     &register_adaptors,
    },
    .Size = {
        .Get =          &get_block_size,
        .Set =          &set_block_size,
        .Reset =        &reset_block_size,
        .Capability =   &get_block_capability,
    },
    .Buffer = {
        .Get =          &get_block_buffer,
        .Write =        &write_block_buffer,
        .Read =         &read_block_buffer,
    },
};

/*============================ IMPLEMENTATION ================================*/

private const block_adapter_t* __get_adapter_interface(block_t *ptItem)
{
    class_internal(ptItem, ptThis, block_t);
    assert(NULL != ptThis);
    block_adapter_t *ptAdapter = NULL;
    
    do {
        uint_fast8_t chID = this.chAdapterID;
        
        if (0 == chID || 255 == chID) {
            break;
        }
        if (NULL == s_tBlockControl.ptAdapters) {
            break;
        }
        if (chID > s_tBlockControl.chLength) {
            break;
        }
        
        ptAdapter = &s_tBlockControl.ptAdapters[chID-1];
        assert(NULL != ptAdapter->ptMethods);
    } while(0);
    
    return ptAdapter;
}

private void register_adaptors(block_adapter_t *ptAdaptors, uint_fast8_t chSize)
{
    //!< this function could only be called once.
    assert(s_tBlockControl.ptAdapters == NULL);   
    
    s_tBlockControl.ptAdapters = ptAdaptors;
    s_tBlockControl.chLength = chSize;
}


private block_t *init(block_t *ptBlock, block_cfg_t *ptCFG)
{
    class_internal(ptBlock, ptThis, block_t);
    assert(NULL != ptBlock && NULL != ptCFG);
    
    do {
        if ((ptCFG->BlockSize < sizeof(this))) {
            break;
        }
        if ((!this.IsNoDirectAccess) && (NULL == ptCFG->pBuffer)) {
            break;
        }

        this.wCapability = ptCFG->wCapability;

        if (NULL != ptCFG->pBuffer) {
            this.pchBuffer = ptCFG->pBuffer;
            this.Size = ptCFG->BlockSize;
        } else {
            this.pchBuffer = ((uint8_t *)&this)+sizeof(this);
            this.Size = ptCFG->BlockSize - sizeof(this);
            this.BlockSize = this.Size;
        }

        this.chAdapterID = ptCFG->chAdapterID;
    } while(false);
    
    return ptBlock;
}

private void reset_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    assert(NULL != ptObj);
    
    if (!this.IsNoWrite) {      //!< readonly
        this.Size = this.BlockSize;
    }
}

private void *get_block_buffer(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    
    assert(NULL != ptObj);
    if (this.IsNoDirectAccess) {
        return NULL;
    }
    
    return this.pchBuffer;
}

private bool write_block_buffer( block_t *ptObj, 
                                const void *pchSrc, 
                                int_fast32_t nSize, 
                                uint_fast32_t wOffsite)
{
    class_internal(ptObj, ptThis, block_t);
    assert(NULL != ptObj && NULL != pchSrc);
    
    bool bResult = false;
    
    do {
        if (0 == nSize) {
            break;
        } else if (this.IsNoWrite) {
            break;
        }
        
        uint_fast32_t wMaxSize = this.BlockSize - wOffsite;
        if (nSize > wMaxSize) {
            break;                          //!< too big to write
        }
        
        if (this.IsNoDirectAccess) {
            const block_adapter_t *ptAdapter = __get_adapter_interface(ptObj);
            if (NULL == ptAdapter) {
                break;
            }
            if (NULL == ptAdapter->ptMethods->Write) {
                break;
            }
            
            nSize = (*ptAdapter->ptMethods->Write)(ptAdapter->pTarget, 
                                                  this.pchBuffer,
                                                  pchSrc,
                                                  nSize,
                                                  wOffsite);
            if (nSize < 0) {
                break;
            }

        } else {
            memcpy(((uint8_t *)get_block_buffer(ptObj))+wOffsite, pchSrc, nSize);
        }
        
        this.Size = nSize+wOffsite;
        bResult = true;
    } while(false);
    
    return bResult;
}

private mem_block_t read_block_buffer(  block_t *ptObj, 
                                        void *pchSrc, 
                                        int_fast32_t nSize, 
                                        uint_fast32_t wOffsite)
{
    class_internal(ptObj, ptThis, block_t);
    assert(NULL != ptObj && NULL != pchSrc);
    uint8_t *pchBuffer = NULL;
    mem_block_t tResult = {0};
    
    bool bResult = false;
    
    do {
        if (0 == nSize) {
            break;
        } else if (this.IsNoRead) {
            break;
        }
        
        uint_fast32_t wMaxSize = this.BlockSize - wOffsite;

        if (this.IsNoDirectAccess) {
            nSize = MIN(nSize, wMaxSize);
            
            const block_adapter_t *ptAdapter = __get_adapter_interface(ptObj);
            if (NULL == ptAdapter) {
                break;
            }
            if (NULL == ptAdapter->ptMethods->Read) {
                break;
            }
            
            nSize = (*ptAdapter->ptMethods->Read)(ptAdapter->pTarget, 
                                                  this.pchBuffer,
                                                  pchSrc,
                                                  nSize,
                                                  wOffsite);
            if (nSize < 0) {
                break;
            }
            pchBuffer = pchSrc;

        } else if (nSize <= wMaxSize) {
            pchBuffer = get_block_buffer(ptObj) + wOffsite;
        } else {
            nSize = wMaxSize;
            memcpy(pchSrc, pchBuffer+wOffsite, nSize);
            pchBuffer = pchSrc;
        }
        
        tResult.pchBuffer = pchBuffer;
        tResult.nSize = nSize;
        
    } while(false);
    
    return tResult;
}

private void set_block_size(block_t *ptObj, uint_fast32_t wSize)
{
    class_internal(ptObj, ptThis, block_t);
    assert(NULL != ptObj);
    
    if (this.IsNoWrite) {            //! readonly
        return ;
    }
    
    this.Size = MIN(wSize, this.BlockSize);
}

private uint_fast32_t get_block_capability(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    assert(NULL != ptObj);
    
    return this.BlockSize;
}

private uint_fast32_t get_block_size(block_t *ptObj)
{
    class_internal(ptObj, ptThis, block_t);
    assert(NULL != ptObj);

    return this.Size;
}

private bool block_pool_init(block_pool_t *ptObj, block_pool_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, block_pool_t);
    assert(NULL != ptObj);
    
    do {
        memset(ptObj, 0, sizeof(block_pool_t));
        
        if (NULL != ptCFG) {
            this.ptAdapter = ptCFG->ptAdapter;
        }
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
    assert(NULL != ptObj);
    
    return pool_get_item_count(ref_obj_as(this, pool_t));
}

private block_t *new_block(block_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, block_pool_t);
    assert(NULL != ptObj);
    
    block_t *ptBlock = (block_t *)pool_new( REF_OBJ_AS(this, pool_t));
    reset_block_size(ptBlock);

    return ptBlock;
}



private void free_block(block_pool_t *ptObj, block_t *ptItem)
{
    class_internal(ptObj, ptThis, block_pool_t);
    class_internal(ptItem, ptTarget, block_t);
    do {
        uint_fast8_t chID = target.chAdapterID;
        assert(ptThis != NULL);
        
        if (BLOCK_NO_FREE == chID) {
            return ;
        }
        const block_adapter_t *ptAdapter = __get_adapter_interface(ptItem);
        if (NULL == ptAdapter) {
            break;
        }
        if (NULL != ptAdapter->ptMethods->Free) {
            (*ptAdapter->ptMethods->Free)(ptAdapter->pTarget, ptItem);
        }
        return ;
    } while(false);
    pool_free( REF_OBJ_AS(this, pool_t), ptItem);
    
}


private void pool_item_init_event_handler(  void *pTarget, 
                                            void *ptItem, 
                                            uint_fast32_t wItemSize)
{
/*
typedef struct {
    void *          pBuffer;
    union {
        struct {
            uint32_t    BlockSize           : 24;
            uint32_t    IsNoWrite           : 1;
            uint32_t    IsNoRead            : 1;
            uint32_t    IsNoDirectAccess    : 1;
            uint32_t                        : 5;
        };
        uint32_t        wCapability;
    };
    uint_fast8_t    chAdapterIndex;
} block_cfg_t;
 */ 
 
    class_internal(pTarget, ptThis, block_pool_t);
    
    uint_fast8_t chID = BLOCK_FREE_TO_ANY;
    if (NULL != this.ptAdapter) {
        chID = this.ptAdapter->chID;
    }
    
    block_cfg_t tCFG = {
        NULL,                   //!< use the rest memory of the block as buffer
        wItemSize,              //!< total block size
        false,                  //!< allow write
        false,                  //!< allow read
        false,                  //!< allow direct access
        chID,     //!< free to current heap
    };

    init((block_t *)ptItem, &tCFG);
}

private bool block_pool_add_heap(  block_pool_t *ptObj, 
                            void *pBuffer, 
                            uint_fast32_t hwSize, 
                            uint_fast32_t hwItemSize)
{
    bool bResult = false;
    class_internal(ptObj, ptThis, block_pool_t);
    assert(NULL != ptObj && NULL != pBuffer);
    
    do {
        if (    (hwSize < hwItemSize) 
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
