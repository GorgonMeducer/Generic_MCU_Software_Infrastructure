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

#ifndef __COMPONENT_BOOTLOADER_H__
#define __COMPONENT_BOOTLOADER_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_COMPONENT_BOOTLOADER == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \brief bootloader command handler
typedef uint_fast16_t bl_cmd_handler_t(uint8_t *pchStream, uint_fast16_t hwSize);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief bootloader command parser
 *! \param pchStream data block
 *! \param hwSize data block size
 *! \retval 0 failed in parsing
 *! \retval none-zero-size reply data size
 */
extern uint_fast16_t booloader_command_parser(uint8_t *pchStream, uint_fast16_t hwSize);

#endif
#endif
/* EOF */
