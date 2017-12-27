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
#ifndef __TASK_H__
#define __TASK_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if TASK_SCHEDULER == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \brief task function prototype
 *  \param pArg start address of a task argument control block
 *  \param pTask a pointer of task control block
 *  \retval true current task want to keep running.
 *  \retval false current task completed
 */
typedef fsm_rt_t safe_task_func_t( void *pArg, void * pTask);

DECLARE_CLASS(safe_call_stack_item_t)
//! \name call stack item
//! @{
DEF_CLASS(safe_call_stack_item_t)
    safe_task_func_t *fnRoutine;          //!< routine
    void  *pArg;                        //!< argument
END_DEF_CLASS(safe_call_stack_item_t);
//! @}

typedef volatile struct _task       safe_task_t;

#if SAFE_TASK_THREAD_SYNC == ENABLED
DECLARE_CLASS(fsm_flag_t)

//! \name task event item
//! @{
DEF_CLASS(fsm_flag_t)
    bool            bSignal;            //!< signal
    safe_task_t       *ptHead;            //!< task item 
    safe_task_t       *ptTail;
    bool            bManualReset;       //!< manual reset flag
#if 0
    locker_t        tLocker;            //!< thread locker
#endif
END_DEF_CLASS(fsm_flag_t)
//! @}

//! \name event
typedef fsm_flag_t  event_t;

#   if SAFE_TASK_CRITICAL_SECTION == ENABLED
//! \name critical section
typedef event_t critical_section_t;
#   endif

#endif

//! \name task control block
//! @{
struct _task
{
#if SAFE_TASK_CALL_STACK == ENABLED
    //! task call stack
    CLASS(safe_call_stack_item_t)     *pStack;             //!< return stack
    uint8_t                         chStackSize;        //!< stack size
    uint8_t                         chSP;               //!< stack pointer
#else
    safe_task_func_t                  *fnProcess;         //!< task routine
    void                            *pArg;              //!< task arguments
#endif

    
#if SAFE_TASK_THREAD_SYNC == ENABLED
    uint8_t                         bStateChanged : 1;  //!< state changed flag
    uint8_t                         bSignalRaised : 1;  //!< signal 
    uint8_t                         bThreadBlocked: 1;  //!< task blocked flag
    CLASS(fsm_flag_t)               *ptFlag;            //!< target semaphore
#else
    bool                            bStateChanged;      //!< state changed flag
#endif
    
    FLASH uint8_t *                 pchKey;             //!< key
    safe_task_t *                     pNext;
};
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

#if 0 
//! \brief task locker
extern NO_INIT locker_t s_TaskLocker;
#endif
extern NO_INIT safe_task_t *s_pFreeList;
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief add tasks to the task pool
 *  \param pHead task heap buffer
 *  \param hwSize heap size
 *  \return access result
 */
extern bool task_pool_init(void *pHeap, uint16_t hwSize);

/*! \brief verify task control block
 *  \param pchKey target key of the taget control block
 *  \retval true the control block is legal
 *  \retval false the control block is illegal
 */
extern bool check_task_key( uint8_t FLASH *pchKey );

#if SAFE_TASK_CALL_STACK == ENABLED
/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param pArg a pointer of a argument control block
 *  \param pStack a return stack buffer
 *  \param chStackSize return stack size
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
extern safe_task_t *new_task(
                safe_task_func_t *fnRoutine,
                void *pArg,
                volatile safe_call_stack_item_t *pStack,
                uint8_t chStackSize );
#else
/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param pArg a pointer of a argument control block
 *  \retval NULL failed to create a new task control block
 *  \retval a pointer for a initialized task control block
 */
extern safe_task_t *new_task( safe_task_func_t *fnRoutine, void *pArg );
#endif


/*! \brief free a task control blcok
 *  \param pTask a pointer of a task control block
 *  \return none
 */
extern void free_task( safe_task_t *pTask );


#if SAFE_TASK_CALL_STACK == ENABLED

/*! \brief try to push a task routine into stack
 *  \param pTask
 *  \param fnRoutine return routine
 *  \param pArg argument for return routine
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
extern bool task_push_ex(
            safe_task_t *pTask,
            safe_task_func_t *fnRoutine,
            void *pArg);

/*! \brief try to pop a task routine from stack
 *  \param pTask
 *  \param pArg a pointer of a argument control block
 *  \retval true succeeded in pushing task routine into common return stack.
 *  \retval false failed to push task routine into common return stack
 */
extern bool task_pop( safe_task_t *pTask );
#endif


#endif

#endif
/* EOF */
