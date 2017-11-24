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
#define END_DEF_EPOOL(__NAME)
#define END_EXTERN_EPOOL(__NAME)

#ifndef __EPOOL_MUTEX_TYPE
#define __EPOOL_MUTEX_TYPE        bool
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

#define EPOOL_ITEM_COUNT(__NAME, __EPOOL)                                       \
            __NAME##_get_pool_item_count((__EPOOL))

#define EXTERN_EPOOL(__NAME, __TYPE)                                            \
DECLARE_CLASS(__NAME##_pool_item_t)                                             \
EXTERN_CLASS(__NAME##_pool_item_t)                                              \
    union {                                                                     \
        INHERIT(__single_list_node_t)                                           \
        __TYPE                  tMem;                                           \
    };                                                                          \
END_EXTERN_CLASS(__NAME##_pool_item_t)                                          \
                                                                                \
DECLARE_CLASS(__NAME##_pool_t)                                                  \
EXTERN_CLASS(__NAME##_pool_t)                                                   \
    INHERIT(pool_t)                                                             \
END_EXTERN_CLASS(__NAME##_pool_t)                                               \
                                                                                \
extern bool __NAME##_pool_init(__NAME##_pool_t *ptPool);                        \
extern bool __NAME##_pool_add_heap(                                             \
    __NAME##_pool_t *ptPool, void *ptBuffer, uint16_t tSize);                   \
extern __TYPE *__NAME##_pool_new(__NAME##_pool_t *ptPool);                      \
extern void __NAME##_pool_free(__NAME##_pool_t *ptPool, __TYPE *ptItem);        \
extern uint16_t __NAME##_get_pool_item_count(__NAME##_pool_t *ptPool);          \
extern __EPOOL_MUTEX_TYPE *__NAME##_pool_mutex(__NAME##_pool_t *ptPool);        

#define DEF_EPOOL(__NAME, __TYPE)                                               \
DECLARE_CLASS(__NAME##_pool_item_t)                                             \
DEF_CLASS(__NAME##_pool_item_t)                                                 \
    union {                                                                     \
        INHERIT(__single_list_node_t)                                           \
        __TYPE                  tMem;                                           \
    };                                                                          \
END_DEF_CLASS(__NAME##_pool_item_t)                                             \
                                                                                \
DECLARE_CLASS(__NAME##_pool_t)                                                  \
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
    __NAME##_pool_t *ptPool, void *ptBuffer, uint16_t tSize)                    \
{                                                                               \
    return pool_add_heap(   (pool_t *)ptPool, ptBuffer,                         \
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
uint16_t __NAME##_get_pool_item_count(__NAME##_pool_t *ptPool)                  \
{                                                                               \
    return pool_get_item_count((pool_t *)ptPool);                               \
}                                                                               \
                                                                                \
__EPOOL_MUTEX_TYPE *__NAME##_pool_mutex(__NAME##_pool_t *ptPool)                \
{                                                                               \
    return pool_get_mutex((pool_t *)ptPool);                                    \
}                                                                               

/*============================ TYPES =========================================*/
DECLARE_CLASS(pool_t)
EXTERN_CLASS(pool_t)
    __single_list_node_t    *ptFreeList;
    uint_fast16_t           tCounter;
    __EPOOL_MUTEX_TYPE      tMutex;
END_EXTERN_CLASS(pool_t)

typedef void pool_item_init_event_handler_t(void *pItem, uint_fast16_t hwItemSize);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern bool pool_init(pool_t *ptPool);
extern bool pool_add_heap(pool_t *ptPool, void *ptBuffer,
                   uint16_t tPoolSize, uint16_t hwItemSize);
extern bool pool_add_heap_ex( pool_t *ptPool, 
                    void *ptBuffer,
                    uint16_t hwPoolSize, 
                    uint16_t hwItemSize,
                    pool_item_init_event_handler_t *fnHandler);
extern void *pool_new(pool_t *ptPool);
extern void pool_free(pool_t *ptPool, void *ptItem);
extern uint16_t pool_get_item_count(pool_t *ptPool);
extern __EPOOL_MUTEX_TYPE *pool_get_mutex(pool_t *ptPool);


#endif
                    
