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
#include ".\app_platform\app_platform.h"

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
    
    if (!(s_wMSTicks % 1000)) {
        static volatile uint16_t wValue = 0;

        //printf("%s [%08x]\r\n", "Hello world!", wValue++);
        
        //STREAM_OUT.Stream.Flush();
    }
    
    /* call application platform 1ms event handler */
    app_platform_1ms_event_handler();
}



static void System_Init(void)
{
    app_platform_init();
    

    SysTick_Config(SystemCoreClock  / 1000);  /* Generate interrupt each 1 ms  */
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    System_Init();
    
    while (true) {
        
        //! software loopback 
        uint8_t chByte;
        if (STREAM_IN.Stream.Read(&chByte)) {
            while(!STREAM_OUT.Stream.Write(chByte));
        } else {
            STREAM_OUT.Stream.Flush();
        }
    }
}

