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

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if TASK_SCHEDULER == ENABLED

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/



#if SAFE_TASK_CALL_STACK == ENABLED

    #define NEW_FSM_WITH_ARG(__NAME, __ROUTINE, __ARG)   \
                (NULL != SCHEDULER.NewTask \
                    (\
                        __ROUTINE,\
                        &(__ARG),\
                        g_##__NAME##CallStack,\
                        UBOUND(g_##__NAME##CallStack)\
                    ))

    #define NEW_FSM(__NAME, __ROUTINE)   \
                (NULL != SCHEDULER.NewTask \
                    (\
                        __ROUTINE,\
                        NULL,\
                        g_##__NAME##CallStack,\
                        UBOUND(g_##__NAME##CallStack)\
                    ))

    #define FSM(__NAME, __STACK_SIZE)  \
                NO_INIT volatile safe_call_stack_item_t g_##__NAME##CallStack[(__STACK_SIZE)+1];\

    #define STATIC_FSM(__NAME)                  FSM(__NAME, 0)

    #define NEW_STATIC_FSM(__NAME,__ROUTINE)    NEW_FSM(__NAME, (__ROUTINE))

#else


    #define NEW_FSM_WITH_ARG(__NAME, __ROUTINE, __ARG_TYPE)   \
                (NULL != SCHEDULER.NewTask \
                    (\
                        __ROUTINE,\
                        &g_##__NAME##Arg\
                    ))

    #define NEW_FSM(__NAME, __ROUTINE)   \
                (NULL != SCHEDULER.NewTask \
                    (\
                        __ROUTINE,\
                        NULL\
                    ))
    
    #define NEW_STATIC_FSM(__NAME,__ROUTINE)    \
                (NULL != SCHEDULER.NewTask \
                    (\
                        __ROUTINE,\
                        NULL\
                    ))

    #define STATIC_FSM(__NAME)
    #define FSM(__NAME, __STACK_SIZE)

#endif

#define EXTERN_STATIC_FSM(__NAME)
#define EXTERN_FSM(__NAME)
#define END_EXTERN_STATIC_FSM
#define END_EXTERN_FSM

#define DEF_ARG                 DEF_CLASS
#define END_DEF_ARG(__NAME)     END_DEF_CLASS(__NAME)

#define EXTERN_ARG              EXTERN_CLASS
#define END_EXTERN_ARG(__NAME)  END_EXTERN_CLASS(__NAME)

#define ARG(__NAME)        CLASS(__NAME)

#define REF_ARG(__NAME)     \
                (*((CLASS(__NAME) *)(pArg)))

#define CHECK_ARG(__NAME)   \
                ((NULL == pArg) ? false : true)

//! \name macros for finit state machine
//! @{

#define END_FSM
#define END_STATIC_FSM

//! \brief start define a FSM state
#define STATE(__NAME) \
            fsm_rt_t FSM_##__NAME(void * pArg, void * pTask)

//! \brief begin & end
#ifndef BEGIN
#define BEGIN           {
#endif
#ifndef END
#define END             }
#endif

#if SAFE_TASK_THREAD_SYNC == ENABLED

#   define WAIT_EVENT_BEGIN(__EVENT)            {                   \
                if (!SCHEDULER.WaitForSingleObject((__EVENT), pTask)) {   \
                    WAIT_FOR_OBJ;                                   \
                }

#   define CS_WAIT_EVENT(__EVENT)      SCHEDULER.WaitForSingleObject((__EVENT), pTask)
                

//! \brief initialize a task event item
#   define CS_INIT_EVENT(__EVENT,__MANUAL_RESET,__INITIAL_STATE)       \
                SCHEDULER.CreateEvent((__EVENT),(__MANUAL_RESET), (__INITIAL_STATE))

//! \brief set task event to active state
#   define CS_SET_EVENT(__EVENT)                   SCHEDULER.SetEvent((__EVENT))

//! \brief reset task event to inactive state
#   define CS_RESET_EVENT(__EVENT)                 SCHEDULER.ResetEvent((__EVENT))
//! @}

#   if SAFE_TASK_CRITICAL_SECTION == ENABLED
//! \name critical section
//! @{
//! \brief initialize a specified critical section
#       define CS_INIT_CRITICAL_SECTION(__CRITICAL)                    \
            SCHEDULER.CreateEvent((event_t *)(__CRITICAL), false, true)

/*! \brief state modifier which inidicates a specified state to be critical 
 *!        section. E.g.
 *!         STATE(DEMO_A) CRITICAL_SECTION_BEGIN(&g_tCritical)
 *!             ...
 *!             TRANSFER_TO_STATE(DEMO_B)
 *!             EXIT_STATE;
 *!         END
 */
#       define CRITICAL_SECTION_BEGIN(__CRITICAL)    {              \
                if (!SCHEDULER.WaitForSingleObject((event_t *)(__CRITICAL), pTask)) {\
                    WAIT_FOR_OBJ;                                   \
                }

#       define CS_ENTER_CRITICAL_SECTION(__CRITICAL)                   \
                SCHEDULER.WaitForSingleObject((event_t *)(__CRITICAL), pTask)

#       define CS_LEAVE_CRITICAL_SECTION(__CRITICAL)                   \
                SCHEDULER.LeaveCriticalSection((__CRITICAL))

#       define enter_critical_section(__CRITICAL, __TASK)           \
                SCHEDULER.WaitForSingleObject((__CRITICAL),(__TASK))

#       define create_critical_section(__CRITICAL)                  \
                SCHEDULER.CreateEvent((event_t *)(__CRITICAL), false, true))
//! @}

//! \name mutex
//! @{
#       define CS_INIT_MUTEX(__MUTEX)                                  \
                INIT_CRITICAL_SECTION(__MUTEX)
#       define CS_RELEASE_MUTEX(__MUTEX)                               \
                LEAVE_CRITICAL_SECTION(__MUTEX)
#       define CS_WAIT_MUTEX(__MUTEX)                                  \
                ENTER_CRITICAL_SECTION(__MUTEX)
#       define release_mutex(__MUTEX)                               \
                LEAVE_CRITICAL_SECTION(__MUTEX)
#       define create_mutex(__MUTEX)                                \
                INIT_CRITICAL_SECTION(__MUTEX)
//! @}

#   endif

#endif


#define REGION

#define STATIC_ALLOC    static
#ifndef PUBLIC
#define PUBLIC
#endif
#ifndef public
#define public
#endif


#ifndef PRIVATE
#define PRIVATE         static
#endif

#ifndef private
#define private         static
#endif

#define REFLEXIVE_STATE         return fsm_rt_on_going;
#define EXIT_STATE              return fsm_rt_cpl;
#define WAIT_FOR_OBJ            return fsm_rt_wait_for_obj;

//! \brief state transfering
#   define TRANSFER_TO_STATE(__ROUTINE)    \
            SCHEDULER.Call(pTask, FSM_##__ROUTINE, pArg, false)

//! \brief call sub state machine
#   define CALL_FSM(__ROUTINE, __ARG_ADDR)     \
            SCHEDULER.Call(pTask, __ROUTINE, __ARG_ADDR, true)

//! \brief call sub state machine
#   define CALL_FSM_EX(__ROUTINE, __ARG_ADDR, __RET_ROUTINE,__RET_ARG_ADDR)    \
            SCHEDULER.CallEx(pTask, __ROUTINE, __ARG_ADDR, \
                        __RET_ROUTINE, __RET_ARG_ADDR, true)

#   define CS_SCHEDULER()               SCHEDULER.Scheduler()
#   define CS_INIT()                    SCHEDULER.Init()
#   define CS_HEAP_INIT(__ADDR, __SIZE) SCHEDULER.HeapInit(__ADDR, __SIZE)
//#   define CS_FINISH()                  SCHEDULER.Finish()


//! \brief a reference to FSM state
#define REF_STATE(__NAME)   FSM_##__NAME

#define THIS_TASK   pTask

#define EXTERN_STATE(__NAME)    \
    extern fsm_rt_t FSM_##__NAME(void * pArg, void *pTask)
//! @}

#define DECLARE
#define END_DECLARE

#define IMPLEMENT_FSM(__NAME)
#define END_IMPLEMENT_FSM

/*============================ TYPES =========================================*/

#ifdef RESERVED
#undef RESERVED
#endif
#define __RSV(__N)              _##__N
#define ___RSV(__N)             __RSV(__N)
#define RESERVED                ___RSV(__LINE__)

DECLARE_CLASS(task_t)
    
EXTERN_CLASS(task_t)
#if SAFE_TASK_CALL_STACK == ENABLED
    void                            *RESERVED;             
    uint8_t                         RESERVED;        
    uint8_t                         RESERVED;               
#else
    void                            *RESERVED;         
    void                            *RESERVED;              
#endif

    
#if SAFE_TASK_THREAD_SYNC == ENABLED
    uint8_t                         RESERVED    : 1; 
    uint8_t                         RESERVED    : 1;
    uint8_t                         RESERVED    : 1; 
    void                            *RESERVED;       
#else
    bool                            RESERVED;      
#endif
    
    FLASH uint8_t                   *RESERVED;           
    void                            *RESERVED;
END_EXTERN_CLASS(task_t)

#undef RESERVED

/*! \brief task function prototype
 *  \param pArg start address of a task argument control block
 *  \param (void * volatile)
 *  \retval true current task want to keep running.
 *  \retval false current task completed
 */
typedef fsm_rt_t safe_task_func_t(void *pArg, void *);

DECLARE_CLASS(safe_call_stack_item_t)
//! \name call stack item
//! @{
EXTERN_CLASS(safe_call_stack_item_t)
    safe_task_func_t  *fnRoutine;          //!< routine
    void            *pArg;               //!< argument
END_EXTERN_CLASS(safe_call_stack_item_t);
//! @}


#if SAFE_TASK_THREAD_SYNC == ENABLED

DECLARE_CLASS(fsm_flag_t)

//! \name task event item
//! @{
EXTERN_CLASS(fsm_flag_t)
    bool            bSignal;            //!< signal
    volatile void  *ptHead;             //!< task item  
    volatile void  *ptTail;
    bool            bManualReset;       //!< manual reset flag
    locker_t        tLocker;            //!< thread locker
END_EXTERN_CLASS(fsm_flag_t)
//! @}

//! \name event
typedef fsm_flag_t  event_t;

#   if SAFE_TASK_CRITICAL_SECTION == ENABLED
//! \name critical section
typedef event_t critical_section_t;
#   endif

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

/*============================ GLOBAL VARIABLES ==============================*/

//#if !defined(__LIB_REFERENCE__)
////! \brief ES-scheduler
//extern const scheduler_t SCHEDULER;
//#else
//#define SCHEDULER       (*(const scheduler_t *)GSF_SCHEDULER_BASE_ADDRESS)
//#endif

extern const scheduler_t SCHEDULER;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \note initialize scheduler
 *  \param none
 *  \return none
 */
extern void scheduler_init( void );

/*! \brief finish scheduler
 *  \param none
 *  \return none
 */
extern void scheduler_finish( void );

/*! \brief add tasks to the task pool
 *  \param pHead task heap buffer
 *  \param hwSize heap size
 *  \return access result
 */
extern bool task_pool_init(void *pHeap, uint16_t hwSize);

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
extern void *register_task(
        safe_task_func_t *fnRoutine,
        void *pArg,
        volatile safe_call_stack_item_t *pStack,
        uint8_t chStackSize  );
#else
/*! \brief create a new task control block
 *  \param fnRoutine task routine
 *  \param fnReturnTo a routine which will be called when task routine return
 *         false.
 *  \param pArg a pointer of a argument control block
 *  \return task handle
 */
extern void *register_task( safe_task_func_t *fnRoutine, void *pArg );
#endif


/*! \brief scheduler function
 *  \retval true system is busy
 *  \retval false system is idle
 */
extern bool scheduler( void );


/*! \brief call a sub task (routine)
 *  \param pT a pointer of task control block
 *  \param fnRoutine target routine
 *  \param pArg a pointer of argument control block
 *  \param bSubcall is sub-task to be called
 *  \retval false failed to add a task control block
 *  \retval true succeeded in add a task
 */
extern bool call_task(  void *pT, safe_task_func_t *fnRoutine , 
                         void *pArg , bool bSubcall);

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
extern bool call_task_ex( void *pT,
                  safe_task_func_t *fnRoutine,
                  void *pArg,
                  safe_task_func_t *fnReturnRoutine,
                  void *pReturnArg,
                  bool bSubcall ) ;

#if SAFE_TASK_THREAD_SYNC == ENABLED

/*! \brief initialize task event
 *! \param ptEvent event object
 *! \param bManualReset flag that indicates whether the event should reset to 
 *!        inactived state automatically.
 *! \param bInitialState event initial state, either set or not.
 *! \return pointer for event object
 */
extern event_t *create_event(event_t *, bool bManualReset, bool bInitialState);

/*! \brief set task event
 *! \param ptEvent pointer for task event
 *! \return none
 */
extern void set_event(event_t *);

/*! \brief reset specified task event
 *! \param ptEvent task event pointer
 *! \return none
 */
extern void reset_event(event_t *);

/*! \brief wait for a specified task event
 *! \param ptEvent target event item
 *! \param pTask parasitifer task
 *! \retval true event raised
 *! \retval false event haven't raised yet.
 */
extern bool wait_for_single_object(fsm_flag_t *, void *);

#   if SAFE_TASK_CRITICAL_SECTION == ENABLED

/*! \brief try to enter critical section
 *! \param critical section item
 *! \return none
 */
extern void leave_critical_section(critical_section_t *);

#   endif

#endif


#endif
#endif
/* EOF */
