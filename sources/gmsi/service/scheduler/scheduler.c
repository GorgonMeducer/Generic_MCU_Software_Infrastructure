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
#include ".\task.h"

#if TASK_SCHEDULER == ENABLED 

/*============================ MACROS ========================================*/

//! \brief task queue pool size
#ifndef SAFE_TASK_QUEUE_POOL_SIZE
#   define SAFE_TASK_QUEUE_POOL_SIZE            (1u)
#   warning No defined SAFE_TASK_QUEUE_POOL_SIZE, use default value 1u.
#endif

//! \brief task pool size
#ifndef SAFE_TASK_POOL_SIZE
#   define SAFE_TASK_POOL_SIZE                  (4u)
#   warning No defined SAFE_TASK_POOL_SIZE, use default value 4u.
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name task queue control block
//! @{
typedef volatile struct __safe_task_queue SAFE_TASK_QUEUE;
struct __safe_task_queue
{
    safe_task_t *     pHead;          //!< queue head
    safe_task_t *     pTail;          //!< queue tail
#if SAFE_TASK_QUEUE_POOL_SIZE > 1
    uint8_t         chBit;
    SAFE_TASK_QUEUE *ptNext;
#endif
};
//! @}

#if SAFE_TASK_QUEUE_POOL_SIZE > 1

#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#endif
//! \brief free task queue list
//! @{
NO_INIT struct {
    SAFE_TASK_QUEUE *ptQHead;
    SAFE_TASK_QUEUE *ptQTail;
} s_tFreeTaskQueueList;
//! @}
#endif



//! \name co-operation scheduler interface
//! @{
DEF_INTERFACE(scheduler_t)
    void (*Init)(void);
    //void (*Finish)(void);
    bool (*HeapInit)(void *, uint16_t);
#if SAFE_TASK_CALL_STACK == ENABLED
    void *(*NewTask)(safe_task_func_t *, void *,
            volatile safe_call_stack_item_t *, uint8_t );
#else
    void *(*NewTask)( safe_task_func_t *, void * );
#endif
    bool (*Scheduler)( void );
    bool (*Call)(  void *, safe_task_func_t * , void * , bool);
    bool (*CallEx)( void *, safe_task_func_t *, void *, safe_task_func_t *,
                        void *, bool );
#if SAFE_TASK_THREAD_SYNC == ENABLED
    event_t *(*CreateEvent)(event_t *, bool , bool );
    void (*SetEvent)(event_t *);
    void (*ResetEvent)(event_t *);
    bool (*WaitForSingleObject)(fsm_flag_t *, void *);
#   if SAFE_TASK_CRITICAL_SECTION == ENABLED
    void (*LeaveCriticalSection)(critical_section_t *);
#   endif
#endif
END_DEF_INTERFACE(scheduler_t)
//! @}
#endif

#if TASK_SCHEDULER == ENABLED
/*============================ PROTOTYPES ====================================*/
static bool _register_task( safe_task_t *pTask );
extern void scheduler_init( void );
extern void scheduler_finish( void );
extern bool task_pool_init(void *pHeap, uint16_t hwSize);

#if SAFE_TASK_CALL_STACK == ENABLED

extern void *register_task(
        safe_task_func_t *fnRoutine,
        void *pArg,
        volatile safe_call_stack_item_t *pStack,
        uint8_t chStackSize  );
#else
extern void *register_task( safe_task_func_t *fnRoutine, void *pArg );
#endif

extern bool scheduler( void );
extern bool call_task(  void *pT, safe_task_func_t *fnRoutine , 
                         void *pArg , bool bSubcall);
extern bool call_task_ex( void *pT,
                  safe_task_func_t *fnRoutine,
                  void *pArg,
                  safe_task_func_t *fnReturnRoutine,
                  void *pReturnArg,
                  bool bSubcall ) ;

#if SAFE_TASK_THREAD_SYNC == ENABLED

