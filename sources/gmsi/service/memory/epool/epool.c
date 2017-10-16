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

#ifndef __ATOM_ACCESS
#define __ATOM_ACCESS       SAFE_ATOM_CODE
#endif
#ifndef __MUTEX_TYPE
#define __MUTEX_TYPE        bool
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define POOL_FREE_ITEM(__PPOOL, __PITEM)                                        \
    do {                                                                        \
        ((CLASS(pool_item_t) *)__PITEM)->ptNext =                               \
            ((CLASS(pool_t) *)__PPOOL)->ptFreeList;                             \
        ((CLASS(pool_t) *)__PPOOL)->ptFreeList  = __PITEM;                      \
        if (((CLASS(pool_t) *)__PPOOL)->tCounter) {                             \
            ((CLASS(pool_t) *)__PPOOL)->tCounter--;                             \
        }                                                                       \
    } while (false)


/*============================ TYPES =========================================*/
DECLARE_CLASS(pool_item_t)
DEF_CLASS(pool_item_t)
    pool_item_t             *ptNext;
END_DEF_CLASS(pool_item_t)

DECLARE_CLASS(pool_t)
DEF_CLASS(pool_t)
    pool_item_t             *ptFreeList;
    uint_fast16_t           tCounter;
    __MUTEX_TYPE            tMutex;
END_DEF_CLASS(pool_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

bool pool_init(pool_t *ptPool)
{
    if (NULL == ptPool) {
        return false;
    }

    ((CLASS(pool_t) *)ptPool)->ptFreeList = NULL;
    ((CLASS(pool_t) *)ptPool)->tCounter   = 0;

    return true;
}

bool pool_add_heap(pool_t *ptPool, void *ptBuffer,
                   uint16_t hwPoolSize, uint16_t hwItemSize)
{
    if ((NULL == ptPool)
    ||  (NULL == ptBuffer)
    ||  (0 == hwPoolSize)
    ||  (sizeof(void *) > hwItemSize)) {
        return false;
    }

    __ATOM_ACCESS(
        for (; hwPoolSize; --hwPoolSize) {
            POOL_FREE_ITEM(ptPool, ptBuffer);
            ptBuffer = (void *)((uint8_t *)ptBuffer + hwItemSize);
        }
    )

    return true;
}

void *pool_new(pool_t *ptPool)
{
    pool_item_t *ptItem = NULL;

    if (NULL == ptPool) {
        return NULL;
    }

    __ATOM_ACCESS(
        do {
            if (NULL == ((CLASS(pool_t) *)ptPool)->ptFreeList) {
                break;
            }
            ptItem = ((CLASS(pool_t) *)ptPool)->ptFreeList;
            ((CLASS(pool_t) *)ptPool)->ptFreeList  =
                ((CLASS(pool_item_t) *)ptItem)->ptNext;
            ((CLASS(pool_item_t) *)ptItem)->ptNext = NULL;
            ((CLASS(pool_t) *)ptPool)->tCounter++;
        } while (false);
    )

    return (void *)ptItem;
}

void pool_free(pool_t *ptPool, void *ptItem)
{
    if ((NULL == ptPool) || (NULL == ptItem)) {
        return;
    }

    __ATOM_ACCESS(
        POOL_FREE_ITEM(ptPool, ptItem);
    )
}

uint16_t pool_get_item_count_allocated(pool_t *ptPool)
{
    uint_fast16_t tCount;

    if (NULL == ptPool) {
        return 0;
    }

    __ATOM_ACCESS(
        tCount = ((CLASS(pool_t) *)ptPool)->tCounter;
    )

    return tCount;
}

__MUTEX_TYPE *pool_get_mutex(pool_t *ptPool)
{
    if (NULL == ptPool) {
        return NULL;
    }

    return &(((CLASS(pool_t) *)ptPool)->tMutex);
}

/*EOF*/

