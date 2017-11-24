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

/*============================ MACROS ========================================*/

#ifndef __EPOOL_ATOM_ACCESS
#define __EPOOL_ATOM_ACCESS(...)       SAFE_ATOM_CODE(__VA_ARGS__)
#endif
#ifndef __EPOOL_MUTEX_TYPE
#define __EPOOL_MUTEX_TYPE              bool
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

DECLARE_CLASS(pool_t)
DEF_CLASS(pool_t)
    __single_list_node_t    *ptFreeList;
    uint_fast16_t           tCounter;
    __EPOOL_MUTEX_TYPE      tMutex;
END_DEF_CLASS(pool_t)

typedef void pool_item_init_event_handler_t(void *pItem, uint_fast16_t hwItemSize);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

bool pool_init(pool_t *ptPool)
{
    class_internal(ptPool, ptThis, pool_t);
    if (NULL == ptPool) {
        return false;
    }

    this.ptFreeList = NULL;
    this.tCounter   = 0;

    return true;
}



bool pool_add_heap_ex( pool_t *ptPool, 
                    void *ptBuffer,
                    uint16_t hwPoolSize, 
                    uint16_t hwItemSize,
                    pool_item_init_event_handler_t *fnHandler)
{
    class_internal(ptPool, ptThis, pool_t);
    
    if ((NULL == ptPool)
    ||  (NULL == ptBuffer)
    ||  (0 == hwPoolSize)
    ||  (sizeof(void *) > hwItemSize)
    ||  (hwPoolSize < hwItemSize)) {
        return false;
    }


    do {
        __EPOOL_ATOM_ACCESS(
            LIST_STACK_PUSH(this.ptFreeList, ptBuffer);
            this.tCounter++;
        )
        
        if (NULL != fnHandler) {
            (*fnHandler)(ptBuffer, hwItemSize);
        }
        ptBuffer = (void *)((uint8_t *)ptBuffer + hwItemSize);
        
        hwPoolSize -= hwItemSize;
        
    } while( hwPoolSize >= hwItemSize);


    return true;
}

bool pool_add_heap( pool_t *ptPool, 
                    void *ptBuffer,
                    uint16_t hwPoolSize, 
                    uint16_t hwItemSize)
{
    return pool_add_heap_ex(ptPool, ptBuffer, hwPoolSize, hwItemSize, NULL);
}

void *pool_new(pool_t *ptPool)
{
    __single_list_node_t *ptItem = NULL;
    CLASS(pool_t) *ptThis = (CLASS(pool_t) *)ptPool;
    
    if (NULL == ptPool) {
        return NULL;
    }

    __EPOOL_ATOM_ACCESS(
        do {
            if (NULL == ((CLASS(pool_t) *)ptPool)->ptFreeList) {
                break;
            }
            
            LIST_STACK_POP(this.ptFreeList, ptItem);
            
            this.tCounter--;
        } while (false);
    )

    return (void *)ptItem;
}

void pool_free(pool_t *ptPool, void *ptItem)
{
    class_internal(ptPool, ptThis, pool_t);
    
    if ((NULL == ptThis) || (NULL == ptItem)) {
        return;
    }

    __EPOOL_ATOM_ACCESS(
        LIST_STACK_PUSH(this.ptFreeList, ptItem);
        this.tCounter++;
    )
}

uint16_t pool_get_item_count(pool_t *ptPool)
{
    uint_fast16_t tCount = 0;
    class_internal(ptPool, ptThis, pool_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }

        tCount = this.tCounter;
    } while(false);
    return tCount;
}

__EPOOL_MUTEX_TYPE *pool_get_mutex(pool_t *ptPool)
{
    class_internal(ptPool, ptThis, pool_t);
    if (NULL == ptThis) {
        return NULL;
    }

    return &(this.tMutex);
}

/*EOF*/

