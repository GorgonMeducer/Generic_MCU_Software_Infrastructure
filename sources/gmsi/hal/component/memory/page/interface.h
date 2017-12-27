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

#ifndef __MEMORY_PAGE_INTERFACE_H__
#define __MEMORY_PAGE_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\interface.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*! \brief prototype for page access
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \return state machine state
 */
typedef fsm_rt_t page_access_t( 
    void *pObj, uint32_t wAddress, uint8_t *pchStream);

//! \name page interface
//! @{
DEF_INTERFACE(i_page_t)
    page_access_t *fnPageWrite;                                 //!< write page
    page_access_t *fnPageRead;                                  //!< read page
    fsm_rt_t   (*fnPageErase)(void *pObj, uint32_t wAddress);  //!< page erase
END_DEF_INTERFACE(i_page_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
