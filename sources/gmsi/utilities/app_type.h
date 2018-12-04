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

#ifndef __APP_TYPE_H_INCLUDED__
#define __APP_TYPE_H_INCLUDED__

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*============================ MACROS ========================================*/
 //! \name boolean value
//! @{
#ifndef false
    # define false          0x00
#endif

#ifdef true
    #undef true         
#endif
#define true                (!false)
//! @}  
     
#ifndef NULL
#   define NULL             ((void*)0)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define GET_U16_MSBFIRST(p)			(	((*((uint8_t *)(p) + 0)) << 8) | \
										((*((uint8_t *)(p) + 1)) << 0))
#define GET_U24_MSBFIRST(p)			(	((*((uint8_t *)(p) + 0)) << 16) | \
										((*((uint8_t *)(p) + 1)) << 8) | \
										((*((uint8_t *)(p) + 2)) << 0))
#define GET_U32_MSBFIRST(p)			(	((*((uint8_t *)(p) + 0)) << 24) | \
										((*((uint8_t *)(p) + 1)) << 16) | \
										((*((uint8_t *)(p) + 2)) << 8) | \
										((*((uint8_t *)(p) + 3)) << 0))
#define GET_U16_LSBFIRST(p)			(	((*((uint8_t *)(p) + 0)) << 0) | \
										((*((uint8_t *)(p) + 1)) << 8))
#define GET_U24_LSBFIRST(p)			(	((*((uint8_t *)(p) + 0)) << 0) | \
										((*((uint8_t *)(p) + 1)) << 8) | \
										((*((uint8_t *)(p) + 2)) << 16))
#define GET_U32_LSBFIRST(p)			(	((*((uint8_t *)(p) + 0)) << 0) | \
										((*((uint8_t *)(p) + 1)) << 8) | \
										((*((uint8_t *)(p) + 2)) << 16) | \
										((*((uint8_t *)(p) + 3)) << 24))

#define SET_U16_MSBFIRST(p, v)		\
	do{\
		*((uint8_t *)(p) + 0) = (((uint16_t)(v)) >> 8) & 0xFF;\
		*((uint8_t *)(p) + 1) = (((uint16_t)(v)) >> 0) & 0xFF;\
	} while (0)
#define SET_U24_MSBFIRST(p, v)		\
	do{\
		*((uint8_t *)(p) + 0) = (((uint32_t)(v)) >> 16) & 0xFF;\
		*((uint8_t *)(p) + 1) = (((uint32_t)(v)) >> 8) & 0xFF;\
		*((uint8_t *)(p) + 2) = (((uint32_t)(v)) >> 0) & 0xFF;\
	} while (0)
#define SET_U32_MSBFIRST(p, v)		\
	do{\
		*((uint8_t *)(p) + 0) = (((uint32_t)(v)) >> 24) & 0xFF;\
		*((uint8_t *)(p) + 1) = (((uint32_t)(v)) >> 16) & 0xFF;\
		*((uint8_t *)(p) + 2) = (((uint32_t)(v)) >> 8) & 0xFF;\
		*((uint8_t *)(p) + 3) = (((uint32_t)(v)) >> 0) & 0xFF;\
	} while (0)
#define SET_U16_LSBFIRST(p, v)		\
	do{\
		*((uint8_t *)(p) + 0) = (((uint16_t)(v)) >> 0) & 0xFF;\
		*((uint8_t *)(p) + 1) = (((uint16_t)(v)) >> 8) & 0xFF;\
	} while (0)
#define SET_U24_LSBFIRST(p, v)		\
	do{\
		*((uint8_t *)(p) + 0) = (((uint32_t)(v)) >> 0) & 0xFF;\
		*((uint8_t *)(p) + 1) = (((uint32_t)(v)) >> 8) & 0xFF;\
		*((uint8_t *)(p) + 2) = (((uint32_t)(v)) >> 16) & 0xFF;\
	} while (0)
#define SET_U32_LSBFIRST(p, v)		\
	do{\
		*((uint8_t *)(p) + 0) = (((uint32_t)(v)) >> 0) & 0xFF;\
		*((uint8_t *)(p) + 1) = (((uint32_t)(v)) >> 8) & 0xFF;\
		*((uint8_t *)(p) + 2) = (((uint32_t)(v)) >> 16) & 0xFF;\
		*((uint8_t *)(p) + 3) = (((uint32_t)(v)) >> 24) & 0xFF;\
	} while (0)

