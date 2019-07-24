/***************************************************************************
 *   Copyright(C)2009-2014 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __TGUI_GRID_INTERFACE_H__
#define __TGUI_GRID_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\interface.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

//! \name grid
//! @{
typedef struct {
    union {
        int_fast8_t chX;
        int_fast8_t chLeft;
    };
    union {
        int_fast8_t chY;
        int_fast8_t chTop;
    };
} grid_t;
//! @}

//! \name grid rectangle
//! @{
typedef struct {
    union {
        grid_t;
        grid_t  __grid_t;
    };
    int_fast8_t chWidth;            //!< Width in grid
    int_fast8_t chHeight;           //!< Height in grid
} grid_rect_t;
//! @}


//! \name grid brush
//! @{
typedef struct {
    color_t     tForeground;
    color_t     tBackground;
} grid_brush_t;
//! @}

//*! \name grid drawing context
//! @{
DEF_INTERFACE(i_gdc_t, 
        DEF_INTERFACE(grid_property_t)
            fsm_rt_t        (*Set)(grid_t tGrid);
            fsm_rt_t        (*Get)(grid_t *ptGrid);
            fsm_rt_t        (*SaveCurrent)(void);
            fsm_rt_t        (*Resume)(void);
        END_DEF_INTERFACE(grid_property_t)
        
        DEF_INTERFACE(grid_brush_property_t)
            fsm_rt_t        (*Set)(grid_brush_t tBrush);
            grid_brush_t    (*Get)(void);
        END_DEF_INTERFACE(grid_brush_property_t)
    )
    const struct {
        int_fast8_t         chWidth;
        int_fast8_t         chHeight;
    }                       Info;
    grid_property_t         Position;
    grid_brush_property_t   Color;
    fsm_rt_t                (*Clear)(void);
    fsm_rt_t                (*Print)(uint8_t *pchString, uint_fast16_t hwSize);
END_DEF_INTERFACE(i_gdc_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */