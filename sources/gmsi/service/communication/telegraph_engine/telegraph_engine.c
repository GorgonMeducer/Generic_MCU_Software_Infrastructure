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

#if USE_SERVICE_TELEGRAPH_ENGINE == ENABLED
#include <string.h>
#include "..\..\time\multiple_delay\multiple_delay.h"
#include "..\..\memory\block\block.h"

#include "..\..\time\multiple_delay\multiple_delay.h"

/*============================ MACROS ========================================*/
#ifndef TELEGRAPH_ENGINE_FRAME_ERROR   
#   define TELEGRAPH_ENGINE_FRAME_ERROR   0xF0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef __TE_ATOM_ACCESS
#   define __TE_ATOM_ACCESS(...)            SAFE_ATOM_CODE(__VA_ARGS__)
#endif

/*============================ TYPES =========================================*/

declare_class(telegraph_t)
declare_class(telegraph_engine_t)

//! \name telegraph report status
//! @{
typedef enum {
    TELEGRAPH_ERROR     = -1,                                                   //!< error detected during checking
    TELEGRAPH_RECEIVED  = 0,                                                    //!< expected telegraph is received
    TELEGRAPH_TIMEOUT,                                                          //!< timeout
    TELEGRAPH_CANCELLED,                                                        //!< telegraph is cancelled by user
} telegraph_report_t;
//! @}

//! \name telegraph report event handler prototype (delegate)
//! \param tStatus the reason to report
//! \param ptTelegraph target telegraph
//! \retval true it's safe to free this telegraph
//! \retval false do not free the telegraph
typedef bool telegraph_handler_t (telegraph_report_t tStatus, telegraph_t *ptTelegraph);

//! \name abstruct class telegraph, user telegraph should inherit from this class
//! @{
def_class(telegraph_t)
    inherit(__single_list_node_t)    
    telegraph_engine_t      *ptEngine;
    telegraph_handler_t     *fnHandler;
    multiple_delay_item_t   *ptDelayItem;
    uint32_t                wTimeout;
    block_t                 *ptOUTData;
    block_t                 *ptINData;
end_def_class(telegraph_t)
//! @}

typedef enum {
    FRAME_UNKNOWN       = -1,
    FRAME_UNMATCH       = 0,
    FRAME_RECEIVED      = 1,
} frame_parsing_report_t;

typedef frame_parsing_report_t telegraph_parser_t(
                                                    block_t **pptBlock,         //! memory buffer
                                                    telegraph_t *ptItem);       //! target telegraph 

typedef fsm_rt_t telegraph_engine_low_level_write_io_t(telegraph_t *ptItem, void *pObj);


simple_fsm(telegraph_engine_task,
    def_params(
        telegraph_t *ptCurrent;
    )
)



//! \name telegraph engine
//! @{
def_class(telegraph_engine_t,   
    which(  inherit(fsm(telegraph_engine_task))
            inherit(pool_t)))

    struct {
        telegraph_t                             *ptHead;
        telegraph_t                             *ptTail;
    } Listener;
    
    struct {
        telegraph_t                             *ptHead;
        telegraph_t                             *ptTail;
    } Transmitter; 
    
    telegraph_parser_t                          *fnDecoder;
    multiple_delay_t                            *ptDelayService;
    telegraph_engine_low_level_write_io_t       *fnWriteIO;
    void                                        *pIOTag;
end_def_class(telegraph_engine_t, 
    which(  inherit(fsm(telegraph_engine_task))
            inherit(pool_t)))
//! @}

typedef struct {
    mem_block_t                             tTelegraphPool;
    uint32_t                                wTelegraphSize;
    
    telegraph_parser_t                      *fnDecoder;
    multiple_delay_t                        *ptDelayService;
    telegraph_engine_low_level_write_io_t   *fnWriteIO;
    void                                    *pIOTag;
} telegraph_engine_cfg_t;


