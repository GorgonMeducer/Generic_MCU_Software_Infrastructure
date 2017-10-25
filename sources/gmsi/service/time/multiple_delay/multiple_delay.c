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
#if USE_SERVICE_MULTIPLE_DELAY == ENABLED

#include <string.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name delay status
//! @{
typedef enum {
    MULTIPLE_DELAY_TIMEOUT = 0,                         //!< timout
    MULTIPLE_DELAY_CANCELLED,                           //!< delay request is cancelled by user
} multiple_delay_report_status_t;
//! @}

typedef enum {
    MULTIPLE_DELAY_LOW_PRIORITY,
    MULTIPLE_DELAY_NORMAL_PRIORITY,
    MULTIPLE_DELAY_HIGH_PRIORITY,
} multiple_delay_request_priority_t;

//! \brief delay request timeout event handler prototype (delegate)
typedef void timeout_event_handler_t(multiple_delay_report_status_t tStatus, void *pObj);

//! \name delay task item
//! @{
declare_class(multiple_delay_item_t)
def_class(multiple_delay_item_t)
    inherit(__single_list_node_t)                           //!< list pointer
    uint32_t wTargetTime;                                   //!< timeout target time
    multiple_delay_request_priority_t tPriority;            //!< request priority
    void *pTag;                                             //!< object passed to timeout event handler
    timeout_event_handler_t *fnHandler;                     //!< time out event handler
end_def_class(multiple_delay_item_t)
//! @}

declare_class(multiple_delay_t)

simple_fsm( multiple_delay_task,
    def_params(
        multiple_delay_t *ptObj;
    ))


//! \name multiple delay service control block
//! @{

def_class(multiple_delay_t)

    multiple_delay_item_t       *ptHighPriorityDelayList;   //!< hight priority list
    multiple_delay_item_t       *ptDelayList;               //!< normal priority list
    struct {
        multiple_delay_item_t       *ptHead;                //!< timeout list Head
        multiple_delay_item_t       *ptTail;                //!< timeout list Tail
    } LowPriorityEvent;
    
    struct {
        multiple_delay_item_t       *ptHead;                //!< timeout list Head
        multiple_delay_item_t       *ptTail;                //!< timeout list Tail
    } NormalPriorityEvent;
    
    uint32_t                    wOldCounter;                //!< Old tick counter number 
    uint32_t                    wCounter;                   //!< Tick Counter
    
    inherit(fsm(multiple_delay_task))                       //!< fsm for multiple delay task

end_def_class(multiple_delay_t)
//! @}


def_interface(i_multiple_delay_t)
    bool        (*Init)             (multiple_delay_t *);
    struct {
        void    (*TimerTickService) (multiple_delay_t *);
    } Dependent;
    
end_def_interface(i_multiple_delay_t)



/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void declare_method_implementation(  multiple_delay_t, 
                            insert_timer_tick_event_handler);

static bool declare_method_implementation(  multiple_delay_t, init);
    
extern_fsm_initialiser(multiple_delay_task, 
    args(
        multiple_delay_t *ptObj
    ));

/*============================ GLOBAL VARIABLES ==============================*/
const i_multiple_delay_t MULTIPLE_DELAY = {
        
        .Init =                     &init,
        .Dependent = {
            .TimerTickService =     &insert_timer_tick_event_handler,
        },
        
    };
    

/*============================ IMPLEMENTATION ================================*/

/*
static void method_implementation(  multiple_delay_item_t, check_delay_list_fast, 
    args(
        uint32_t wCounter
    ))
    method_body(
        
        while (NULL != ptThis) {
            
            if (this.wTargetTime <= wCounter) {
                //! timeout detected, raise timeout event immediately 
                if (NULL != this.fnHandler) {
                    this.fnHandler( MULTIPLE_DELAY_TIMEOUT, this.pTag);
                }
            }
            
            ptThis = (class(multiple_delay_item_t) *)this.ptNext;
        }
    )

static INLINE multiple_delay_item_t * method_implementation(  multiple_delay_item_t, get_next)
    method_body(
        if (NULL != ptThis) {
            return (multiple_delay_item_t *)(ptThis->ptNext);
        }
        
        return (multiple_delay_item_t *)ptThis;
    )
        
static multiple_delay_item_t * method_implementation(  multiple_delay_item_t, find_timeout, 
    args(
        uint32_t wCounter
    ))
    method_body(
        
        while (NULL != ptThis) {
            
            if (this.wTargetTime <= wCounter) {
                break;
            }
            
            ptThis = (class(multiple_delay_item_t) *)this.ptNext;
        }
        
        return (multiple_delay_item_t *)ptThis;
    )
*/

