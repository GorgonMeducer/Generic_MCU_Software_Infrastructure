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
#include "./app_cfg.h"
#include "./arch/arch.h"
#include "./hal/hal.h"
#include "./service/service.h"

/*============================ MACROS ========================================*/

//! \name GMSI Interface Type
//! @{
//! general purpose interface for normal MCU applicatoin
#define GENERAL_PURPOSE                 0           
//! @}

#define GMSI_PURPOSE                     GENERAL_PURPOSE

/*! \brief interface version
 *! \note Change this when new generation of interface is released
 */
#define GMSI_INTERFACE_VERSION           1

/*! \brief major version for specified gsf interface
 *! \note major version is rarely changed
 */
#define GMSI_MAJOR_VERSION               1

/*! \brief minor version for normal maintaince
 *! \note update this for function update and bug fixing
 */
#define GMSI_MINOR_VERSION               0

/*! \brief GSF version 
 *         (InterfaceType, InterfaceVersion, MajorVersion, MinorVersion)
 */
#define GMSI_VERSION                 {                                      \
                                        GMSI_PURPOSE,                       \
                                        GMSI_INTERFACE_VERSION,             \
                                        GMSI_MAJOR_VERSION,                 \
                                        GMSI_MINOR_VERSION,                 \
                                    }

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

//! \name gmsi version info
//! @{
const struct {
    uint8_t chPurpose;          //!< software framework purpose
    uint8_t chInterface;        //!< interface version
    uint8_t chMajor;            //!< major version
    uint8_t chMinor;            //!< minor version
} GMSIVersion = GMSI_VERSION;
//! @}

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \note initialize platform
 *  \param none
 *  \retval true platform initialization succeeded.
 *  \retval false platform initialization failed
 */
bool gmsi_platform_init( void )
{
    bool bResult = false;

    do {
        /* initialise architecture */
        if (!arch_init()) {
            break;
        }
        
        /*! initialize hardware abstract layer */
        if ( !hal_init() ) {
            break;
        }

        /*! initialize system services */
        if ( !service_init() )  {
            break;
        }

        bResult = true;
    } while(false);

    #ifdef AFETER_SYSTEM_INIT
        AFETER_SYSTEM_INIT
    #endif


    return bResult;
}


/* EOF */
