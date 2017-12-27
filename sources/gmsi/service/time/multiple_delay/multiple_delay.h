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

#ifndef __MULTIPLE_DELAY_H__
#define __MULTIPLE_DELAY_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_SERVICE_MULTIPLE_DELAY == ENABLED
#include "..\..\memory\epool\epool.h"
/*============================ MACROS ========================================*/

#define MULTIPLE_DELAY_CFG(__ADDR, ...)                 \
    do {                                                \
        multiple_delay_cfg_t tCFG = {                   \
            __VA_ARGS__                                 \
        };                                              \
                                                        \
        MULTIPLE_DELAY.Init((__ADDR), &tCFG);           \
    } while(false)


/*============================ MACROFIED FUNCTIONS ===========================*/
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
extern_class(multiple_delay_item_t)
    inherit(__single_list_node_t)                                               //!< list pointer
    uint32_t wTargetTime;                                                       //!< timeout target time
    multiple_delay_request_priority_t tPriority;                                //!< request priority
    void *pTag;                                                                 //!< object passed to timeout event handler
    timeout_event_handler_t *fnHandler;                                         //!< time out event handler
end_extern_class(multiple_delay_item_t)
//! @}

declare_class(multiple_delay_t)

extern_simple_fsm( multiple_delay_task,
    def_params(
        multiple_delay_t *ptObj;
    ))


EXTERN_EPOOL(multiple_delay_item_heap_t, multiple_delay_item_t) 

END_EXTERN_EPOOL(multiple_delay_item_heap_t)


//! \name multiple delay service control block
//! @{

extern_class(multiple_delay_t, 
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
    
end_extern_class(multiple_delay_t,
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

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern const i_multiple_delay_t MULTIPLE_DELAY;
    
#endif
#endif
/* EOF */
