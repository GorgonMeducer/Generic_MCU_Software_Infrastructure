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

#ifdef FRDM_K64F
#include "MK64F12.h"
//#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
#else
#include "Device.h"                     // Keil::Board Support:V2M-MPS2:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_Touch.h"                // ::Board Support:Touchscreen
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.SAM4E-EK::Board Support:Graphic LCD


#endif

#include "RTE_Components.h"             // Component selection
#include ".\stdout_USART.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
static void counter_overflow(void);

/*============================ IMPLEMENTATION ================================*/

/* \note please put it into a 1ms timer handler
 */
void app_platform_1ms_event_handler(void)
{
    counter_overflow();
    STREAM_IN_1ms_event_handler();
}


/*! \note initialize board specific package
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */  
bool app_platform_init( void )
{
    do {
        
#ifdef FRDM_K64F
        BOARD_InitPins();
        BOARD_BootClockRUN();
        
        SystemCoreClockUpdate();
#else
        SystemCoreClockUpdate();

        LED_Initialize();                       /* Initializ LEDs                 */
        Buttons_Initialize();                   /* Initializ Push Buttons         */
#endif

    #ifdef RTE_Compiler_IO_STDOUT_User
        if (!stdout_init()) {
            break;
        }
    #endif
        
        return true;
    } while(false);
    
    return false;
}
  
volatile static int32_t s_nCycleCounts = 0;

/*! \brief start performance counter (cycle accurate)
 */
void start_counter(void)
{
    SAFE_ATOM_CODE(
        s_nCycleCounts =  (int32_t)SysTick->VAL - (int32_t)SysTick->LOAD;
    )
}

static void counter_overflow(void)
{
    s_nCycleCounts += SysTick->LOAD;
}

/*! \brief stop performance counter (cycle accurate)
 *! \retval cycle elapsed. 
 */
int32_t stop_counter(void)
{
    SAFE_ATOM_CODE(
        s_nCycleCounts += (int32_t)SysTick->LOAD - (int32_t)SysTick->VAL;
    )
    return s_nCycleCounts;
}

/* EOF */
