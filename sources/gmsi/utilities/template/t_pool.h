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

#ifndef _USE_TEMPLATE_POOL_H_
#define _USE_TEMPLATE_POOL_H_

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#define END_DEF_POOL(__NAME)
#define END_DEF_SAFE_POOL(__NAME)

/*============================ MACROFIED FUNCTIONS ===========================*/
#ifndef NONE_ATOM_ACCESS
#   define NONE_ATOM_ACCESS(...)        {__VA_ARGS__;}
#endif

#define POOL(__NAME)        __NAME##_pool_t

#define POOL_ITEM(__NAME)   __NAME##_pool_item_t

#define DEF_SAFE_POOL(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)                 \
        DEF_POOL_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, SAFE_ATOM_CODE)

#define DEF_POOL(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)                      \
        DEF_POOL_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, NONE_ATOM_ACCESS)

#define POOL_MUTEX(__NAME, __POOL)                                              \
            __NAME##_pool_mutex((__POOL))

#define POOL_INIT(__NAME, __POOL)                                               \
            __NAME##_pool_init((__POOL))

#define POOL_ADD_HEAP(__NAME, __POOL, __BUFFER, __SIZE)                         \
            __NAME##_pool_add_heap((__POOL), (__BUFFER), (__SIZE))

#define POOL_FREE(__NAME, __POOL, __ITEM)                                       \
            __NAME##_pool_free((__POOL), (__ITEM))

#define POOL_NEW(__NAME, __POOL)                                                \
            __NAME##_pool_new((__POOL))

#define POOL_ITEM_COUNT_ALLOCATED(__NAME, __POOL)                               \
            __NAME##_get_pool_item_count_allocated((__POOL))

