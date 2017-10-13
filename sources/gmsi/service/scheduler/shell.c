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
#include ".\scheduler.h"

#if TASK_SCHEDULER == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/



WEAK bool ON_ENTERING_IDLE_TASK(void)
{
    return true;
}

WEAK void TRY_TO_SLEEP(void)
{
}

WEAK void ON_LEAVING_IDLE_TASK(void)
{
}

/*! \note system idle task
*! \param none
*! \return none
*/
static void idle_task( void )
{
    if (ON_ENTERING_IDLE_TASK()) {
        //! try to enter a max allowed sleep mode
        TRY_TO_SLEEP();
    }

    ON_LEAVING_IDLE_TASK();
}

ROOT void OS_SCHEDULER(void)
{
    if (!CS_SCHEDULER()) {          //!< scheduler
        idle_task();                //!< idle state
    }
}

ROOT void SCHEDULER_INIT(void)
{
    //! initialize scheduler
    CS_INIT();
    static NO_INIT task_t s_tTaskPool[SAFE_TASK_POOL_SIZE];
    CS_HEAP_INIT(s_tTaskPool, sizeof(s_tTaskPool));
}
#endif

