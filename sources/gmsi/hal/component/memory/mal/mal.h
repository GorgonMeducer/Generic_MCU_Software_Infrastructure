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

#ifndef __MAL_H__
#define __MAL_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\interface.h"
#include ".\sdf_w25qxxx\sdf_w25qxxx.h"
#include ".\sram\sram.h"

/*============================ MACROS ========================================*/



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \note initialize memory abstract layer
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */  
extern bool mal_init( void );

/*! \brief read mal memory disregard page
 *! \param ptMEM memory object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
extern fsm_rt_t mal_mem_read( mem_t *ptMEM, 
     uint_fast16_t hwAddress, uint8_t *pchStream, uint_fast16_t hwSize );

/*! \brief write mal memory disregard page
 *! \param ptMEM memory object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
extern fsm_rt_t mal_mem_write( mem_t *ptMEM, 
     uint_fast16_t hwAddress, uint8_t *pchStream, uint_fast16_t hwSize );

#endif
/* EOF */
