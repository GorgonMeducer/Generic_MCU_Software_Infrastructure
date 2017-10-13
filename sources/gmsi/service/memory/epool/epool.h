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

#ifndef __EPOOL_H__
#define __EPOOL_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define END_DEF_EPOOL


#ifndef __ATOM_ACCESS
#define __ATOM_ACCESS       SAFE_ATOM_CODE
#endif
#ifndef __MUTEX_TYPE
#define __MUTEX_TYPE        bool
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define EPOOL(__NAME)        __NAME##_pool_t

#define EPOOL_ITEM(__NAME)   __NAME##_pool_item_t

#define EPOOL_MUTEX(__NAME, __EPOOL)                                            \
            __NAME##_pool_mutex((__EPOOL))

#define EPOOL_INIT(__NAME, __EPOOL)                                             \
            __NAME##_pool_init((__EPOOL))

#define EPOOL_ADD_HEAP(__NAME, __EPOOL, __BUFFER, __SIZE)                       \
            __NAME##_pool_add_heap((__EPOOL), (__BUFFER), (__SIZE))

#define EPOOL_FREE(__NAME, __EPOOL, __ITEM)                                     \
            __NAME##_pool_free((__EPOOL), (__ITEM))

#define EPOOL_NEW(__NAME, __EPOOL)                                              \
            __NAME##_pool_new((__EPOOL))

#define EPOOL_ITEM_COUNT_ALLOCATED(__NAME, __EPOOL)                             \
            __NAME##_get_pool_item_count_allocated((__EPOOL))

#define EXTERN_EPOOL(__NAME, __TYPE)                                            \
EXTERN_CLASS(__NAME##_pool_item_t)                                              \
    union {                                                                     \
        INHERIT(pool_item_t)                                                    \
        __TYPE                  tMem;                                           \
    };                                                                          \
END_EXTERN_CLASS(__NAME##_pool_item_t)                                          \
                                                                                \
EXTERN_CLASS(__NAME##_pool_t)                                                   \
    INHERIT(pool_t)                                                             \
END_EXTERN_CLASS(__NAME##_pool_t)                                               \
                                                                                \
extern bool __NAME##_pool_init(__NAME##_pool_t *ptPool);                        \
extern bool __NAME##_pool_add_heap(                                             \
    __NAME##_pool_t *ptPool, __NAME##_pool_item_t *ptBuffer, uint16_t tSize);   \
extern __TYPE *__NAME##_pool_new(__NAME##_pool_t *ptPool);                      \
extern void __NAME##_pool_free(__NAME##_pool_t *ptPool, __TYPE *ptItem);        \
extern uint16_t __NAME##_get_pool_item_count_allocated(__NAME##_pool_t *ptPool);\
extern __MUTEX_TYPE *__NAME##_pool_mutex(__NAME##_pool_t *ptPool);              \

#define DEF_EPOOL(__NAME, __TYPE)                                               \
DEF_CLASS(__NAME##_pool_item_t)                                                 \
    union {                                                                     \
        INHERIT(pool_item_t)                                                    \
        __TYPE                  tMem;                                           \
    };                                                                          \
END_DEF_CLASS(__NAME##_pool_item_t)                                             \
                                                                                \
DEF_CLASS(__NAME##_pool_t)                                                      \
    INHERIT(pool_t)                                                             \
END_DEF_CLASS(__NAME##_pool_t)                                                  \
                                                                                \
bool __NAME##_pool_init(__NAME##_pool_t *ptPool)                                \
{                                                                               \
    return pool_init((pool_t *)ptPool);                                         \
}                                                                               \
                                                                                \
bool __NAME##_pool_add_heap(                                                    \
    __NAME##_pool_t *ptPool, __NAME##_pool_item_t *ptBuffer, uint16_t tSize)    \
{                                                                               \
    return pool_add_heap(   (pool_t *)ptPool, (void *)ptBuffer,                 \
                            tSize, sizeof(__NAME##_pool_item_t));               \
}                                                                               \
                                                                                \
__TYPE *__NAME##_pool_new(__NAME##_pool_t *ptPool)                              \
{                                                                               \
    return (__TYPE *)pool_new((pool_t *)ptPool);                                \
}                                                                               \
                                                                                \
void __NAME##_pool_free(__NAME##_pool_t *ptPool, __TYPE *ptItem)                \
{                                                                               \
    pool_free((pool_t *)ptPool, (void *)ptItem);                                \
}                                                                               \
                                                                                \
uint16_t __NAME##_get_pool_item_count_allocated(__NAME##_pool_t *ptPool)        \
{                                                                               \
    return pool_get_item_count_allocated((pool_t *)ptPool);                     \
}                                                                               \
                                                                                \
__MUTEX_TYPE *__NAME##_pool_mutex(__NAME##_pool_t *ptPool)                      \
{                                                                               \
    return pool_get_mutex((pool_t *)ptPool);                                    \
}                                                                               \

/*============================ TYPES =========================================*/
DECLARE_CLASS(pool_item_t)
EXTERN_CLASS(pool_item_t)
    pool_item_t             *ptNext;
END_EXTERN_CLASS(pool_item_t)

DECLARE_CLASS(pool_t)
EXTERN_CLASS(pool_t)
    pool_item_t             *ptFreeList;
    uint_fast16_t           tCounter;
    __MUTEX_TYPE            tMutex;
END_EXTERN_CLASS(pool_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern bool pool_init(pool_t *ptPool);
extern bool pool_add_heap(pool_t *ptPool, void *ptBuffer,
                   uint16_t tPoolSize, uint16_t hwItemSize);
extern void *pool_new(pool_t *ptPool);
extern void pool_free(pool_t *ptPool, void *ptItem);
extern uint16_t pool_get_item_count_allocated(pool_t *ptPool);
extern __MUTEX_TYPE *pool_get_mutex(pool_t *ptPool);

#endif
