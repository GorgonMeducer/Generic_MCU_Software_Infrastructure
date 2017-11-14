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
#include "..\..\time\multiple_delay\multiple_delay.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(telegraph_t)

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
    uint32_t wTimeout;
    telegraph_handler_t *fnHandler;
end_def_class(telegraph_t)
//! @}

//! \name telegraph engine
//! @{
declare_class(telegraph_engine_t)
def_class(telegraph_engine_t)

    telegraph_t *ptHead;
    telegraph_t *ptTail;

end_def_class(telegraph_engine_t)
//! @}


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


/* EOF */
