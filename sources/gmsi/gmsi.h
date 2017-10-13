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

#ifndef __ES_FRAMEWORK_H__
#define __ES_FRAMEWORK_H__


/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\hal\hal.h"
#include ".\service\service.h"

/*============================ MACROS ========================================*/

//! \name GSF Interface Type
//! @{
//! general purpose interface for normal MCU applicatoin
#define GENERAL_PURPOSE                 0           
//! @}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

//! \name gsf version info
//! @{
extern const struct {
    uint8_t chPurpose;          //!< software framework purpose
    uint8_t chInterface;        //!< interface version
    uint8_t chMajor;            //!< major version
    uint8_t chMinor;            //!< minor version
} GSFVersion;
//! @}

/*============================ PROTOTYPES ====================================*/


/*! \note initialize platform
 *  \param none
 *  \retval true platform initialization succeeded.
 *  \retval false platform initialization failed
 */
extern bool platform_init( void );

#endif