def_interface(i_telegraph_engine_t)
    bool            (*Init)     (   telegraph_engine_t *ptObj, 
                                    telegraph_engine_cfg_t *ptCFG);
    fsm_rt_t        (*Task)     (   telegraph_engine_t *ptObj);
    struct {
        block_t *   (*Parse)    (   block_t *ptBlock, telegraph_engine_t *ptObj);
    } Dependent;
    
    struct {
        telegraph_t *(*New)     (   telegraph_engine_t *ptObj,
                                    telegraph_handler_t *fnHandler, 
                                    uint32_t wTimeout, 
                                    block_t *ptData);
        bool        (*TryToSend)(   telegraph_t *ptTelegraph,
                                    bool bPureListener);
        bool        (*Listen)   (   telegraph_t *ptTelegraph);
        
        struct {
            struct {
                block_t *(*Get)(telegraph_t *ptTelegraph);
                void (*Reset)(telegraph_t *ptTelegraph);
            }Input;
            struct {
                block_t *(*Get)(telegraph_t *ptTelegraph);
                void (*Reset)(telegraph_t *ptTelegraph);
            }Output;
        } Data;
        
        bool        (*IsWriteOnly) (telegraph_t *ptTelegraph);
        bool        (*IsReadOnly)  (telegraph_t *ptTelegraph);
        
    } Telegraph;
end_def_interface(i_telegraph_engine_t)


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

private bool init(telegraph_engine_t *ptObj, telegraph_engine_cfg_t *ptCFG);
private block_t * frontend(block_t *ptBlock, telegraph_engine_t *ptObj);
private bool try_to_send_telegraph( telegraph_t *ptTelegraph, bool bListener);
private bool try_to_listen( telegraph_t *ptTelegraph);
private fsm_rt_t task(telegraph_engine_t *ptObj);

private block_t *get_input_block(telegraph_t *ptTelegraph);
private block_t *get_output_block(telegraph_t *ptTelegraph);
private bool is_write_only_telegraph(telegraph_t *ptTelegraph);
private bool is_read_only_telegraph(telegraph_t *ptTelegraph);

private telegraph_t * telegraph_init(    telegraph_engine_t *ptObj,
                                        telegraph_handler_t *fnHandler, 
                                        uint32_t wTimeout, 
                                        block_t *ptData);
private void reset_input_block(telegraph_t *ptTelegraph);
private void reset_output_block(telegraph_t *ptTelegraph);

/*============================ GLOBAL VARIABLES ==============================*/

const i_telegraph_engine_t TELEGRAPH_ENGINE = {
    .Init =                 &init,
    .Task =                 &task,
    .Dependent = {
        .Parse =            &frontend,
    },
    .Telegraph = {
        .TryToSend =        &try_to_send_telegraph,
        .Listen =           &try_to_listen,
        .New =              &telegraph_init,
        .Data = {
            .Input = {
                .Get =      &get_input_block,
                .Reset =    &reset_input_block,
            },
            .Output = {
                .Get =      &get_output_block,
                .Reset =    &reset_output_block,
            },
        },
        .IsWriteOnly =      &is_write_only_telegraph,
        .IsReadOnly =       &is_read_only_telegraph,
    },
};

/*============================ IMPLEMENTATION ================================*/

private fsm_initialiser(telegraph_engine_task)
    init_body()


