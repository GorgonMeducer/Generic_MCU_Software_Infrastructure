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

#ifndef __USE_APP_PLATFORM_H__
#define __USE_APP_PLATFORM_H__

/*============================ INCLUDES ======================================*/
#include "..\sources\gmsi\gmsi.h"       // Import GMSI Support

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define LOG(__STR)                                                              \
        do {                                                                    \
            static char chBuffer[] = {__STR};                                   \
            transfer_string(chBuffer, sizeof(chBuffer));                        \
        } while(false)

/*============================ TYPES =========================================*/

EXTERN_OUTPUT_STREAM_BUFFER(STREAM_OUT, 128)
                      
END_EXTERN_OUTPUT_STREAM_BUFFER(STREAM_OUT)

EXTERN_INPUT_STREAM_BUFFER(STREAM_IN, 8)
                      
END_EXTERN_INPUT_STREAM_BUFFER(STREAM_OUT)        
        
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void transfer_string(const char *pchStr, uint_fast16_t hwSize);




#endif
