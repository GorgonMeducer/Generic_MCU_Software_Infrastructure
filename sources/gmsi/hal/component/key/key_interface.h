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

#ifndef __KEY_INTERFACE_H__
#define __KEY_INTERFACE_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
//! \brief No Button Pressed
#define KEY_NULL	                    0

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name key statues enum type
//! @{
typedef enum{
	KEY_DOWN = 0,                       //!< key press down
	KEY_UP,                             //!< key release
	KEY_PRESSED,                        //!< key pressed
	KEY_LONG_PRESSED,                   //!< key long pressed
	KEY_REPEAT,                         //!< key repeat 
}key_event_t;
//! @}

//! \name key event type
//! @{
typedef struct{
	uint8_t     chKeyValue;             //!< key value and the value 0 is reserve
	key_event_t tEvent;                 //!< key event type
}key_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */