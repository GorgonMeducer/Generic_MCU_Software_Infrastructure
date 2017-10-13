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
#ifndef __TEMPLATE_USE_SERVICE_XMODEM_H__
#define __TEMPLATE_USE_SERVICE_XMODEM_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
#define fsm_rt_user_cancel              ((fsm_rt_t)-2)

/*============================ MACROFIED FUNCTIONS ===========================*/
#define XMODEM_CFG(__MODE, __BUFFER)        \
    do {                                    \
        xmodem_t tCFG = {                   \
            .pchBuffer = __BUFFER,          \
            .tMode = __MODE                 \
        };                                  \
        xmodem_init(&tCFG);                 \
    } while (0)

/*============================ TYPES =========================================*/
//! \name xmodem receive configuration
//! @{
typedef struct {
    uint8_t *pchBuffer;         //!< receive buffer
    enum {
        XMODEM_BLOCK_SZ_256  = 0, 
        XMODEM_BLOCK_SZ_1K,
    } tMode;
} xmodem_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief initialize xmodem receive service
 *! \param pchBuffer xmodem receive configuration
 *  \retval true xmodem receive service initialization succeeded
 *  \retval false xmodem receive service initialization failed
 */
extern bool xmodem_init(xmodem_t *ptCFG);

/*! \brief xmodem task
 *! \retrun fsm_rt_cpl          xmodem transfer finish
 *! \retrun fsm_rt_on_going     xmodem transfer on-going
 *! \retrun fsm_rt_on_err       xmodem transfer error
 *! \retrun fsm_rt_user_cancel  user cancel transfer
 */
extern fsm_rt_t xmodem_task(void);

#endif
