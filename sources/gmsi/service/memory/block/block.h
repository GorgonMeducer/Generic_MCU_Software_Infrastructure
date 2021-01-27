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
#include "../epool/epool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/   
#define __BLOCK_FREE_TO(__N, __NUM)            BLOCK_FREE_TO_##__N = __N+1,
 
/*============================ TYPES =========================================*/

enum {
    BLOCK_FREE_TO_ANY = 0,
    MACRO_REPEAT(254, __BLOCK_FREE_TO, NULL)
    BLOCK_NO_FREE     = 255
};

declare_class(block_t)

//! \name special methods for accessing blocks
//! \note the instance of i_block_methods_t should stored in ROM
//! @{
def_interface(i_block_methods_t)

    void (*Free)(void *, block_t *ptBlock);         //!< User specified free 
    int_fast32_t (*Read)(   void *pTarget, 
                            void *pBlockBuffer,
                            void *pBuff,            
                            int_fast32_t wSize, 
                            uint_fast32_t wOffset);
                            
    int_fast32_t (*Write)(  void *pTarget, 
                            void *pBlockBuffer,
                            const void *pBuff, 
                            int_fast32_t nSize, 
                            uint_fast32_t wOffset);

end_def_interface(i_block_methods_t)
//! @}

typedef struct {
    void *pTarget;
    uint_fast8_t chID;
    const i_block_methods_t *ptMethods;             //!< block methods
} block_adapter_t;

//! \brief fixed memory block used as stream buffer
//! @{

extern_class(block_t,
    which(implement(__single_list_node_t)),
    
    uint8_t  *pchBuffer;                            //!< buffer address

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

    uint32_t    Size                        : 24;
    uint32_t    chAdapterID                 : 8;
)
end_extern_class(block_t)
//! @}

declare_class(block_pool_t)

extern_class(block_pool_t, 
    which( inherit(pool_t) ),   
    block_adapter_t *ptAdapter;
)
end_extern_class(block_pool_t)


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
    uint_fast8_t    chAdapterID;
} block_cfg_t;

typedef struct {
    block_adapter_t *ptAdapter;
}block_pool_cfg_t;

def_interface(i_block_t)
    
    struct {
        bool            (*Init)    (block_pool_t *, block_pool_cfg_t *ptCFG);
        bool            (*Add)     (block_pool_t *,
                                    void *, 
                                    uint_fast32_t, 
                                    uint_fast32_t);
        block_t*        (*New)     (block_pool_t *);
        void            (*Free)    (block_pool_t *, block_t *);
        uint_fast32_t   (*Count)   (block_pool_t *ptObj);
    } Heap;
    block_t *           (*Init)    (block_t *ptBlock, block_cfg_t *ptCFG);
    
    struct {
        void            (*Register)(block_adapter_t *ptAdaptors,
                                    uint_fast8_t chSize);
    } Adapter;
    
    struct {
        uint_fast32_t   (*Get)     (block_t *);
        void            (*Set)     (block_t *, uint_fast32_t);
        void            (*Reset)   (block_t *);
        uint_fast32_t   (*Capability)(block_t *);
    } Size;
    struct {
        void *          (*Get)(block_t *);
        bool            (*Write)   (block_t *ptObj, 
                                    const void *pchSrc, 
                                    int_fast32_t nSize, 
                                    uint_fast32_t wOffsite);
        mem_block_t     (*Read)    (block_t *ptObj, 
                                    void *pchSrc, 
                                    int_fast32_t nSize, 
                                    uint_fast32_t wOffsite);
    } Buffer;
    
end_def_interface(i_block_t)
extern const i_block_t BLOCK;

/*============================ PROTOTYPES ====================================*/


#endif
#endif
/* EOF */
