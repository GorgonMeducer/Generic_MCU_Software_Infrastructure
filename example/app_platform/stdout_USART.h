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

#define LOG(__STR)                                                              \
        do {                                                                    \
            static char chBuffer[] = {__STR};                                   \
            transfer_string(chBuffer, sizeof(chBuffer));                        \
        } while(false)

/*============================ TYPES =========================================*/

EXTERN_OUTPUT_STREAM_BUFFER(STREAM_OUT, OUTPUT_STREAM_BLOCK_SIZE)
                      
END_EXTERN_OUTPUT_STREAM_BUFFER(STREAM_OUT)

EXTERN_INPUT_STREAM_BUFFER(STREAM_IN, INPUT_STREAM_BLOCK_SIZE)
                      
END_EXTERN_INPUT_STREAM_BUFFER(STREAM_OUT)        
        
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
        
/* \note please put it into a 1ms timer handler
 */
extern void stream_in_1ms_event_handler(void);        
        
/*! \note initialize usart for stdout
 *  \param none
 *  \retval true    initialization succeeded.
 *  \retval false   initialization failed
 */  
extern bool stdout_init (void);

/*! \note transfer spcified stream via uart
 *! \param chStr  the start address of the target memory
 *! \param hwSize the size of the target memory
 *! \return none
 */
extern void transfer_string(const char *pchStr, uint_fast16_t hwSize);

/*! \note send one char directly to usart
 *  \param chByte target byte
 *  \retval true  access succeed 
 *  \retval false access failed
 */ 
extern bool serial_out(uint8_t chByte);

/*! \note read one char directly from usart
 *  \param pchByte the address of a byte buffer
 *  \retval true  access succeed 
 *  \retval false access failed
 */
extern bool serial_in(uint8_t *pchByte);


#endif
