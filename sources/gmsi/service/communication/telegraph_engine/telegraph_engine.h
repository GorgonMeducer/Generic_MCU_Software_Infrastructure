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

#ifndef __TELEGRAPH_ENGINE_H__
#define __TELEGRAPH_ENGINE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_SERVICE_TELEGRAPH_ENGINE == ENABLED
#include "..\..\memory\block\block.h"
#include "..\..\time\multiple_delay\multiple_delay.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define TELEGRAPH_ENGINE_CFG(__ADDR, ...)               \
    do {                                                \
        telegraph_engine_cfg_t tCFG = {                 \
            __VA_ARGS__                                 \
        };                                              \
                                                        \
        TELEGRAPH_ENGINE.Init((__ADDR), &tCFG);         \
    } while(false)

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
extern_class(telegraph_t)
    inherit(__single_list_node_t)    
    telegraph_engine_t      *ptEngine;
    telegraph_handler_t     *fnHandler;
    multiple_delay_item_t   *ptDelayItem;
    uint32_t                wTimeout;
    block_t                 *ptOUTData;
    block_t                 *ptINData;
end_extern_class(telegraph_t)
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


extern_simple_fsm(telegraph_engine_task,
    def_params(
        telegraph_t *ptCurrent;
    )
)



//! \name telegraph engine
//! @{
extern_class(telegraph_engine_t,   
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
end_extern_class(telegraph_engine_t, 
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

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_telegraph_engine_t TELEGRAPH_ENGINE;

/*============================ PROTOTYPES ====================================*/


#endif
#endif
/* EOF */
