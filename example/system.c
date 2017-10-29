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

#ifndef FRAME_BUFFER_SIZE
#   warning No defined FRAME_BUFFER_SIZE, default value 512 is used
#define FRAME_BUFFER_SIZE               (512)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static volatile uint32_t s_wMSTicks = 0;   
NO_INIT static es_simple_frame_t s_tFrame;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------
  SysTick / Timer0 IRQ Handler
 *----------------------------------------------------------------------------*/

void SysTick_Handler (void) 
{
    /*! 1ms timer event handler */
    s_wMSTicks++;
    
    if (!(s_wMSTicks % 1000)) {
        static volatile uint16_t wValue = 0;

        //printf("%s [%08x]\r\n", "Hello world!", wValue++);
        
        //STREAM_OUT.Stream.Flush();
    }
    
    /*! call application platform 1ms event handler */
    app_platform_1ms_event_handler();
}



static void system_init(void)
{
    app_platform_init();

    SysTick_Config(SystemCoreClock  / 1000);  //!< Generate interrupt every 1 ms 
}

static uint_fast16_t frame_parser(mem_block_t tMemory, uint_fast16_t hwSize)
{
    return hwSize;
}

static void app_init(void)
{
    
    NO_INIT static uint8_t s_chFrameBuffer[FRAME_BUFFER_SIZE];
    NO_INIT static i_byte_pipe_t s_tPipe;
    s_tPipe.ReadByte = (STREAM_IN.Stream.Read);
    s_tPipe.WriteByte = (STREAM_OUT.Stream.Write);

    //! initialise simple frame service
    ES_SIMPLE_FRAME_CFG(&s_tFrame, 
                        &s_tPipe,
                        &frame_parser,
                        s_chFrameBuffer,
                        sizeof(s_chFrameBuffer));

}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
    system_init();
    app_init();
    
    while (true) {
    #if false
        if (fsm_rt_cpl == ES_SIMPLE_FRAME.Task(&s_tFrame)) {
            STREAM_OUT.Stream.Flush();
        }
    #else
        uint8_t chByte;
        if (STREAM_IN.Stream.Read(&chByte)) {
            STREAM_OUT.Stream.Write(chByte);
        } else {
            STREAM_OUT.Stream.Flush();
        }
    #endif
    }
}

