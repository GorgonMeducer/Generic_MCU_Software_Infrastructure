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


//! \note do not move this pre-processor statement to other places

#ifndef __SYSTEM_APP_CFG_H__
#define __SYSTEM_APP_CFG_H__


/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

/*----------------------------------------------------------------------------*
 * Application Platform Configuration                                         *
 *----------------------------------------------------------------------------*/

//  <h> STDOUT (via UART) configuration
//      <o>SerialPort Baudrate
//      <i>Configure the baudrate of the UART which is used as stdout (printf)
#define USART_BAUDRATE                      (115200)

//      <o>Block Input Timeout in ms <0-65535>
//      <i>When timeout, all received bytes will be imported to input stream (STREAM_IN), 0 means disabling the timeout feature.
#define STREAM_IN_RCV_TIMEOUT               (5)

//      <h> Output Stream (STREAM_OUT)
//          <o>The size of output buffer block <8-4096> 
//          <i>Output stream will be transfered in blocks, the size of an output block is defined here.
#define OUTPUT_STREAM_BLOCK_SIZE            (32)
//          <o>The number of output blocks in a dedicated heap <2-65535>
//          <i>All output blocks are allocated from a dedicated heap, the size of the heap is defined here.
#define OUTPUT_STREAM_BLOCK_COUNT           (4)
//      </h>
//      <h> Input Stream (STREAM_IN)
//          <o>The size of input buffer block <8-4096> 
//          <i>Input stream will be received in blocks, the size of an input block is defined here.
#define INPUT_STREAM_BLOCK_SIZE             (16)  
//          <o>The number of input blocks in a dedicated heap <2-65535>
//          <i>All input blocks are allocated from a dedicated heap, the size of the heap is defined here.
#define INPUT_STREAM_BLOCK_COUNT            (8)
//      </h>
//  </h>

//  <h> Simple Frame Encoding / Decodign configuration
//      <o>Frame Buffer Size in byte <1-65536>
//      <i>The frame buffer is only used to store the data decoded from a valid frame. Reply data can also be stored in this buffer.
#define FRAME_BUFFER_SIZE                   (1024)
//  </h>

//  <h> Multiple Delay Service Configuration
//      <o>Maximum number of delay objects supported <1-65536>
//      <i>A dedicated pool is provided to hold all the delay objects. The pool size determined the maximum allowed delay tasks supported at the same time.
#define DELAY_OBJ_POOL_SIZE                 8
//  </h>



#define USE_SERVICE_ES_SIMPLE_FRAME         ENABLED



#define USE_SERVICE_MULTIPLE_DELAY          ENABLED
#define USE_SERVICE_TELEGRAPH_ENGINE        ENABLED
#define USE_SERVICE_BLOCK                   ENABLED
#define USE_SERVICE_BLOCK_QUEUE             ENABLED
#define USE_SERVICE_STREAM_TO_BLOCK         ENABLED


#define DEMO_MULTIPLE_DELAY                 DISABLED
#define DEMO_FRAME_USE_BLOCK_MODE           ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
