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

#ifndef __APP_CFG__
#define __APP_CFG__

/*============================ INCLUDES ======================================*/
//! \brief import head files

#ifdef __USER_DEFINED_APP_CFG_H__
#include "..\example\app_cfg.h"
#else

/*============================ MACROS ========================================*/


/*----------------------------------------------------------------------------*
 * Hardware Abstruct Layer Configuration                                      *
 *----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*
 * Scheduler Configuration                                                    *
 *----------------------------------------------------------------------------*/
//! \name configure safe task service:1.2K for current configuration
//! @{
#define TASK_SCHEDULER                      ENABLED
#define SAFE_TASK_THREAD_SYNC               ENABLED     //!< disable semaphore support
#define SAFE_TASK_CRITICAL_SECTION          ENABLED     //!< enable critical support
#define SAFE_TASK_CALL_STACK                ENABLED
#define SAFE_TASK_USE_RESERVED_SYSTEM_RAM   DISABLED


#define SAFE_TASK_QUEUE_POOL_SIZE           (1ul)       //!< task queue pool size
#define SAFE_TASK_POOL_SIZE                 (8ul)       //!< task pool size       
//! @}


/*----------------------------------------------------------------------------*
 * Component Configuration                                                    *
 *----------------------------------------------------------------------------*/

#define USE_MAL_PAGE_FREE_ACCESS            ENABLED
#define USE_COMPONENT_MAL_SDF_W25QXXXX      DISABLED
#define USE_COMPONENT_MAL_SRAM              ENABLED
#define USE_COMPONENT_SW_SDIO               DISABLED


#define USE_COMPONENT_BOOTLOADER            DISABLED

/*----------------------------------------------------------------------------*
 * Service Configuration                                                      *
 *----------------------------------------------------------------------------*/
#define USE_SERVICE_ES_SIMPLE_FRAME         ENABLED
#define USE_SERVICE_XMODEM                  DISABLED
#define USE_SERVICE_MULTIPLE_DELAY          ENABLED
#define USE_SERVICE_TELEGRAPH_ENGINE        ENABLED

#define USE_SERVICE_BLOCK                   ENABLED
#define USE_SERVICE_BLOCK_QUEUE             ENABLED
#define USE_SERVICE_STREAM_TO_BLOCK         ENABLED


#define USE_SERVICE_GUI_TGUI                DISABLED
#if USE_SERVICE_GUI_TGUI == ENABLED
#   define TGUI_SIZE_INT_TYPE               TGUI_MEDIUM
#   define TGUI_COLOR_BITS                  TGUI_4BITS
#endif




#define CRC7_OPTIMIZE                       CRC_OPT_SPEED
#define CRC8_OPTIMIZE                       CRC_OPT_BALANCE
#define CRC8_ROHC_OPTIMIZE                  CRC_OPT_BALANCE
#define CRC16_MODBUS_OPTIMIZE               CRC_OPT_BALANCE
#define CRC16_USB_OPTIMIZE                  CRC_OPT_BALANCE
#define CRC16_CCITT_FALSE_OPTIMIZE          CRC_OPT_BALANCE
#define CRC32_IEEE802_3_OPTIMIZE            CRC_OPT_BALANCE
#define CRC32_OPTIMIZE                      CRC_OPT_BALANCE
//! @}

#endif

//! \name CRC configuration
//! @{
#define CRC_OPT_SIZE        2
#define CRC_OPT_BALANCE     1
#define CRC_OPT_SPEED       0


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ INCLUDES ======================================*/



#endif