extern event_t *create_event(event_t *, bool bManualReset, bool bInitialState);
extern void set_event(event_t *);
extern void reset_event(event_t *);
extern bool wait_for_single_object(fsm_flag_t *, void *);

#   if SAFE_TASK_CRITICAL_SECTION == ENABLED
extern void leave_critical_section(critical_section_t *);
#   endif

#endif

/*============================ GLOBAL VARIABLES ==============================*/
#endif


#if TASK_SCHEDULER == ENABLED

#if defined(LIB_GENERATION)
ROOT
#endif
SECTION(".api_table")
//! \brief ES-scheduler
const scheduler_t SCHEDULER = 
    {
        &scheduler_init,            //!< initialize
        //&scheduler_finish,          //!< finish     
        &task_pool_init,            //!< add task heap
        &register_task,             //!< register task
        &scheduler,                 //!< scheduler
        &call_task,                 //!< call task
        &call_task_ex,              //!< call task extended version
    #if SAFE_TASK_THREAD_SYNC == ENABLED
        &create_event,              //!< create event
        &set_event,                 //!< set event
        &reset_event,               //!< reset event
        &wait_for_single_object,    //!< wait for single object
    #   if SAFE_TASK_CRITICAL_SECTION == ENABLED
        &leave_critical_section     //!< leave critical section
    #   endif
    #endif
    };

#endif

#if TASK_SCHEDULER == ENABLED 
/*============================ LOCAL VARIABLES ===============================*/

#if SAFE_TASK_POOL_SIZE > 0
//! \brief task pool
#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#else
#   if __IS_COMPILER_IAR__
#   pragma location = ".app_ram_noinit"
#   endif
#endif
NO_INIT static safe_task_t s_TaskPool[SAFE_TASK_POOL_SIZE];
#endif

#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#else
#   if __IS_COMPILER_IAR__
#   pragma location = ".app_ram_noinit"
#   endif
#endif
//! \brief task queue pool
NO_INIT static SAFE_TASK_QUEUE   s_TaskQueuePool[SAFE_TASK_QUEUE_POOL_SIZE];

//! \brief task locker
#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#endif
NO_INIT volatile locker_t s_SchedulerLocker;

#if SAFE_TASK_QUEUE_POOL_SIZE > 1
#if SAFE_TASK_USE_RESERVED_SYSTEM_RAM == ENABLED
#   if __IS_COMPILER_IAR__
#   pragma location = ".sys_ram"
#   endif
#endif
NO_INIT static uint8_t s_chIdleFlag;
#endif
/*============================ IMPLEMENTATION ================================*/

#if SAFE_TASK_QUEUE_POOL_SIZE > 1
#if __IS_COMPILER_IAR__
#   pragma optimize=no_size_constraints
#endif
static void free_task_queue(SAFE_TASK_QUEUE *ptQueue)
{
/*
    LOCK( s_SchedulerLocker, 
        ptQueue->ptNext = NULL;
        if (NULL == s_tFreeTaskQueueList.ptQTail) {
            s_tFreeTaskQueueList.ptQHead = ptQueue;
        } else {
            s_tFreeTaskQueueList.ptQTail->ptNext = ptQueue;
        }
        s_tFreeTaskQueueList.ptQTail = ptQueue;
    )
*/
    SAFE_ATOM_CODE(
        ptQueue->ptNext = NULL;
        if (NULL == s_tFreeTaskQueueList.ptQTail) {
            s_tFreeTaskQueueList.ptQHead = ptQueue;
        } else {
            s_tFreeTaskQueueList.ptQTail->ptNext = ptQueue;
        }
        s_tFreeTaskQueueList.ptQTail = ptQueue;
    )
}