private fsm_implementation(telegraph_engine_task)
    def_states(FETCH_TELEGRAPH, SEND_TELGRAPH, REGISTER_LISTENER)
    
    class_internal(ptThis, ptBase, telegraph_engine_t);
    
    body(
        
        on_start(
            if (NULL == base.fnWriteIO) {
                fsm_report(GSF_ERR_NOT_SUPPORT);
            }
        )
        
        state(FETCH_TELEGRAPH,
            
            __TE_ATOM_ACCESS(
                LIST_QUEUE_DEQUEUE( base.Transmitter.ptHead, 
                                    base.Transmitter.ptTail, 
                                    this.ptCurrent);
            )
            
            if (NULL == this.ptCurrent) {
                fsm_cpl();
            }
            
            update_state_to(SEND_TELGRAPH);
        )
        
        state(SEND_TELGRAPH,
            {
                class_internal(this.ptCurrent, ptTarget, telegraph_t);
                fsm_rt_t tResult = (*(base.fnWriteIO))(this.ptCurrent, base.pIOTag );
                
                if (IS_FSM_ERR(tResult)) {
                    fsm_report(tResult);
                } else if (fsm_rt_cpl != tResult) {
                    fsm_continue();
                } 
                
                if (NULL == target.fnHandler) {
                
                    //! free telegraph
                    pool_free(ref_obj_as(base, pool_t), ptTarget);
                
                    //! pure sender
                    transfer_to(FETCH_TELEGRAPH);
                } else {
                    update_state_to(REGISTER_LISTENER);
                }
            }
        )
        
        state(REGISTER_LISTENER,
        
            //! add the target telegraph to listener list
            if (!try_to_listen(this.ptCurrent)) {
                fsm_continue();
            }
            
            transfer_to(FETCH_TELEGRAPH);
        )
    )
    
private fsm_rt_t task(telegraph_engine_t *ptObj)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    if (NULL == ptThis) {
        return fsm_rt_err;
    }
    
    return call_fsm(telegraph_engine_task, 
                    ref_obj_as(this, fsm(telegraph_engine_task)));
}

private bool init(telegraph_engine_t *ptObj, telegraph_engine_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    do {
        if (NULL == ptThis || NULL == ptCFG) {
            break;
        } else if (     (NULL == ptCFG->tTelegraphPool.pchSrc) 
                    ||  (ptCFG->tTelegraphPool.hwSize < sizeof(telegraph_t))
                    ||  (ptCFG->wTelegraphSize < sizeof(telegraph_t))) {
            break;
        }
        
        memset((void *)ptObj, 0, sizeof(telegraph_engine_t));
        

        this.fnDecoder = ptCFG->fnDecoder;
        this.ptDelayService = ptCFG->ptDelayService;
        this.fnWriteIO = ptCFG->fnWriteIO;
        this.pIOTag = ptCFG->pIOTag;
        init_fsm(   telegraph_engine_task, 
                    ref_obj_as(this, fsm(telegraph_engine_task)));
        
        //! initialise telegraph pool
        pool_init(ref_obj_as(this, pool_t));
        
        //! add buffer to telegraph heap
        pool_add_heap(  ref_obj_as(this, pool_t), 
                        ptCFG->tTelegraphPool.pObj,
                        ptCFG->tTelegraphPool.hwSize,
                        ptCFG->wTelegraphSize);
        
        
        return true;
    } while(false);

    return false;
}

private telegraph_t * telegraph_init(    telegraph_engine_t *ptObj,
                                        telegraph_handler_t *fnHandler, 
                                        uint32_t wTimeout, 
                                        block_t *ptData)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        class_internal(pool_new(ref_obj_as(this, pool_t)), ptTarget, telegraph_t);
        
        if (NULL == ptTarget) {
            break;
        }
        memset(ptTarget, 0, sizeof(telegraph_t));
        
        target.fnHandler = fnHandler;
        target.wTimeout = wTimeout;
        target.ptOUTData = ptData;
        target.ptINData = NULL;
        target.ptEngine = ptObj;
        
        return (telegraph_t *)ptTarget;
    } while(false);
    
    return NULL;
}