#define EXTERN_POOL(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)                   \
DECLARE_CLASS(__NAME##_pool_item_t)                                             \
EXTERN_CLASS(__NAME##_pool_item_t)                                              \
    union {                                                                     \
        INHERIT(__single_list_node_t)                                           \
        __TYPE                  tObject;                                        \
    };                                                                          \
EXTERN_DEF_CLASS(__NAME##_pool_item_t)                                          \
                                                                                \
DECLARE_CLASS(__NAME##_pool_t)                                                  \
EXTERN_CLASS(__NAME##_pool_t)                                                   \
    __single_list_node_t    *ptFreeList;                                        \
    __MUTEX_TYPE            tMutex;                                             \
    __PTR_TYPE              tCounter;                                           \
END_EXTERN_CLASS(__NAME##_pool_t)                                               \
                                                                                \
extern __MUTEX_TYPE *__NAME##_pool_mutex(__NAME##_pool_t *ptPool);              \
extern bool __NAME##_pool_init(__NAME##_pool_t *ptPool);                        \
extern __PTR_TYPE __NAME##_get_pool_item_count_allocated(                       \
    __NAME##_pool_t *ptPool);                                                   \
extern __TYPE *__NAME##_pool_new(__NAME##_pool_t *ptPool);                      \
extern void __NAME##_pool_free(                                                 \
    __NAME##_pool_t *ptPool, __TYPE *ptItem);                                   \
extern bool __NAME##_pool_add_heap(                                             \
    __NAME##_pool_t *ptPool, __NAME##_pool_item_t *ptBuffer, __PTR_TYPE tSize); 

#define DEF_POOL_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, __ATOM_ACCESS)    \
DECLARE_CLASS(__NAME##_pool_item_t)                                             \
DEF_CLASS(__NAME##_pool_item_t)                                                 \
    union {                                                                     \
        INHERIT(__single_list_node_t)                                           \
        __TYPE                  tObject;                                        \
    };                                                                          \
END_DEF_CLASS(__NAME##_pool_item_t)                                             \
                                                                                \
DECLARE_CLASS(__NAME##_pool_t)                                                  \
DEF_CLASS(__NAME##_pool_t)                                                      \
    __single_list_node_t    *ptFreeList;                                        \
    __MUTEX_TYPE            tMutex;                                             \
    __PTR_TYPE              tCounter;                                           \
END_DEF_CLASS(__NAME##_pool_t)                                                  \
                                                                                \
__MUTEX_TYPE *__NAME##_pool_mutex(__NAME##_pool_t *ptPool)                      \
{                                                                               \
    if (NULL == ptPool)  {                                                      \
        return NULL;                                                            \
    }                                                                           \
                                                                                \
    return &(((CLASS(__NAME##_pool_t) *)ptPool)->tMutex);                       \
}                                                                               \
                                                                                \
bool __NAME##_pool_init(__NAME##_pool_t *ptPool)                                \
{                                                                               \
    if (NULL == ptPool) {                                                       \
        return false;                                                           \
    }                                                                           \
                                                                                \
    ((CLASS(__NAME##_pool_t) *)ptPool)->ptFreeList = NULL;                      \
    ((CLASS(__NAME##_pool_t) *)ptPool)->tCounter   = 0;                         \
                                                                                \
    return true;                                                                \
}                                                                               \
                                                                                \
static void __##__NAME##_pool_free_item(                                        \
    __NAME##_pool_t *ptPool, __NAME##_pool_item_t *ptItem)                      \
{                                                                               \
    LIST_STACK_PUSH(                                                            \
        ((CLASS(__NAME##_pool_t) *)ptPool)->ptFreeList,                         \
        ptItem                                                                  \
    );                                                                          \
}                                                                               \
                                                                                \
void __NAME##_pool_free(                                                        \
    __NAME##_pool_t *ptPool, __TYPE *ptItem)                                    \
{                                                                               \
    if ((NULL == ptPool) || (NULL == ptItem)                                    \
        ||(NULL != ((CLASS(__NAME##_pool_item_t) *)ptItem)->ptNext)) {          \
        return ;                                                                \
    }                                                                           \
                                                                                \
    __ATOM_ACCESS(                                                              \
        __##__NAME##_pool_free_item(ptPool, (__NAME##_pool_item_t *)ptItem);    \
        if (((CLASS(__NAME##_pool_t) *)ptPool)->tCounter) {                     \
            ((CLASS(__NAME##_pool_t) *)ptPool)->tCounter--;                     \
        }                                                                       \
    )                                                                           \
}                                                                               \
                                                                                \
__TYPE *__NAME##_pool_new(__NAME##_pool_t *ptPool)                              \
{                                                                               \
    __single_list_node_t *ptItem = NULL;                                        \
                                                                                \
    if (NULL == ptPool) {                                                       \
        return NULL;                                                            \
    }                                                                           \
                                                                                \
    __ATOM_ACCESS(                                                              \
        do {                                                                    \
            if ((NULL == ((CLASS(__NAME##_pool_t) *)ptPool)->ptFreeList)) {     \
                break;                                                          \
            }                                                                   \
                                                                                \
            LIST_STACK_POP(                                                     \
                ((CLASS(__NAME##_pool_t) *)ptPool)->ptFreeList,                 \
                ptItem                                                          \
            );                                                                  \
        } while(false);                                                         \
    )                                                                           \
                                                                                \
    return &(((CLASS(__NAME##_pool_item_t) *)ptItem)->tObject);                 \
}                                                                               \
                                                                                \
bool __NAME##_pool_add_heap(                                                    \
    __NAME##_pool_t *ptPool, __NAME##_pool_item_t *ptBuffer, __PTR_TYPE tSize)  \
{                                                                               \
    if ((NULL == ptPool) || (NULL == ptBuffer) || (0 == tSize)) {               \
        return false;                                                           \
    }                                                                           \
                                                                                \
    __ATOM_ACCESS(                                                              \
        do {                                                                    \
            __##__NAME##_pool_free_item(ptPool, ptBuffer++);                    \
        } while(--tSize);                                                       \
    )                                                                           \
                                                                                \
    return true;                                                                \
}                                                                               \
__PTR_TYPE __NAME##_get_pool_item_count_allocated(__NAME##_pool_t *ptPool)      \
{                                                                               \
    __PTR_TYPE tResult = 0;                                                     \
    if (NULL == ptPool) {                                                       \
        return 0;                                                               \
    }                                                                           \
                                                                                \
    __ATOM_ACCESS(                                                              \
        tResult = ((CLASS(__NAME##_pool_t) *)ptPool)->tCounter;                 \
    )                                                                           \
                                                                                \
    return tResult;                                                             \
}          




/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif
