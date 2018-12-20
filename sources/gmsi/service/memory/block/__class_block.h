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

//#ifndef __CLASS_BLOCK_H__             /* deliberately comment this out! */
//#define __CLASS_BLOCK_H__

/*============================ INCLUDES ======================================*/
#include "app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/    
/*============================ TYPES =========================================*/

declare_class(block_t)

//! \name special methods for accessing blocks
//! \note the instance of i_block_methods_t should stored in ROM
//! @{
def_interface(i_block_methods_t)

    void (*Free)(void *, block_t *ptBlock);         //!< User specified free 
    /* todo: Add special read / write interface */

end_def_interface(i_block_methods_t)
//! @}

typedef struct {
    void *pTarget;
    const i_block_methods_t *ptMethods;             //!< block methods
} block_adapter_t;

//! \brief fixed memory block used as stream buffer
//! @{

def_class(block_t)
    implement(__single_list_node_t)
    uint8_t  *pchBuffer;                            //!< buffer address
    block_adapter_t *ptAdapter;                     //!< block adapter
    
    uint16_t IsReadOnly             : 1;
    uint16_t BlockSize              : 15;
    uint16_t hwSize;                            //!< memory block
end_def_class(block_t);
//! @}

declare_class(block_pool_t)

def_class(block_pool_t, which( inherit(pool_t) ))   
    block_adapter_t tDefaultAdapter;
end_def_class(block_pool_t, which( inherit(pool_t) ))


typedef struct {
    void *                  pBuffer;
    uint_fast16_t           hwSize;
    bool                    bIsReadOnly;
    block_adapter_t *       pAdapter;
} block_cfg_t;


def_interface(i_block_t)
    
    struct {
        bool        (*Init) (block_pool_t *);
        bool        (*Add)(block_pool_t *,void *, uint_fast16_t, uint_fast16_t);
        block_t*    (*New)(block_pool_t *);
        void        (*Free)(block_pool_t *, block_t *);
        uint32_t    (*Count)(block_pool_t *ptObj);
    } Heap;
    block_t *       (*Init)(block_t *ptBlock, block_cfg_t *ptCFG);
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
/*============================ PROTOTYPES ====================================*/


//#endif                                /* deliberately comment this out! */

