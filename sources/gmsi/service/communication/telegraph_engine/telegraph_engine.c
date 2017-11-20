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
typedef fsm_rt_t telegraph_handler_t (telegraph_report_t tStatus, telegraph_t *ptTelegraph);

//! \name abstruct class telegraph, user telegraph should inherit from this class
//! @{
def_class(telegraph_t)
    inherit(__single_list_node_t)    
    telegraph_engine_t      *ptEngine;
    telegraph_handler_t     *fnHandler;
    multiple_delay_item_t   *ptDelayItem;
    uint32_t                wTimeout;
    block_t                 *ptData;
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

typedef fsm_rt_t telegraph_engine_low_level_write_io_t(block_t *ptBlock, void *pObj);

//! \name telegraph engine
//! @{

def_class(telegraph_engine_t)

    telegraph_t                             *ptHead;
    telegraph_t                             *ptTail;
    telegraph_parser_t                      *fnDecoder;
    multiple_delay_t                        *ptDelayService;
    telegraph_engine_low_level_write_io_t   *fnWriteIO;
                            
end_def_class(telegraph_engine_t)
//! @}

typedef struct {
    telegraph_parser_t                      *fnDecoder;
    multiple_delay_t                        *ptDelayService;
    telegraph_engine_low_level_write_io_t   *fnWriteIO;
} telegraph_engine_cfg_t;


def_interface(i_telegraph_engine_t)
    bool        (*Init)         (   telegraph_engine_t *ptObj, 
                                    telegraph_engine_cfg_t *ptCFG);
                                    
    struct {
        block_t *   (*Parse)    (   block_t *ptBlock, telegraph_engine_t *ptObj);
    } Dependent;
    
    struct {
        bool        (*TryToSend)(   telegraph_engine_t *ptObj, 
                                    telegraph_t *ptTelegraph,
                                    bool bPureListener);
        bool        (*Listen)   (   telegraph_engine_t *ptObj, 
                                    telegraph_t *ptTelegraph);
    } Telegraph;
end_def_interface(i_telegraph_engine_t)


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static bool init(telegraph_engine_t *ptObj, telegraph_engine_cfg_t *ptCFG);
static block_t * frontend(block_t *ptBlock, telegraph_engine_t *ptObj);
static bool try_to_send_telegraph(  telegraph_engine_t *ptObj, 
                                    telegraph_t *ptTelegraph,
                                    bool bListener);
static bool try_to_listen(  telegraph_engine_t *ptObj, 
                            telegraph_t *ptTelegraph);
                                    
/*============================ GLOBAL VARIABLES ==============================*/

const i_telegraph_engine_t TELEGRAPH_ENGINE = {
    .Init =             &init,
    .Dependent = {
        .Parse =            &frontend,
    },
    .Telegraph = {
        .TryToSend =    &try_to_send_telegraph,
        .Listen =       &try_to_listen,
    },
};

/*============================ IMPLEMENTATION ================================*/

static bool init(telegraph_engine_t *ptObj, telegraph_engine_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    do {
        if (NULL == ptThis || NULL == ptCFG) {
            break;
        } else if (NULL == ptCFG->fnDecoder) {
            break;
        }
        
        memset((void *)ptObj, 0, sizeof(telegraph_engine_t));
        

        this.fnDecoder = ptCFG->fnDecoder;
        this.ptDelayService = ptCFG->ptDelayService;
        this.fnWriteIO = ptCFG->fnWriteIO;
        
        return true;
    } while(false);

    return false;
}

static void telegraph_timeout_event_handler(
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
    
    
    if (MULTIPLE_DELAY_TIMEOUT == tStatus) {
        target.ptData = NULL;
        if (NULL != target.fnHandler) {
            //! call telegraph handler
            (*target.fnHandler)(TELEGRAPH_TIMEOUT, (telegraph_t *)ptTarget);
        
        }
    } 
    
    __TE_ATOM_ACCESS (
        //! remove it from the listener queue
        LIST_QUEUE_DEQUEUE(this.ptHead, this.ptTail, ptTarget);
    )

}

static bool try_to_listen(  telegraph_engine_t *ptObj, 
                            telegraph_t *ptTelegraph)
{
    return try_to_send_telegraph(ptObj, ptTelegraph, true);
}

static bool try_to_send_telegraph(  telegraph_engine_t *ptObj, 
                                    telegraph_t *ptTelegraph,
                                    bool bPureListener)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    class_internal(ptTelegraph, ptTarget, telegraph_t);
    bool bResult = false; 
    
    do {
        if (NULL == ptThis || NULL == ptTelegraph) {
            break;
        }  
       
        target.ptEngine = ptObj;
       
        if (bPureListener) {
            //! telegraph for pure listening 
            if (    (target.wTimeout > 0 && NULL == this.ptDelayService)        //!< no delay service available
                ||  (NULL == this.fnDecoder)                                    //!< no decoder
                ||  (NULL == target.fnHandler)) {                               //!< no listen callback available (pure sender)
                //! illegal parameters
                break;
            }
            
            //! request timeout service
            if (target.wTimeout > 0) {
                target.ptDelayItem = 
                    MULTIPLE_DELAY.RequestDelay(    this.ptDelayService, 
                                                    target.wTimeout,
                                                    MULTIPLE_DELAY_LOW_PRIORITY,
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
                LIST_QUEUE_ENQUEUE(this.ptHead, this.ptTail, ptTelegraph);
            )
            
            bResult = true;
            break;
        } 
        //! normal telegraph
        
        if (NULL == this.fnWriteIO) {
            break;
        }
        
        //! todo add telegraph to transmission queue
        
        
    } while(false);
    
    return bResult;
}




static block_t * frontend(block_t *ptBlock, telegraph_engine_t *ptObj)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    telegraph_t *ptItem;
    
    do {
        if (NULL == ptThis || NULL == ptBlock) {
            break;
        } else if (0 == BLOCK.Size.Get(ptBlock)) {
            break;
        }
        
        if (NULL == this.ptHead || NULL == this.fnDecoder) {
            //! there is pending telegraph
            break;
        }
       
        __TE_ATOM_ACCESS (
            ptItem = this.ptHead;
        )
        do {
            class_internal(ptItem, ptTarget, telegraph_t);
            block_t *ptTempBlock = ptBlock;
            
            //! call frame parser
            frame_parsing_report_t tReport = this.fnDecoder(&ptTempBlock, ptItem);
            
            if (NULL != ptTempBlock) {
                ptBlock = ptTempBlock;
            }
            
            if (FRAME_UNMATCH != tReport) {
                if  (tReport == FRAME_RECEIVED) {
                
                    //! cancel delay service
                    if (    (NULL != target.ptDelayItem)
                        &&  (NULL != this.ptDelayService)) {
                        MULTIPLE_DELAY.Cancel(  this.ptDelayService, 
                                                target.ptDelayItem);

                        //! raise telegraph received event
                        if (NULL != target.fnHandler) {
                            target.ptData = ptBlock;
                            //! call telegraph handler
                            (*target.fnHandler)(TELEGRAPH_RECEIVED, (telegraph_t *)ptTarget);
                        
                        }
                                                
                    }

                    //! frame is received
                } else if (FRAME_UNKNOWN == tReport)  {
                    //! unknown frame detected
                    
                }
                break;
            }
            
            __TE_ATOM_ACCESS (
                ptItem = (telegraph_t *)target.ptNext;
            )
            
        } while(NULL != ptItem);
        
    } while(false);
    
    return ptBlock;
}



#endif
/* EOF */