#define GET_LE_U16(p)				GET_U16_LSBFIRST(p)
#define GET_LE_U24(p)				GET_U24_LSBFIRST(p)
#define GET_LE_U32(p)				GET_U32_LSBFIRST(p)
#define GET_BE_U16(p)				GET_U16_MSBFIRST(p)
#define GET_BE_U24(p)				GET_U24_MSBFIRST(p)
#define GET_BE_U32(p)				GET_U32_MSBFIRST(p)
#define SET_LE_U16(p, v)			SET_U16_LSBFIRST(p, v)
#define SET_LE_U24(p, v)			SET_U24_LSBFIRST(p, v)
#define SET_LE_U32(p, v)			SET_U32_LSBFIRST(p, v)
#define SET_BE_U16(p, v)			SET_U16_MSBFIRST(p, v)
#define SET_BE_U24(p, v)			SET_U24_MSBFIRST(p, v)
#define SET_BE_U32(p, v)			SET_U32_MSBFIRST(p, v)

#define SWAP_U16(v)					((((uint16_t)(v) & 0xFF00) >> 8) | \
										(((uint16_t)(v) & 0x00FF) << 8))
#define SWAP_U24(v)					((((uint32_t)(v) & 0x00FF0000) >> 16) | \
										(((uint32_t)(v) & 0x0000FF00) << 0) | \
										(((uint32_t)(v) & 0x000000FF) << 16))
#define SWAP_U32(v)					((((uint32_t)(v) & 0xFF000000) >> 24) | \
										(((uint32_t)(v) & 0x00FF0000) >> 8) | \
										(((uint32_t)(v) & 0x0000FF00) << 8) | \
										(((uint32_t)(v) & 0x000000FF) << 24))

#if defined(__BIG_ENDIAN__) && (__BIG_ENDIAN__ == 1)
#	define LE_TO_SYS_U16(v)			SWAP_U16(v)
#	define LE_TO_SYS_U24(v)			SWAP_U24(v)
#	define LE_TO_SYS_U32(v)			SWAP_U32(v)
#	define BE_TO_SYS_U16(v)			((uint16_t)(v))
#	define BE_TO_SYS_U24(v)			((uint32_t)(v))
#	define BE_TO_SYS_U32(v)			((uint32_t)(v))

#	define SYS_TO_LE_U16(v)			SWAP_U16(v)
#	define SYS_TO_LE_U24(v)			SWAP_U24(v)
#	define SYS_TO_LE_U32(v)			SWAP_U32(v)
#	define SYS_TO_BE_U16(v)			((uint16_t)(v))
#	define SYS_TO_BE_U24(v)			((uint32_t)(v))
#	define SYS_TO_BE_U32(v)			((uint32_t)(v))

#	define GET_SYS_U16(p)			GET_BE_U16(p)
#	define GET_SYS_U24(p)			GET_BE_U24(p)
#	define GET_SYS_U32(p)			GET_BE_U32(p)
#else
#	define LE_TO_SYS_U16(v)			((uint16_t)(v))
#	define LE_TO_SYS_U24(v)			((uint32_t)(v))
#	define LE_TO_SYS_U32(v)			((uint32_t)(v))
#	define BE_TO_SYS_U16(v)			SWAP_U16(v)
#	define BE_TO_SYS_U24(v)			SWAP_U24(v)
#	define BE_TO_SYS_U32(v)			SWAP_U32(v)

#	define SYS_TO_LE_U16(v)			((uint16_t)(v))
#	define SYS_TO_LE_U24(v)			((uint32_t)(v))
#	define SYS_TO_LE_U32(v)			((uint32_t)(v))
#	define SYS_TO_BE_U16(v)			SWAP_U16(v)
#	define SYS_TO_BE_U24(v)			SWAP_U24(v)
#	define SYS_TO_BE_U32(v)			SWAP_U32(v)

#	define GET_SYS_U16(p)			GET_LE_U16(p)
#	define GET_SYS_U24(p)			GET_LE_U24(p)
#	define GET_SYS_U32(p)			GET_LE_U32(p)
#endif     
     
#define IS_FSM_ERR(__FSM_RT)        ((__FSM_RT) < fsm_rt_cpl)
#define is_fsm_err(__FSM_RT)        IS_FSM_ERR(__FSM_RT)
/*============================ TYPES =========================================*/
#ifndef __FSM_RT_TYPE__
#define __FSM_RT_TYPE__
//! \name finit state machine state
//! @{
typedef enum {
    fsm_rt_err          = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl          = 0,     //!< fsm complete
    fsm_rt_on_going     = 1,     //!< fsm on-going
    fsm_rt_wait_for_obj = 2,     //!< fsm wait for object
    fsm_rt_asyn         = 3,     //!< fsm asynchronose complete, you can check it later.
} fsm_rt_t;
//! @}

#endif


#define __REG_TYPE__

typedef volatile unsigned char     reg8_t;
typedef volatile unsigned short    reg16_t;
typedef volatile unsigned int      reg32_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif // __APP_TYPE_H_INCLUDED__
