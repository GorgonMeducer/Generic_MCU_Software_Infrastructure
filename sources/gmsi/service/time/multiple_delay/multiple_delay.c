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
#include "..\..\memory\epool\epool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef __MD_ATOM_ACCESS
#   define __MD_ATOM_ACCESS(...)            SAFE_ATOM_CODE(__VA_ARGS__)
#endif

/*============================ TYPES =========================================*/

//! \name delay status
//! @{
typedef enum {
    MULTIPLE_DELAY_TIMEOUT = 0,                                                 //!< timout
    MULTIPLE_DELAY_CANCELLED,                                                   //!< delay request is cancelled by user
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
    inherit(__single_list_node_t)                                               //!< list pointer
    uint32_t wTargetTime;                                                       //!< timeout target time
    multiple_delay_request_priority_t tPriority;                                //!< request priority
    void *pTag;                                                                 //!< object passed to timeout event handler
    timeout_event_handler_t *fnHandler;                                         //!< time out event handler
end_def_class(multiple_delay_item_t)
//! @}

declare_class(multiple_delay_t)

simple_fsm( multiple_delay_task,
    def_params(
        multiple_delay_t *ptObj;
    ))


DEF_EPOOL(multiple_delay_item_heap_t, multiple_delay_item_t)

END_DEF_EPOOL(multiple_delay_item_heap_t)


//! \name multiple delay service control block
//! @{

def_class(multiple_delay_t, 
    which ( inherit(    fsm(multiple_delay_task))
            inherit(    EPOOL(multiple_delay_item_heap_t) )))                   //!< fsm for multiple delay task))

    multiple_delay_item_t       *ptHighPriorityDelayList;                       //!< hight priority list
    multiple_delay_item_t       *ptDelayList;                                   //!< normal priority list
    struct {
        multiple_delay_item_t       *ptHead;                                    //!< timeout list Head
        multiple_delay_item_t       *ptTail;                                    //!< timeout list Tail
    } LowPriorityEvent;
    
    struct {
        multiple_delay_item_t       *ptHead;                                    //!< timeout list Head
        multiple_delay_item_t       *ptTail;                                    //!< timeout list Tail
    } NormalPriorityEvent;
    
    uint32_t                    wOldCounter;                                    //!< Old tick counter number 
    uint32_t                    wCounter;                                       //!< Tick Counter
    uint32_t                    wSavedCounter;
    
end_def_class(multiple_delay_t,
    which ( inherit(    fsm(multiple_delay_task))
            inherit(    EPOOL(multiple_delay_item_heap_t) )))
//! @}

typedef struct {
    union {
        mem_block_t;
        mem_block_t tHeapBuffer;
    };
}multiple_delay_cfg_t;

def_interface(i_multiple_delay_t)
    bool        (*Init)            (multiple_delay_t *, multiple_delay_cfg_t *);
    fsm_rt_t    (*Task)            (multiple_delay_t *);
    
    multiple_delay_item_t * 
                (*RequestDelay)    (multiple_delay_t *                         , 
                                    uint32_t wDelay                            ,
                                    multiple_delay_request_priority_t tPriority,//!< request priority
                                    void *pTag                                 ,//!< object passed to timeout event handler
                                    timeout_event_handler_t *fnHandler);
    void        (*Cancel)          (multiple_delay_t *ptObj, 
                                    multiple_delay_item_t *ptItem);
    struct {
        void    (*TimerTickService)(multiple_delay_t *);
    } Dependent;
    
end_def_interface(i_multiple_delay_t)



    
    
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

private void insert_timer_tick_event_handler(  multiple_delay_t *ptObj);

private bool init(  multiple_delay_t *ptObj,  multiple_delay_cfg_t *ptCFG );

private fsm_rt_t task(multiple_delay_t *ptObj);

private multiple_delay_item_t * request_delay(  
                            multiple_delay_t *ptObj,
                            uint32_t wDelay,
                            multiple_delay_request_priority_t tPriority,        //!< request priority
                            void *pTag,                                         //!< object passed to timeout event handler
                            timeout_event_handler_t *fnHandler);
private void cancel_delay(multiple_delay_t *ptObj, multiple_delay_item_t *ptItem);

private declare_fsm_implementation(multiple_delay_task);
private declare_fsm_initialiser(multiple_delay_task, 
            args(
                multiple_delay_t *ptObj
            ));

/*============================ GLOBAL VARIABLES ==============================*/

#if defined(LIB_GENERATION)
ROOT
#endif
const i_multiple_delay_t MULTIPLE_DELAY = {
        
        .Init =                     &init,
        .Task =                     &task,
        .RequestDelay =             &request_delay,
        .Cancel =                   &cancel_delay,
        .Dependent = {
            .TimerTickService =     &insert_timer_tick_event_handler,
        },
    };
    

/*============================ IMPLEMENTATION ================================*/

private fsm_rt_t task(multiple_delay_t *ptObj)
{
    class_internal(ptObj, ptThis, multiple_delay_t);
    if (NULL == ptThis) {
        return fsm_rt_err;
    }

    return call_fsm(    multiple_delay_task, 
                        ref_obj_as(this, fsm(multiple_delay_task)));
    
}
        
        
private void add_to_delay_list(  multiple_delay_item_t *ptItem, 
                                multiple_delay_item_t **ppList) 
{
    class_internal(ptItem, ptTarget, multiple_delay_item_t);
    __MD_ATOM_ACCESS (
        do {
            class_internal((*ppList), ptListItem, multiple_delay_item_t);
            
            if (NULL == ptListItem) {
                LIST_INSERT_AFTER((*ppList), ptItem);
                break;
            }
            
            if (target.wTargetTime <= ptListItem->wTargetTime) {
                
                LIST_INSERT_AFTER((*ppList), ptItem);
                break;
            }
            
            ppList = (multiple_delay_item_t **)&(ptListItem->ptNext);
            
        } while(true);
    )
}

private bool remove_from_delay_list( multiple_delay_item_t *ptItem, 
                                    multiple_delay_item_t **ppList)
{
    class_internal(ptItem, ptTarget, multiple_delay_item_t);
    bool bResult = false;
    __MD_ATOM_ACCESS (
        do {
            class_internal((*ppList), ptListItem, multiple_delay_item_t);
            
            if (NULL == ptListItem) {
                break;
            }
            
            if (ptListItem == &target) {
                LIST_REMOVE_AFTER((*ppList), ptItem);
                bResult = true;
                break;
            }
            
            ppList = (multiple_delay_item_t **)&(ptListItem->ptNext);
            
            
        } while(true);
    )

    return bResult;
}

private void cancel_delay(multiple_delay_t *ptObj, multiple_delay_item_t *ptItem)
{
    class_internal(ptObj, ptThis, multiple_delay_t);
    class_internal(ptItem, ptTarget, multiple_delay_item_t);
    if (NULL == ptThis || NULL == ptItem) {
        return ;
    }
    uint32_t tCount = 2;
    
    switch( target.tPriority ) {

        while(0 != tCount) {
            case MULTIPLE_DELAY_HIGH_PRIORITY:
                if (remove_from_delay_list(ptItem, &this.ptHighPriorityDelayList)) {
                    break;
                }
                tCount--;
            case MULTIPLE_DELAY_LOW_PRIORITY:
            case MULTIPLE_DELAY_NORMAL_PRIORITY:
            default:
                if (remove_from_delay_list(ptItem, &this.ptDelayList)) {
                    break;
                }
                tCount--;
        }
    }
    
    if (0 == tCount) {
        return;
    }
    
    //! raise event handler
    if (NULL != target.fnHandler) {
        //! call timeout handler
        (*target.fnHandler)( MULTIPLE_DELAY_CANCELLED, target.pTag);
    }
    
    //! free obj
    EPOOL_FREE( multiple_delay_item_heap_t,
                ref_obj_as(this, EPOOL(multiple_delay_item_heap_t)),
                ptItem);
}

private multiple_delay_item_t * request_delay(  
                            multiple_delay_t *ptObj,
                            uint32_t wDelay,
                            multiple_delay_request_priority_t tPriority,        //!< request priority
                            void *pTag,                                         //!< object passed to timeout event handler
                            timeout_event_handler_t *fnHandler)  
            
{
    class_internal(ptObj, ptThis, multiple_delay_t);
    multiple_delay_item_t *ptNewItem = NULL;
       
    
    do {
        uint32_t wCurrentCounter;
        
        if (NULL == ptThis || 0 == wDelay || NULL == fnHandler) {
            break;
        }
        
        __MD_ATOM_ACCESS(
            wCurrentCounter = this.wCounter;
        )
        if ((uint32_t)(wCurrentCounter + wDelay) < wCurrentCounter) {
            //! prevent overflow
            break;
        } 
        
        //! allocate an delay item
        ptNewItem = EPOOL_NEW(  multiple_delay_item_heap_t,
                        ref_obj_as(this, EPOOL(multiple_delay_item_heap_t)));
        if (NULL == ptNewItem) {
            break;
        }
                  
        do
        {
            
            class_internal(ptNewItem, ptTarget, multiple_delay_item_t);
            
            target.fnHandler = fnHandler;
            target.pTag = pTag;
            target.tPriority = tPriority;
            target.wTargetTime = wCurrentCounter + wDelay;
            
            switch( tPriority ) {

                case MULTIPLE_DELAY_LOW_PRIORITY:
                case MULTIPLE_DELAY_NORMAL_PRIORITY:
                default:
                    add_to_delay_list(ptNewItem, &this.ptDelayList);
                    break;
                case MULTIPLE_DELAY_HIGH_PRIORITY:
                    add_to_delay_list(ptNewItem, &this.ptHighPriorityDelayList);
                    break;
            }
        } while(false);

    } while(false);
    
    return ptNewItem;
}
    
        
private void insert_timer_tick_event_handler(multiple_delay_t *ptObj)
{
    class_internal(ptObj, ptThis, multiple_delay_t);
    
    if (NULL == ptThis) {
        return ;
    }

    //! visit each node of High Priority Delay List (sorted list)
    do {
        if (NULL == this.ptHighPriorityDelayList) {
            if (NULL == this.ptDelayList) {
                this.wCounter = 0;
                this.wOldCounter = 0;
                this.wSavedCounter = 0;
                break;
            }
        } else {

            do {
                class_internal( this.ptHighPriorityDelayList, 
                                ptTarget, 
                                multiple_delay_item_t);
                                
                if (NULL == ptTarget) {
                    break;
                }
                
                if (target.wTargetTime <= this.wCounter) {
                    //! timeout detected
                    LIST_STACK_POP(this.ptHighPriorityDelayList, ptTarget);
                    if (NULL != target.fnHandler) {
                        //! call timeout handler
                        (*target.fnHandler)( MULTIPLE_DELAY_TIMEOUT, target.pTag);
                    }
                    
                    EPOOL_FREE( multiple_delay_item_heap_t,
                            ref_obj_as(this, EPOOL(multiple_delay_item_heap_t)),
                            (multiple_delay_item_t *)&target);
                } else {
                    break;
                }
            } while(true);
        }
        
        //! increase the counter
        this.wCounter++;
        
    } while(false);
}
        

        
private bool init(  multiple_delay_t *ptObj,  multiple_delay_cfg_t *ptCFG )
{
    class_internal(ptObj, ptThis, multiple_delay_t);
    if (NULL == ptThis) {
        return false;
    }
    
    do {
        if (NULL == ptThis || NULL == ptCFG) {
            break; 
        } else if (NULL == ptCFG->pchBuffer) {
            break;
        } else if (ptCFG->wSize < sizeof(multiple_delay_item_t)) {
            break;
        }
        
        memset(ptThis, 0, sizeof(multiple_delay_t));
        
        if (!EPOOL_INIT( multiple_delay_item_heap_t, 
                    ref_obj_as(this, EPOOL(multiple_delay_item_heap_t)))) {
            break;
        }
        
        EPOOL_ADD_HEAP(  multiple_delay_item_heap_t,
                    ref_obj_as(this, EPOOL(multiple_delay_item_heap_t)),
                    ptCFG->pchBuffer, ptCFG->wSize);

        return NULL != init_fsm(multiple_delay_task, 
                        &base_obj(fsm(multiple_delay_task)), 
                        args(ptObj));
    } while(false);
    
    return false;
}
        

private fsm_initialiser(multiple_delay_task, 
    args(
        multiple_delay_t *ptObj
    ))
    init_body(
        if (NULL == ptObj) {
            abort_init();
        }
        
        this.ptObj = ptObj;
    )
    

private fsm_implementation(multiple_delay_task) 
        
    def_states(CHECK_LIST, RAISE_NORMAL_PRIORITY_EVENT, RAISE_LOW_PRIORITY_EVENT)    
    class_internal(this.ptObj, ptTarget, multiple_delay_t);
      
      
    body(
        on_start(
            if (NULL == ptTarget) {
                fsm_report(GSF_ERR_INVALID_PTR);
            }
            
            __MD_ATOM_ACCESS(
                target.wSavedCounter = target.wCounter;
            )
            
            if (target.wOldCounter == target.wSavedCounter) {
                fsm_continue();
            }
            
            target.wOldCounter = target.wSavedCounter;
        )
            
            
        state( CHECK_LIST,

            do {
                class_internal(target.ptDelayList, ptItem, multiple_delay_item_t);
                
                if (NULL == target.ptDelayList) {
                    break;
                }
                
                if (ptItem->wTargetTime <= target.wSavedCounter) {
 
                    __MD_ATOM_ACCESS (
                        //! timeout detected
                        LIST_STACK_POP(target.ptDelayList, ptItem);
                    )
                    
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
                
                EPOOL_FREE( multiple_delay_item_heap_t,
                            ref_obj_as(target, EPOOL(multiple_delay_item_heap_t)),
                            (multiple_delay_item_t *)ptItem);
                
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
                
                EPOOL_FREE( multiple_delay_item_heap_t,
                            ref_obj_as(target, EPOOL(multiple_delay_item_heap_t)),
                            (multiple_delay_item_t *)ptItem);
                
            } while(false);
            
            fsm_continue();
        )

    )
        
#endif
/* EOF */