static void
method_implementation(  multiple_delay_t, 
                        insert_timer_tick_event_handler)
    method_body(
        if (NULL == ptThis) {
            return ;
        }
        
        //! increase the counter
        this.wCounter++;
        
        //! visit each node of High Priority Delay List (sorted list)
        do {
            if (NULL == this.ptHighPriorityDelayList) {
                break;
            }
            
            do {
                class_internal(this.ptHighPriorityDelayList, ptTarget, multiple_delay_item_t);
                if (target.wTargetTime <= this.wCounter) {
                    //! timeout detected
                    LIST_STACK_POP(this.ptHighPriorityDelayList, ptTarget);
                    if (NULL != target.fnHandler) {
                        //! call timeout handler
                        (*target.fnHandler)( MULTIPLE_DELAY_TIMEOUT, target.pTag);
                    }
                } else {
                    break;
                }
            } while(true);
            
            
        } while(false);
    )
        
        
static bool
method_implementation(  multiple_delay_t, init)
    method_body(
        do {
            if (NULL == ptThis) {
                break; 
            }
            
            memset(ptThis, 0, sizeof(this));
            
            
            return init_fsm(multiple_delay_task, 
                            &base_obj(fsm(multiple_delay_task)), 
                            args(ptObj));
        } while(false);
        
        return false;
    )
        

fsm_initialiser(multiple_delay_task, 
    args(
        multiple_delay_t *ptObj
    ))
    init_body(
        if (NULL == ptObj) {
            abort_init();
        }
        
        this.ptObj = ptObj;
    )
        
fsm_implementation(multiple_delay_task) 
        
    def_states(CHECK_LIST, RAISE_NORMAL_PRIORITY_EVENT, RAISE_LOW_PRIORITY_EVENT)    
    class_internal(this.ptObj, ptTarget, multiple_delay_t);
        
    body(
        on_start(
            if (NULL == ptTarget) {
                fsm_report(GSF_ERR_INVALID_PTR);
            }
            
            if (target.wOldCounter == target.wCounter) {
                fsm_continue();
            }
        )
            
        state(CHECK_LIST,
            do {
                if (NULL == target.ptDelayList) {
                    break;
                }
                
                do {
                    class_internal(target.ptDelayList, ptItem, multiple_delay_item_t);
                    if (ptItem->wTargetTime <= target.wCounter) {
                        //! timeout detected
                        LIST_STACK_POP(target.ptDelayList, ptItem);
                        
                        if (ptItem->tPriority == MULTIPLE_DELAY_LOW_PRIORITY) {
                            //! add the item to the low priority timeout list
                            LIST_QUEUE_ENQUEUE( target.LowPriorityEvent.ptHead, 
                                                target.LowPriorityEvent.ptTail, 
                                                ptItem);
                        } else {
                            //! add the item to the normal priority timeout list
                            LIST_QUEUE_ENQUEUE( target.NormalPriorityEvent.ptHead, 
                                                target.NormalPriorityEvent.ptTail, 
                                                ptItem);
                        }
                        
                    } else {
                        break;
                    }
                } while(true);
                
            } while(false);
                
            update_state_to(RAISE_NORMAL_PRIORITY_EVENT);
        )
            
        privilege_state(RAISE_NORMAL_PRIORITY_EVENT,
            do {
                class(multiple_delay_item_t) *ptItem;
                
                LIST_QUEUE_DEQUEUE( target.NormalPriorityEvent.ptHead, 
                                    target.NormalPriorityEvent.ptTail, 
                                    ptItem);
                
                if (NULL == ptItem) {
                    //! no pending list
                    transfer_to(RAISE_LOW_PRIORITY_EVENT);
                }
                
                if (NULL != ptItem->fnHandler) {
                    (*(ptItem->fnHandler))(MULTIPLE_DELAY_TIMEOUT, ptItem->pTag);
                }
                
            } while(false);
            
            fsm_continue();
        )
            
        state(RAISE_LOW_PRIORITY_EVENT,
            do {
                class(multiple_delay_item_t) *ptItem;
                
                LIST_QUEUE_DEQUEUE( target.LowPriorityEvent.ptHead, 
                                    target.LowPriorityEvent.ptTail, 
                                    ptItem);
                
                if (NULL == ptItem) {
                    //! no pending list
                    fsm_cpl();
                }
                
                if (NULL != ptItem->fnHandler) {
                    (*(ptItem->fnHandler))(MULTIPLE_DELAY_TIMEOUT, ptItem->pTag);
                }
                
            } while(false);
            
            fsm_continue();
        )
    )
        
#endif
/* EOF */