#if __IS_COMPILER_IAR__
#   pragma optimize=no_size_constraints
#endif
static SAFE_TASK_QUEUE *get_free_task_queue(void)
{
    SAFE_TASK_QUEUE *ptQueue;
    /*
    LOCK( s_SchedulerLocker,
        ptQueue = s_tFreeTaskQueueList.ptQHead;
        if (NULL != ptQueue) {
            s_tFreeTaskQueueList.ptQHead = ptQueue->ptNext;
            if (NULL == s_tFreeTaskQueueList.ptQHead) {
                //! it is the last item.
                s_tFreeTaskQueueList.ptQTail = s_tFreeTaskQueueList.ptQHead;
            }
            ptQueue->ptNext = NULL;
        }
    )
    */
    SAFE_ATOM_CODE(
        ptQueue = s_tFreeTaskQueueList.ptQHead;
        if (NULL != ptQueue) {
            s_tFreeTaskQueueList.ptQHead = ptQueue->ptNext;
            if (NULL == s_tFreeTaskQueueList.ptQHead) {
                //! it is the last item.
                s_tFreeTaskQueueList.ptQTail = s_tFreeTaskQueueList.ptQHead;
            }
            ptQueue->ptNext = NULL;
        }
    )
    return ptQueue;
}
#endif


/*! \brief add a task control block to a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param pTask a pointer of a task control block
 *  \retval false failed to add task to queue
 *  \retval true succeeded in adding task to queue
 */
static bool add_task_to_queue( SAFE_TASK_QUEUE *pTaskQueue, safe_task_t *pTask )
{
#if 0
    if ((NULL == pTaskQueue) || (NULL == pTask)) {
        return false;
    }
#endif

    pTask->pNext = NULL;
    if (NULL == pTaskQueue->pTail) {
        pTaskQueue->pHead = pTask;          //!< update task head
        
    #if SAFE_TASK_QUEUE_POOL_SIZE > 1
        LOCK( s_SchedulerLocker,
            s_chIdleFlag |= _BV(pTaskQueue->chBit);
        )
    #endif
    } else {
        pTaskQueue->pTail->pNext = pTask;   //!< add a new task to tail
    }
    pTaskQueue->pTail = pTask;              //!< add a new task to queue

    return true;
}


/*! \brief get a task control block from a specified task control queue
 *  \param pTaskQueue a pointer of a task queue control block
 *  \param pTask a pointer of a task control block
 *  \retval NULL failed to get a task from queue
 *  \retval true succeeded in adding task to queue
 */
static safe_task_t *get_task_from_queue(SAFE_TASK_QUEUE *pTaskQueue)
{
    safe_task_t *pTask = pTaskQueue->pHead;
    if (NULL != pTask) {
        /* update queue */
        pTaskQueue->pHead = pTask->pNext;

        /* check whether queue is empty */
        if (NULL == pTaskQueue->pHead) {
            pTaskQueue->pTail = NULL;   //! update tail
        #if SAFE_TASK_QUEUE_POOL_SIZE > 1
            LOCK( s_SchedulerLocker,
                s_chIdleFlag &= ~_BV(pTaskQueue->chBit);
            )
        #endif
        }
    }
    return pTask;
}
 
#if __IS_COMPILER_IAR__
#   pragma optimize=no_size_constraints
#endif
/*! \brief scheduler function
 *  \retval true system is busy
 *  \retval false system is idle
 */
