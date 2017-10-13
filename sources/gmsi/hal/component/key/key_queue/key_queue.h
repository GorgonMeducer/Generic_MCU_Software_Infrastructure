/****************************************************************************
 *   Copyright(C)2013-2014 by TanekLiang<y574824080@gmail.com>              *
 *                                                                          *
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

#ifndef __KEY_QUEUE_H__
#define __KEY_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
DECLARE_CLASS(key_queue_t)
//! \name key queue interface
//! @{
EXTERN_CLASS(key_queue_t)
	key_t *ptBuffer;
	uint16_t hwSize;
	uint16_t hwHead;
	uint16_t hwTail;
	uint16_t hwLength;
END_EXTERN_CLASS(key_queue_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief init key queue
 *! \param ptQueue address of the target queue object
 *! \param ptKeyBuffer address of the key message buffer
 *! \param size key size
 *! \return true key queue initialization succeed
 *! \return false key queue initialization failed
 */
extern bool key_queue_init(key_queue_t * ptQueue, 
                           key_t *ptKeyBuffer, uint16_t hwsize);

/*! \brief enqueue 
 *! \param ptQueue address of the target queue object
 *! \param ptKeyBuffer address of the key message
 *! \return true key event enqueue succeed
 *! \return false key event enqueue failed
 */
extern bool key_enqueue(key_queue_t* ptQueue, key_t* ptKey);

/*! \brief dequeue 
 *! \param ptQueue address of the target queue object
 *! \param ptKeyBuffer address of the key message
 *! \return true key event dequeue succeed
 *! \return false key event dequeue failed
 */
extern bool key_dequeue(key_queue_t* ptQueue, key_t* ptKey);

#endif
/* EOF */