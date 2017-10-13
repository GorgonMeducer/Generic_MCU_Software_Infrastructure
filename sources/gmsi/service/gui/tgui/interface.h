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

#ifndef __TGUI_INTERFACE_H__
#define __TGUI_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if  TGUI_SIZE_INT_TYPE == TGUI_TINY
typedef int_fast8_t     tgui_int_t;
#elif   TGUI_SIZE_INT_TYPE == TGUI_MEDIUM
typedef int_fast16_t    tgui_int_t;
#else
typedef int_fast32_t    tgui_int_t;
#endif

//! \name point
//! @{
typedef struct {
    union {
        tgui_int_t  tX;
        tgui_int_t  tLeft;
    };
    union {
        tgui_int_t  tY;
        tgui_int_t  tTop;
    };
} point_t;
//! @}

//! \name rectangle
//! @{
typedef struct {
    union {
        point_t;
        point_t __point_t;
    };
    tgui_int_t  tWidth;             //!< Width in pix
    tgui_int_t  tHeight;            //!< Height in pix
} rect_t;
//! @}

#if     TGUI_COLOR_BITS == TGUI_24BITS
typedef union {
    uint32_t    tValue;
    struct {
        uint8_t chR;
        uint8_t chG;
        uint8_t ghB;
    };
} color_t;
#elif   TGUI_COLOR_BITS == TGUI_8BITS
typedef union {
    uint8_t     tValue;         
} color_t;
#else   /*TGUI_COLOR_BITS == TGUI_4BITS*/
typedef union {
    uint8_t     tValue;
} color_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */