/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher for MPS2
 * Note(s): possible defines set in "options for target - C/C++ - Define"
 *            __USE_LCD    - enable Output on GLCD
 *            __USE_TIMER0 - use Timer0  to generate timer interrupt
 *                         - use SysTick to generate timer interrupt (default)
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "Device.h"                     // Keil::Board Support:V2M-MPS2:Common
#include "RTE_Components.h"             // Component selection
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_Touch.h"                // ::Board Support:Touchscreen
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.SAM4E-EK::Board Support:Graphic LCD

#include "..\sources\gmsi\gmsi.h"       // Import GMSI Support


static volatile uint32_t s_wMSTicks = 0;   


/*----------------------------------------------------------------------------
  SysTick / Timer0 IRQ Handler
 *----------------------------------------------------------------------------*/

void SysTick_Handler (void) 
{
    /* 1ms timer event handler */
    s_wMSTicks++;
    
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

        if (!(s_wMSTicks & (_BV(10) - 1))) {
            static volatile uint16_t wValue = 0;

            printf("%s [%08x]\r\n", "Hello World", wValue++);
        }
    }
}
