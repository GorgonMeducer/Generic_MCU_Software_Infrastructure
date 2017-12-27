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

#ifndef _USE_MAL_SPI_FLASH_H_
#define _USE_MAL_SPI_FLASH_H_

/*============================ INCLUDES ======================================*/

#if USE_COMPONENT_MAL_SDF_W25QXXXX == ENABLED
/*============================ MACROS ========================================*/
         
#define MAL_SDF_W25QXXX_PAGE_BUFFER_SIZE    (4096ul)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief initialize serial data flash module
 *! 
 *! \param none
 *! 
 *! \retval fsm_rt_cpl initialize succeed.
 *! \retval fsm_rt_err initialize failed
 */
extern fsm_rt_t sdf_init(void);

/*! \brief read unique ID
 *! 
 *! \param pdwUID unique ID
 *! 
 *! \retval fsm_rt_err access failed or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY serial data flash is not ready
 *! \retval fsm_rt_on_going current access is on going.
 */
extern fsm_rt_t fsm_sdf_read_unique_ID(uint64_t *pdwUID);


/*! \brief read device ID
 *! 
 *! \param pdwUID device ID
 *! 
 *! \retval fsm_rt_err access failed or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY serial data flash is not ready
 *! \retval fsm_rt_on_going current access is on going.
 */
extern fsm_rt_t fsm_sdf_read_device_ID(uint16_t *phwDID);

/*============================ GLOBAL VARIABLES ==============================*/
//! SPI flash interface
extern const i_mem_t ISDF;

//! SPI flash static instance
EXTERN_CLASS_OBJ(mem_t,SDF);

#endif
#endif
/* EOF */