private void telegraph_timeout_event_handler(
    multiple_delay_report_status_t tStatus, void *pObj)
{
    class_internal(pObj, ptTarget, telegraph_t);
    if (NULL == ptTarget) {
        return ;
    }
    class_internal(target.ptEngine, ptThis, telegraph_engine_t);
    if (NULL == ptThis) {
        return ;
    }  
    
    __TE_ATOM_ACCESS (
        //! remove it from the listener queue
        LIST_QUEUE_REMOVE(this.Listener.ptHead, this.Listener.ptTail, ptTarget);
    )
    
    if (MULTIPLE_DELAY_TIMEOUT == tStatus) {
    
        do {
            target.ptINData = NULL;
            if (NULL != target.fnHandler) {
                //! call telegraph handler
                if (!(*target.fnHandler)(TELEGRAPH_TIMEOUT, (telegraph_t *)ptTarget)) {
                    //! do not free the telegraph
                    break;
                }
            }
            
            //! free telegraph
            pool_free(ref_obj_as(this, pool_t), ptTarget);
        } while(false);
    } 

}

private bool is_write_only_telegraph(telegraph_t *ptTelegraph) 
{
    class_internal(ptTelegraph, ptThis, telegraph_t);
    bool bResult = false;
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        bResult = (NULL == this.fnHandler);
    
    } while(false);
    
    return bResult;
}

private bool is_read_only_telegraph(telegraph_t *ptTelegraph) 
{
    class_internal(ptTelegraph, ptThis, telegraph_t);
    bool bResult = false;
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        bResult = (NULL == this.ptOUTData);
    
    } while(false);
    
    return bResult;
}

private void reset_input_block(telegraph_t *ptTelegraph) 
{
    class_internal(ptTelegraph, ptThis, telegraph_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        this.ptINData = NULL;
    
    } while(false);
}

private block_t *get_input_block(telegraph_t *ptTelegraph) 
{
    class_internal(ptTelegraph, ptThis, telegraph_t);
    block_t *ptBlock = NULL;
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        ptBlock = this.ptINData;
    
    } while(false);
    
    return ptBlock;
}

private void reset_output_block(telegraph_t *ptTelegraph) 
{
    class_internal(ptTelegraph, ptThis, telegraph_t);
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        this.ptOUTData = NULL;
    
    } while(false);
}

private block_t *get_output_block(telegraph_t *ptTelegraph) 
{
    class_internal(ptTelegraph, ptThis, telegraph_t);
    block_t *ptBlock = NULL;
    
    do {
        if (NULL == ptThis) {
            break;
        }
        
        ptBlock = this.ptOUTData;
    
    } while(false);
    
    return ptBlock;
}


private bool try_to_listen( telegraph_t *ptTelegraph )
{
    return try_to_send_telegraph(ptTelegraph, true);
}

private bool try_to_send_telegraph( telegraph_t *ptTelegraph,
                                    bool bPureListener)
{
    class(telegraph_engine_t) *ptThis = NULL;
    class_internal(ptTelegraph, ptTarget, telegraph_t);
    bool bResult = false; 
    
    do {
        if (NULL == ptTelegraph) {
            break;
        }  
        
        ptThis = (class(telegraph_engine_t) *)target.ptEngine;
        if (NULL == ptThis) {
            break;
        }
       
        if (bPureListener) {
            NOP();
            //! telegraph for pure listening 
            if (    (target.wTimeout > 0 && NULL == this.ptDelayService)        //!< no delay service available
                ||  (NULL == this.fnDecoder)                                    //!< no decoder
                ||  (NULL == target.fnHandler)) {                               //!< no listen callback available (pure sender)
                //! illegal parameters
                break;
            }
            
            //never do this! :target.ptOUTData = NULL;
            
            //! request timeout service
            if (target.wTimeout > 0) {
                target.ptDelayItem = 
                    MULTIPLE_DELAY.RequestDelay(    this.ptDelayService, 
                                                    target.wTimeout,
                                                    MULTIPLE_DELAY_HIGH_PRIORITY,
                                                    ptTelegraph,
                                                    &telegraph_timeout_event_handler);
                if (NULL == target.ptDelayItem) {
                    //! insufficient delay slots
                    break;
                }
            } else {
                target.ptDelayItem = NULL;
            }
            
            __TE_ATOM_ACCESS (
                //! add it to the listener queue
                LIST_QUEUE_ENQUEUE(
                    this.Listener.ptHead, this.Listener.ptTail, ptTelegraph);
            )
            
            bResult = true;
            break;
        } 
        //! normal telegraph
        
        if (NULL == this.fnWriteIO || NULL == target.ptOUTData) {
            break;
        }
        
        //! add telegraph to transmitter queue
        __TE_ATOM_ACCESS (
            //! add it to the listener queue
            LIST_QUEUE_ENQUEUE(
                this.Transmitter.ptHead, this.Transmitter.ptTail, ptTelegraph
            );
        )
        
        bResult = true;
    } while(false);
    
    if (    (!bResult) 
        &&  (NULL != ptThis) 
        &&  (NULL != ptTarget)) {
        
        do {
            //! raise telegraph received event
            if (NULL != target.fnHandler) {
                //! call telegraph handler
                if (!(*target.fnHandler)(TELEGRAPH_ERROR, (telegraph_t *)ptTarget)) {
                    //! do not free the telegraph
                    break;
                }
            
            }
            
            //! free telegraph
            pool_free(ref_obj_as(this, pool_t), ptTarget);
        } while(false);
    }
    
    return bResult;
}


