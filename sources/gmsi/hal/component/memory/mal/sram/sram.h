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
#include "..\app_cfg.h"

#ifndef __MAL_SRAM_H__
#define __MAL_SRAM_H__

/*============================ INCLUDES ======================================*/

#if USE_COMPONENT_MAL_SRAM == ENABLED

/*============================ MACROS ========================================*/

#define MAL_SRAM_PAGE_BUFFER_SIZE           (512ul)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

//! sram mal interface
extern const i_mem_t I_SDF_SRAM;

//! sram mal static instance
//extern mem_t SRAM;
EXTERN_CLASS_OBJ( mem_t, SRAM );

#endif
#endif
/* EOF */
