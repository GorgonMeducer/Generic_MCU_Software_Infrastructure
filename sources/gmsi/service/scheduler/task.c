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
#include "app_cfg.h"
#include ".\task.h"

#if TASK_SCHEDULER == ENABLED

/*============================ MACROS ========================================*/
//const static uint32_t c_wTaskSize = sizeof(safe_task_t);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if 0
//! \brief task locker
#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#endif
NO_INIT locker_t s_TaskLocker;
#endif

#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#endif
NO_INIT safe_task_t *s_pFreeList;

/*============================ LOCAL VARIABLES ===============================*/


//! \brief task key for identifying task control blocks
static FLASH uint8_t s_chTaskKey[] = "Schedule";

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if SAFE_TASK_CALL_STACK == ENABLED

/*! \brief try to push a task routine into stack
 *  \param pTask
 *  \param fnRoutine return routine
 *  \param pArg argument for return routine
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
bool task_push_ex(
            safe_task_t *pTask,
            safe_task_func_t *fnRoutine,
            void *pArg) {

    if (    (NULL == pTask)
        ||  (NULL == pTask->pStack)
        ||  (s_chTaskKey != pTask->pchKey)
        ||  (pTask->chSP >= pTask->chStackSize)
        ||  (NULL == fnRoutine)) {
        return false;
    }
    do {
        volatile CLASS(safe_call_stack_item_t) *ptRoutine = pTask->pStack;
        ptRoutine->fnRoutine = fnRoutine;
        ptRoutine->pArg = pArg;
    } while (false);
    pTask->pStack++;
    pTask->chSP++;

    return true;
}

/*! \brief try to pop a task routine from stack
 *  \param pTask
 *  \param pArg a pointer of a argument control block
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
bool task_pop( safe_task_t *pTask )
{
    /*! check input */
    if (    (NULL == pTask)
        ||  (NULL == pTask->pStack)
        ||  (s_chTaskKey != pTask->pchKey)
        ||  (pTask->chSP > pTask->chStackSize)) {
        return false;
    }

    //! is stack empty
    if (0 == pTask->chSP) {
        /* nothing to pop */
        return false;
    }

    pTask->pStack--;
    pTask->chSP--;

    return true;
}
#endif

/*! \brief verify task control block
 *  \param pchKey target key of the taget control block
 *  \retval true the control block is legal
 *  \retval false the control block is illegal
 */
bool check_task_key( uint8_t FLASH *pchKey )
{
    return (pchKey == s_chTaskKey);
}


#if SAFE_TASK_CALL_STACK == ENABLED
/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param pArg a pointer of a argument control block
 *  \param pStack a return stack buffer
 *  \param chStackSize return stack size
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
safe_task_t *new_task(
                safe_task_func_t *fnRoutine,
                void *pArg,
                volatile safe_call_stack_item_t *pStack,
                uint8_t chStackSize )
#else
/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param pArg a pointer of a argument control block
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
safe_task_t *new_task( safe_task_func_t *fnRoutine, void *pArg )
#endif
{
    safe_task_t *pTask = NULL;

#if SAFE_TASK_CALL_STACK == ENABLED
    if (( NULL == pStack ) || ( 0 == chStackSize )) {    //!< check stack
        return NULL;
    }
#endif

    if (NULL == fnRoutine) {                            //!< check task routine
        return NULL;
    }

    
#if SAFE_TASK_CALL_STACK == ENABLED
#if 0
    LOCK(
        s_TaskLocker,
        if (NULL != s_pFreeList) {
            pTask = s_pFreeList;
            s_pFreeList = s_pFreeList->pNext;
        }
    )
#else
    SAFE_ATOM_CODE(
        if (NULL != s_pFreeList) {
            pTask = s_pFreeList;
            s_pFreeList = s_pFreeList->pNext;
        }
    )
#endif
    if (NULL != pTask) {
        /* find a task control block */
        pTask->pchKey = s_chTaskKey;     //!< set task key 

        //!< set stack buffer
        pTask->pStack = (CLASS(safe_call_stack_item_t) *)pStack;          
        //!< set task routine    
        pTask->pStack->fnRoutine = fnRoutine; 
        //!< set argument
        pTask->pStack->pArg = (void *)pArg;   
        
        pTask->chStackSize = chStackSize;//!< set stack size
        pTask->chSP = 0;
        pTask->bStateChanged = false;
    #if SAFE_TASK_THREAD_SYNC == ENABLED
        pTask->ptFlag = NULL;
        pTask->bSignalRaised = false;
        pTask->bThreadBlocked = false;
    #endif
        pTask->pNext = NULL;
    }
#else
#if 0
    LOCK(
        s_TaskLocker,
        if (NULL != s_pFreeList) {
            pTask = s_pFreeList;
            s_pFreeList = s_pFreeList->pNext;
        }
    )
#else
    SAFE_ATOM_CODE(
        if (NULL != s_pFreeList) {
            pTask = s_pFreeList;
            s_pFreeList = s_pFreeList->pNext;
        }
    )
#endif
    
    if (NULL != pTask) {
        /* find a task control block */
        pTask->pchKey = s_chTaskKey;     //!< set task key
        pTask->fnProcess = fnRoutine;    //!< set task routine
        pTask->pArg = pArg;              //!< set argument
        pTask->bStateChanged = false;
    #if SAFE_TASK_THREAD_SYNC == ENABLED
        pTask->ptFlag = NULL;
        pTask->bSignalRaised = false;
        pTask->bThreadBlocked = false;
    #endif
        pTask->pNext = NULL;
    }
#endif

    return pTask;
}


/*! \brief free a task control blcok
 *  \param pTask a pointer of a task control block
 *  \return none
 */
void free_task( safe_task_t *pTask )
{
    if (NULL == pTask) {
        return ;
    } else if (pTask->pchKey != s_chTaskKey) {    //!< check the key
        return ;
    }

#if 0
    LOCK(
        s_TaskLocker,
        pTask->pchKey = NULL;                   //!< clear task key

        //! add task item to freelist
        pTask->pNext = s_pFreeList;      
        s_pFreeList = pTask;
    )
#else
    SAFE_ATOM_CODE(
        pTask->pchKey = NULL;                   //!< clear task key

        //! add task item to freelist
        pTask->pNext = s_pFreeList;      
        s_pFreeList = pTask;
    )
#endif
}


/*! \brief add tasks to the task pool
 *  \param pHead task heap buffer
 *  \param hwSize heap size
 *  \return access result
 */
bool task_pool_init(void *pHeap, uint16_t hwSize)
{
    uint_fast8_t n = 0;
    safe_task_t *pTask = (safe_task_t *)pHeap;
    if (NULL == pHeap || hwSize < sizeof(safe_task_t)) {
        return false;
    }
    
    //! add tasks to the free list
    n = hwSize / sizeof(safe_task_t);
    do {
#if 0
        LOCK(
            s_TaskLocker,
            pTask->pchKey = NULL;                   //!< clear task key

            //! add task item to freelist
            pTask->pNext = s_pFreeList;      
            s_pFreeList = pTask;
        )
#else
        SAFE_ATOM_CODE(
            pTask->pchKey = NULL;                   //!< clear task key

            //! add task item to freelist
            pTask->pNext = s_pFreeList;      
            s_pFreeList = pTask;
        )
#endif
        pTask++;
    } while(--n);

    return true;
}

#endif

/* EOF */