private block_t * frontend(block_t *ptBlock, telegraph_engine_t *ptObj)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    telegraph_t *ptItem;
    
    do {
        if (NULL == ptThis || NULL == ptBlock) {
            break;
        } else if (0 == BLOCK.Size.Get(ptBlock)) {
            break;
        }
        
        if (NULL == this.Listener.ptHead || NULL == this.fnDecoder) {
            //! there is pending telegraph
            break;
        }
       
        __TE_ATOM_ACCESS (
            ptItem = this.Listener.ptHead;
        )
        do {
            class_internal(ptItem, ptTarget, telegraph_t);
            block_t *ptTempBlock = ptBlock;
            
            //! call frame parser
            frame_parsing_report_t tReport = this.fnDecoder(&ptTempBlock, ptItem);
            
            
            
            if (FRAME_UNMATCH != tReport) {
                if  (tReport == FRAME_RECEIVED) {
                
                    //! cancel delay service
                    if (    (NULL != target.ptDelayItem)
                        &&  (NULL != this.ptDelayService)) {
                        MULTIPLE_DELAY.Cancel(  this.ptDelayService, 
                                                target.ptDelayItem);

                        do {
                            //! raise telegraph received event
                            if (NULL != target.fnHandler) {
                                target.ptINData = ptBlock;
                                //! call telegraph handler
                                if (!(*target.fnHandler)(TELEGRAPH_RECEIVED, (telegraph_t *)ptTarget)) {
                                    //! do not free the telegraph
                                    break;
                                }
                            
                            }
                            __TE_ATOM_ACCESS (
                                __LIST_QUEUE_REMOVE(this.Listener.ptHead, 
                                                    this.Listener.ptTail, 
                                                    ptItem);
                            )
                            //! free telegraph
                            pool_free(ref_obj_as(this, pool_t), ptItem);
                            
                        } while(false);
                    }

                    if (NULL != ptTempBlock) {
                        ptBlock = ptTempBlock;
                    } else {
                        //! this should not happen
                        while(1);
                    }
                    
                    
                    
                    return ptBlock;
                    //! frame is received
                } /*else if (FRAME_UNKNOWN == tReport)  {
                    //! unknown frame detected
                    BLOCK.Size.Set(ptBlock, 0);
                } */
                break;
            }

            __TE_ATOM_ACCESS (
                ptItem = (telegraph_t *)target.ptNext;
            )
            
        } while(NULL != ptItem);
        
    } while(false);
    
    BLOCK.Size.Set(ptBlock, 0);
    
    return ptBlock;
}



#endif
/* EOF */
