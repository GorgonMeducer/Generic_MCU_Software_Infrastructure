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


#include <stdio.h>
#include "Device.h"                     // Keil::Board Support:V2M-MPS2:Common
#include "RTE_Components.h"             // Component selection
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_Touch.h"                // ::Board Support:Touchscreen
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.SAM4E-EK::Board Support:Graphic LCD

#include "app_platform.h"

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/

/*============================ TYPES =========================================*/





/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static volatile uint32_t s_wMSTicks = 0;   

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/





/*----------------------------------------------------------------------------
  SysTick / Timer0 IRQ Handler
 *----------------------------------------------------------------------------*/

void SysTick_Handler (void) 
{
    /* 1ms timer event handler */
    s_wMSTicks++;
    
    if (!(s_wMSTicks & (_BV(10) - 1))) {
        static volatile uint16_t wValue = 0;

        //printf("%s [%08x]\r\n", "Hello world!", wValue++);
        
        //STREAM_OUT.Stream.Flush();
    }
}



static void System_Init(void)
{
    SystemCoreClockUpdate();

    LED_Initialize();                       /* Initializ LEDs                 */
    Buttons_Initialize();                   /* Initializ Push Buttons         */
    
#ifdef RTE_Compiler_IO_STDOUT_User
    extern int stdout_init (void);

    stdout_init();                          /* Initializ Serial interface     */
#endif

    SysTick_Config(SystemCoreClock >> 10);  /* Generate interrupt roughly each 1 ms  */
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    System_Init();
    
    while (true) {
        uint8_t chByte;
        if (STREAM_IN.Stream.Read(&chByte)) {
            while(!STREAM_OUT.Stream.Write(chByte));
        } else {
            STREAM_OUT.Stream.Flush();
        }
    }
}

