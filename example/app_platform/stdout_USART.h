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

#ifndef __USE_STDOUT_USART_H__
#define __USE_STDOUT_USART_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"


/*============================ MACROS ========================================*/

#ifndef OUTPUT_STREAM_BLOCK_SIZE
#   define OUTPUT_STREAM_BLOCK_SIZE         (32)
#endif

#ifndef INPUT_STREAM_BLOCK_SIZE
#   define INPUT_STREAM_BLOCK_SIZE          (32)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \note define a buffer for output stream
//! @{
EXTERN_OUTPUT_STREAM_BUFFER(            STREAM_OUT, OUTPUT_STREAM_BLOCK_SIZE)
                      
END_EXTERN_OUTPUT_STREAM_BUFFER(        STREAM_OUT)

//! \note add an adapter for serial port
EXTERN_STREAM_OUT_SERIAL_PORT_ADAPTER(  STREAM_OUT)
//! @}

//! \note define a buffer for input stream
//! @{
EXTERN_INPUT_STREAM_BUFFER(             STREAM_IN, INPUT_STREAM_BLOCK_SIZE)
                      
END_EXTERN_INPUT_STREAM_BUFFER(         STREAM_OUT)        
//! \note add an adapter for serial port
EXTERN_STREAM_IN_SERIAL_PORT_ADAPTER(   STREAM_IN)
//! @}
        
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/    
        
/*! \note initialize usart for stdout
 *  \param none
 *  \retval true    initialization succeeded.
 *  \retval false   initialization failed
 */  
extern bool stdout_init (void);

#endif
