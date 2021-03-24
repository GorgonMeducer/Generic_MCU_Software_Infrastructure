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


#include <stdio.h>
#include "app_platform.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
static void counter_overflow(void);
extern void SystemCoreClockUpdate (void);
/*============================ IMPLEMENTATION ================================*/

 #if __IS_COMPILER_ARM_COMPILER_6__
__asm(".global __use_no_semihosting\n\t");
__asm(".global __ARM_use_no_argv\n\t");

void _sys_exit(int ch)
{
    while(1);
}


void _ttywrch(int ch)
{
    
}

#include <rt_sys.h>

FILEHANDLE $Sub$$_sys_open(const char *name, int openmode)
{
    return 0;
}

#endif


/* \note please put it into a 1ms timer handler
 */
void app_platform_1ms_event_handler(void)
{
    counter_overflow();
    STREAM_IN_1ms_event_handler();
}

__attribute__((constructor(101)))
/*! \note initialize board specific package
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */  
void app_platform_init( void )
{
    do {
        SystemCoreClockUpdate();

        gmsi_platform_init();
    

        if (!stdout_init()) {
            break;
        }
        
        return ;
    } while(false);
    
    NVIC_SystemReset();
}
  
volatile static int32_t s_nCycleCounts = 0;

/*! \brief start performance counter (cycle accurate)
 */
void start_counter(void)
{
    __IRQ_SAFE {
        s_nCycleCounts =  (int32_t)SysTick->VAL - (int32_t)SysTick->LOAD;
    }
}

static ALWAYS_INLINE void counter_overflow(void)
{
    s_nCycleCounts += SysTick->LOAD;
}

/*! \brief stop performance counter (cycle accurate)
 *! \retval cycle elapsed. 
 */
int32_t stop_counter(void)
{
    __IRQ_SAFE {
        s_nCycleCounts += (int32_t)SysTick->LOAD - (int32_t)SysTick->VAL;
    }
    return s_nCycleCounts;
}

/* EOF */
