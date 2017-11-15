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
#include "..\..\memory\epool\epool.h"
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
    telegraph_engine_t *ptEngine;
    telegraph_handler_t *fnHandler;
end_def_class(telegraph_t)
//! @}

typedef enum {
    FRAME_UNKNOWN       = -1,
    FRAME_UNMATCH       = 0,
    FRAME_RECEIVED      = 1,
} frame_parsing_report_t;

typedef frame_parsing_report_t telegraph_parser_t(
                                                    mem_block_t tMem,           //! memory buffer
                                                    uint_fast16_t hwSize,       //! size of received data
                                                    telegraph_t *ptItem);       //! target telegraph 
            

//! \name telegraph engine
//! @{

def_class(telegraph_engine_t)

    telegraph_t            *ptHead;
    telegraph_t            *ptTail;
    locker_t                tLocker;
    telegraph_parser_t     *fnParser;
    
end_def_class(telegraph_engine_t)
//! @}


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool init(telegraph_engine_t *ptObj)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    do {
        if (NULL == ptObj) {
            break;
        }
        
        memset((void *)ptObj, 0, sizeof(telegraph_engine_t));
        
        
        return true;
    } while(false);

    return false;
}

static uint_fast16_t frontend(  telegraph_engine_t *ptObj, 
                                mem_block_t tMem, uint_fast16_t hwSize)
{
    class_internal(ptObj, ptThis, telegraph_engine_t);
    telegraph_t *ptItem;
    uint_fast16_t tReturn = 0;
    
    do {
        if (NULL == ptThis || NULL == tMem.pchBuffer || 0 == tMem.hwSize) {
            break;
        }
        
        if (NULL == this.ptHead || NULL == this.fnParser) {
            //! there is pending telegraph
            break;
        }
        
        /*! \note as frontend is always running in super-loop, enter_lock should 
         *        always return true. If RTOS is used, the enter_lock should be 
         *        re-written to support RTOS. In this case, the enter_lock might
         *        block current task and yield to other tasks
         */
        while(!enter_lock(&this.tLocker));
        
        ptItem = this.ptHead;
        do {
            class_internal(ptItem, ptTarget, telegraph_t);
            
            //! call frame parser
            frame_parsing_report_t tReport = this.fnParser(tMem, hwSize, ptItem);
            if (FRAME_UNMATCH != tReport) {
                if  (tReport >= FRAME_RECEIVED) {
                    //! frame is received
                    tReturn = (uint_fast16_t)tReport - 1;
                    
                } else /*if (FRAME_UNKNOWN == tReport) */ {
                    //! unknown frame detected
                    tMem.pchSrc[0] = TELEGRAPH_ENGINE_FRAME_ERROR;
                    tReturn = 1;
                }
                break;
            }
            
            ptItem = (telegraph_t *)target.ptNext;
            
        } while(NULL != ptItem);
        
        leave_lock(&this.tLocker);
    } while(false);
    
    return tReturn;
}



#endif
/* EOF */