bool scheduler( void )
{
   
#if SAFE_TASK_QUEUE_POOL_SIZE > 1
    bool bResult = true;

    do {
    #if SAFE_TASK_CALL_STACK == ENABLED
        volatile CLASS(safe_call_stack_item_t) *ptRoutine;
    #endif
        SAFE_TASK_QUEUE *ptQueue = get_free_task_queue();
        if (NULL == ptQueue) {
            //! no free task queue
            break;
        }

        do {
            safe_task_t *pTask = get_task_from_queue(ptQueue);
            free_task_queue(ptQueue);
            if (NULL == pTask) {
                break;
            }
            pTask->bStateChanged = false;
        #if SAFE_TASK_CALL_STACK == ENABLED
            ptRoutine = pTask->pStack;
            /*if (NULL != ptRoutine->fnRoutine)*/ {
                /* run task routine */
                fsm_rt_t tState = ptRoutine->fnRoutine(ptRoutine->pArg, (void *)pTask);
        #else
            /*if (NULL != pTask->fnProcess)*/ {
                /* run task routine */
                fsm_rt_t tState = pTask->fnProcess(pTask->pArg, (void *)pTask);
        #endif
                if (fsm_rt_cpl == tState) {
                    if (pTask->bStateChanged) {
                        while(!_register_task(pTask));  //!< re-add this task to queue
                        break;
                    #if SAFE_TASK_CALL_STACK == ENABLED
                    } else if (task_pop(pTask)) {             //!< return
                        /* re-add this task to queue */
                        while(!_register_task(pTask));
                        break;
                    #endif
                    }             
                #if SAFE_TASK_THREAD_SYNC == ENABLED
                } else if (fsm_rt_wait_for_obj == tState) {
                    /* wait for semaphore object, task is blocked */
                    if (NULL == pTask->ptFlag) {
                        pTask->bSignalRaised = false;
                        //! wait...this should not be happen!
                        while(!_register_task(pTask));  //!< re-add this task to queue
                        break;
                    }
                    /*
                    LOCK (
                        pTask->ptFlag->tLocker,                    
                        if (pTask->ptFlag->bSignal) {
                            //! signal already set
                            pTask->bThreadBlocked = false;
                            while(!_register_task(pTask));  //!< re-add this task to queue
                            pTask->bSignalRaised = true;    //!< set task flag
                            pTask->ptFlag = NULL;
                            
                        } else {
                            //! event is not raised
                            pTask->bThreadBlocked = true;
                        }
                    )
                    */
                    SAFE_ATOM_CODE(
                        if (pTask->ptFlag->bSignal) {
                            //! signal already set
                            pTask->bThreadBlocked = false;
                            while(!_register_task(pTask));  //!< re-add this task to queue
                            pTask->bSignalRaised = true;    //!< set task flag
                            pTask->ptFlag = NULL;
                            
                        } else {
                            //! event is not raised
                            pTask->bThreadBlocked = true;
                        }
                    )
                    
                    break;
                #endif
                } else /*if (fsm_rt_on_going == tState)*/ {
                    while(!_register_task(pTask));  //!< re-add this task to queue
                    break;
                }
            }

            free_task(pTask);                       //!< free task
        } while(false);

    } while(false);

    LOCK(s_SchedulerLocker,
        bResult = s_chIdleFlag ? true : false;
    )

    return bResult;
#else

    safe_task_t *pTask;
    /*LOCK(
        s_SchedulerLocker,
        pTask = get_task_from_queue(s_TaskQueuePool);
        if (NULL == pTask) {
            EXIT_LOCK();
            return false;
        }       
    )*/
    SAFE_ATOM_CODE(
        pTask = get_task_from_queue(s_TaskQueuePool);
    )
    if (NULL == pTask) {
        return false;
    }
    

    do {
    #if SAFE_TASK_CALL_STACK == ENABLED
        volatile CLASS(safe_call_stack_item_t) *ptRoutine;
    #endif
        pTask->bStateChanged = false;
    #if SAFE_TASK_CALL_STACK == ENABLED
        ptRoutine = pTask->pStack;
        /*if (NULL != ptRoutine->fnRoutine)*/ {
            /* run task routine */
            fsm_rt_t tState = ptRoutine->fnRoutine(ptRoutine->pArg, (void *)pTask);
    #else
        /*if (NULL != pTask->fnProcess)*/ {
            /* run task routine */
            fsm_rt_t tState = pTask->fnProcess(pTask->pArg, (void *)pTask);
    #endif
            if (fsm_rt_cpl == tState) {
                if (pTask->bStateChanged) {
                    while(!_register_task(pTask));  //!< re-add this task to queue
                    break;
                #if SAFE_TASK_CALL_STACK == ENABLED
                } else if (task_pop(pTask)) {             //!< return
                    /* re-add this task to queue */
                    while(!_register_task(pTask));
                    break;
                #endif
                }             
            #if SAFE_TASK_THREAD_SYNC == ENABLED
            } else if (fsm_rt_wait_for_obj == tState) {
                /* wait for semaphore object, task is blocked */
                if (NULL == pTask->ptFlag) {
                    pTask->bSignalRaised = false;
                    //! wait...this should not happen!
                    while(!_register_task(pTask));  //!< re-add this task to queue
                    break;
                }
                /*
                LOCK (
                    pTask->ptFlag->tLocker,                    
                    if (pTask->ptFlag->bSignal) {
                        //! signal already set
                        pTask->bThreadBlocked = false;
                        while(!_register_task(pTask));  //!< re-add this task to queue
                        pTask->bSignalRaised = true;    //!< set task flag
                        pTask->ptFlag = NULL;
                        
                    } else {
                        //! event is not raised
                        pTask->bThreadBlocked = true;
                    }
                )
                */
                SAFE_ATOM_CODE(
                    if (pTask->ptFlag->bSignal) {
                        //! signal already set
                        pTask->bThreadBlocked = false;
                        while(!_register_task(pTask));  //!< re-add this task to queue
                        pTask->bSignalRaised = true;    //!< set task flag
                        pTask->ptFlag = NULL;
                        
                    } else {
                        //! event is not raised
                        pTask->bThreadBlocked = true;
                    }
                )
                break;
            #endif
            } else /*if (fsm_rt_on_going == tState)*/ {
                while(!_register_task(pTask));  //!< re-add this task to queue
                break;
            }
        }

        free_task(pTask);                           //!< free task
    } while(false);

    return true;

#endif
}

#if SAFE_TASK_THREAD_SYNC == ENABLED
/*! \brief initialize task event
 *! \param ptEvent event object
 *! \param bManualReset flag that indicates whether the event should reset to 
 *!        inactived state automatically.
 *! \param bInitialState event initial state, either set or not.
 *! \return pointer for event object
 */
event_t *create_event(event_t *pEvent, bool bManualReset, bool bInitialState)
{
    CLASS(fsm_flag_t) *ptEvent = (CLASS(fsm_flag_t) *)pEvent; 
    do {
        if (NULL == ptEvent) {
            break;
        }
        
    #if 0
        LOCK_INIT(ptEvent->tLocker);            //!< initialize thread locker
    #endif 
        ptEvent->bSignal = bInitialState;       //!< set initial state
        ptEvent->bManualReset = bManualReset;   //!< manual reset flag
        ptEvent->ptHead = NULL;                
        ptEvent->ptTail = NULL;
    } while(0);

    return (event_t *)ptEvent;
}

/*! \brief set task event
 *! \param ptEvent pointer for task event
 *! \return none
 */
void set_event(event_t *pEvent) 
{
    CLASS(fsm_flag_t) *ptEvent = (CLASS(fsm_flag_t) *)pEvent; 
    if (NULL == ptEvent) {
        return ;
    }
#if 0
    LOCK( ptEvent->tLocker,
        //! wake up blocked tasks
        safe_task_t *pTask = ptEvent->ptHead;
        while(NULL != pTask) {
            if (pTask->bThreadBlocked) {
                pTask->bThreadBlocked = false;
                while (!_register_task(pTask)); //!< register task
            }
            
            pTask->ptFlag = NULL;
            pTask->bSignalRaised = true;        //!< set task flag

            pTask = pTask->pNext;            
        }

        ptEvent->ptTail = NULL;
        ptEvent->ptHead = NULL;                  //!< clear tasks

        ptEvent->bSignal = true;
//        if (ptEvent->bManualReset) {
//            ptEvent->bSignal = true;            //!< set flag
//        } else {
//            ptEvent->bSignal = false;           //!< set flag
//        }

    )
#else
    SAFE_ATOM_CODE(
        //! wake up blocked tasks
        safe_task_t *pTask = ptEvent->ptHead;
        while(NULL != pTask) {
            if (pTask->bThreadBlocked) {
                pTask->bThreadBlocked = false;
                while (!_register_task(pTask)); //!< register task
            }
            
            pTask->ptFlag = NULL;
            pTask->bSignalRaised = true;        //!< set task flag

            pTask = pTask->pNext;            
        }

        ptEvent->ptTail = NULL;
        ptEvent->ptHead = NULL;                  //!< clear tasks

        ptEvent->bSignal = true;
//        if (ptEvent->bManualReset) {
//            ptEvent->bSignal = true;            //!< set flag
//        } else {
//            ptEvent->bSignal = false;           //!< set flag
//        }
    )
#endif

    
}

#   if SAFE_TASK_CRITICAL_SECTION == ENABLED
/*! \brief try to enter critical section
 *! \param critical section item
 *! \return none
 */
void leave_critical_section(critical_section_t *ptCritical)
{
    CLASS(fsm_flag_t) *ptEvent = (CLASS(fsm_flag_t) *)ptCritical; 
    if (NULL == ptEvent) {
        return ;
    }
#if 0
    LOCK( ptEvent->tLocker,
        if (!ptEvent->bSignal) {
            //! wake up blocked tasks
            safe_task_t *ptTask = ptEvent->ptHead;
            if (NULL == ptTask) {
                ptEvent->bSignal = true;
            } else {
                ptEvent->bSignal = false;                           //!< set flag

                //! remove task from queue list
                ptEvent->ptHead = ptTask->pNext;
                if (NULL == ptEvent->ptHead) {
                    ptEvent->ptTail = NULL;
                }
                ptTask->pNext = NULL;

                //! release critical section for the target task
                if (ptTask->bThreadBlocked) {
                    ptTask->bThreadBlocked = false;
                    while (!_register_task(ptTask));     //!< register task
                }
                ptTask->ptFlag = NULL;
                ptTask->bSignalRaised = true;            //!< set task flag
            }
        }
    )
#else
    SAFE_ATOM_CODE(
        if (!ptEvent->bSignal) {
            //! wake up blocked tasks
            safe_task_t *ptTask = ptEvent->ptHead;
            if (NULL == ptTask) {
                ptEvent->bSignal = true;
            } else {
                ptEvent->bSignal = false;                           //!< set flag

                //! remove task from queue list
                ptEvent->ptHead = ptTask->pNext;
                if (NULL == ptEvent->ptHead) {
                    ptEvent->ptTail = NULL;
                }
                ptTask->pNext = NULL;

                //! release critical section for the target task
                if (ptTask->bThreadBlocked) {
                    ptTask->bThreadBlocked = false;
                    while (!_register_task(ptTask));     //!< register task
                }
                ptTask->ptFlag = NULL;
                ptTask->bSignalRaised = true;            //!< set task flag
            }
        }
    )
#endif
}

#endif

/*! \brief reset specified task event
 *! \param ptEvent task event pointer
 *! \return none
 */
void reset_event(event_t *pEvent)
{
    CLASS(fsm_flag_t) *ptEvent = (CLASS(fsm_flag_t) *)pEvent; 
    if (NULL == ptEvent) {
        return ;
    }
#if 0
    LOCK(ptEvent->tLocker,
        ptEvent->bSignal = false;
    )
#else
    SAFE_ATOM_CODE(
        ptEvent->bSignal = false;
    )
#endif
}

/*! \brief wait for a specified task event
 *! \param ptEvent target event item
 *! \param pTask parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
bool wait_for_single_object(fsm_flag_t *ptFlag, void *ptTask)
{
    bool bResult = true;
    safe_task_t *pTask = (safe_task_t *)ptTask;
    CLASS(fsm_flag_t) *ptEvent = (CLASS(fsm_flag_t) *)ptFlag; 
    if (NULL == ptEvent) {
        return bResult;                         //!< wait nothing
    }
    
#if 0
    LOCK(ptEvent->tLocker,
        bResult = ptEvent->bSignal;
        if (!ptEvent->bManualReset) {
            ptEvent->bSignal = false;
        }
        if (NULL != pTask) {
            if (bResult) {
                pTask->bSignalRaised = false;
            } else if (pTask->bSignalRaised) {
                pTask->bSignalRaised = false;
                bResult = true;
            } else {
                //! add task to the wait list
                pTask->pNext = NULL;
                if (NULL == ptEvent->ptTail) {
                    ptEvent->ptHead = pTask;
                } else {
                    ptEvent->ptTail->pNext = pTask;
                }
                ptEvent->ptTail = pTask;

                pTask->ptFlag = ptEvent;  
                pTask->bThreadBlocked = false;
                bResult = false;
            }
        }
    )
#else
    SAFE_ATOM_CODE(
        bResult = ptEvent->bSignal;
        if (!ptEvent->bManualReset) {
            ptEvent->bSignal = false;
        }
        if (NULL != pTask) {
            if (bResult) {
                pTask->bSignalRaised = false;
            } else if (pTask->bSignalRaised) {
                pTask->bSignalRaised = false;
                bResult = true;
            } else {
                //! add task to the wait list
                pTask->pNext = NULL;
                if (NULL == ptEvent->ptTail) {
                    ptEvent->ptHead = pTask;
                } else {
                    ptEvent->ptTail->pNext = pTask;
                }
                ptEvent->ptTail = pTask;

                pTask->ptFlag = ptEvent;  
                pTask->bThreadBlocked = false;
                bResult = false;
            }
        }
    )
#endif

    return bResult;
}

/*
event_t tEvent;

STATE(Demo) BEGIN
    if (!wait_for_single_object(&tEvent, THIS_TASK)) {
        WAIT_FOR_OBJ;
    }

    ...
END
 */

#endif

//#pragma optimize=speed
/*! \brief add a task control block to task queue
 *  \param pTask task control block
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
static bool _register_task( safe_task_t *pTask )
{
    bool bResult = true;
#if 0
    if (NULL == pTask)
    {
        return false;
    }
#endif

#if SAFE_TASK_QUEUE_POOL_SIZE > 1
    SAFE_TASK_QUEUE *ptTaskQueue = get_free_task_queue();
    if (NULL != ptTaskQueue) {
        bResult = add_task_to_queue(ptTaskQueue,pTask);
        free_task_queue(ptTaskQueue);
    }
#else
    /*LOCK(
        s_SchedulerLocker,
        if (!add_task_to_queue(&s_TaskQueuePool[0],pTask)) {
            bResult = false;   
        } 
    )*/
    SAFE_ATOM_CODE(
        if (!add_task_to_queue(&s_TaskQueuePool[0],pTask)) {
            bResult = false;   
        }
    )
#endif
    return bResult;
}

#if __IS_COMPILER_IAR__
#   pragma optimize=no_size_constraints
#endif
/*! \brief call a sub task (routine)
 *  \param pT a pointer of task control block
 *  \param fnRoutine target routine
 *  \param pArg a pointer of argument control block
 *  \param fnReturnRoutine return to this routine when FSM completed
 *  \param pReturnArg argument for return routine
 *  \param bSubcall is sub-task to be called
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
bool call_task_ex( void *pT,
                  safe_task_func_t *fnRoutine,
                  void *pArg,
                  safe_task_func_t *fnReturnRoutine,
                  void *pReturnArg,
                  bool bSubcall )
{
    safe_task_t *pTask = (safe_task_t *)pT;
    if (    (NULL == pTask)
        ||  (!check_task_key(pTask->pchKey))
        ||  (NULL == fnRoutine)
        ||  (NULL == fnReturnRoutine)) {
        return false;
    }

#if SAFE_TASK_CALL_STACK == ENABLED
    if ( bSubcall ) {
        //! push task
        if (!task_push_ex(pTask,fnReturnRoutine, pReturnArg)) {
            return false;
        }
    }

    do {
        volatile CLASS(safe_call_stack_item_t) *ptRoutine = pTask->pStack;
        ptRoutine->fnRoutine = fnRoutine;
        ptRoutine->pArg = pArg;
    } while (false);

#else
    pTask->fnProcess = fnRoutine;
    pTask->pArg = pArg;
#endif

    pTask->bStateChanged = true;

    //! issue found here, in rom, it is return false
    return true;
}

#if __IS_COMPILER_IAR__
#   pragma optimize=no_size_constraints
#endif
/*! \brief call a sub task (routine)
 *  \param pT a pointer of task control block
 *  \param fnRoutine target routine
 *  \param pArg a pointer of argument control block
 *  \param bSubcall is sub-task to be called
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
bool call_task( void *pT, safe_task_func_t *fnRoutine ,  void *pArg , bool bSubcall)
{
    safe_task_t *pTask = (safe_task_t *)pT;
    if (NULL == pTask) {
        return false;
    }
#if SAFE_TASK_CALL_STACK == ENABLED
    do {
        volatile CLASS(safe_call_stack_item_t) *ptRoutine = pTask->pStack;
        return call_task_ex(
            pT, fnRoutine, pArg, ptRoutine->fnRoutine, ptRoutine->pArg, bSubcall);
    } while (false);
#else
    return call_task_ex(
            pT, fnRoutine, pArg, pTask->fnProcess, pTask->pArg, bSubcall);
#endif
}

#if SAFE_TASK_CALL_STACK == ENABLED

/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param fnReturnTo a routine which will be called when task routine return
 *         false.
 *  \param pArg a pointer of a argument control block
 *  \param pStack a return stack buffer
 *  \param chStackSize return stack size
 *  \return task handle
 */
void *register_task(
        safe_task_func_t *fnRoutine,
        void *pArg,
        volatile safe_call_stack_item_t *pStack,
        uint8_t chStackSize  )
#else

/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param fnReturnTo a routine which will be called when task routine return
 *         false.
 *  \param pArg a pointer of a argument control block
 *  \return task handle
 */
void *register_task( safe_task_func_t *fnRoutine, void *pArg )
#endif
{
#if SAFE_TASK_CALL_STACK == ENABLED
    /*! try to create a new task control block */
    safe_task_t *pTask = new_task( fnRoutine, pArg, pStack, chStackSize );
#else
    /*! try to create a new task control block */
    safe_task_t *pTask = new_task( fnRoutine, (void *)pArg );
#endif
    if (NULL != pTask) {
        //! register a task
        if (_register_task(pTask)) {
            return (void *)pTask;
        }
    }

    //! free task
    free_task(pTask);
    return NULL;
}

/*! \brief finish scheduler
 *  \param none
 *  \return none
 */
void scheduler_finish( void )
{
    //! initialize ciritical locker
#if 0
    LOCK_INIT(s_TaskLocker);
#endif
    LOCK_INIT(s_SchedulerLocker);
#if SAFE_TASK_QUEUE_POOL_SIZE > 1
    s_chIdleFlag = _BV(SAFE_TASK_QUEUE_POOL_SIZE)-1;
    OBJECT_INIT_ZERO(s_tFreeTaskQueueList);
#endif
    OBJECT_INIT_ZERO(s_TaskQueuePool);


    s_pFreeList = NULL;
}

/*! \brief initialize scheduler
 *  \param none
 *  \return none
 */
void scheduler_init( void )
{
    scheduler_finish();
#if SAFE_TASK_POOL_SIZE > 0
    task_pool_init((void *)s_TaskPool,sizeof(s_TaskPool));
#endif

#if SAFE_TASK_QUEUE_POOL_SIZE > 1
    //! add task queue to free list
    do {
        uint_fast8_t n = UBOUND(s_TaskQueuePool);
        SAFE_TASK_QUEUE *ptQueue = s_TaskQueuePool;
        
        do {
            ptQueue->chBit = n-1;
            free_task_queue(ptQueue++);
        } while(--n);

    } while (false);
#endif
}

#endif

/* EOF */
