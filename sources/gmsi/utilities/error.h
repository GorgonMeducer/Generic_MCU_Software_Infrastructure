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

#ifndef _USE_ERROR_H_
#define _USE_ERROR_H_

/*============================ INCLUDES ======================================*/
#include "compiler.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
//! \brief setting error code
# define SET_ERROR(__ERROR_ADDR,__CODE,__HANDLER)    \
        do{(*(__ERROR_ADDR)) = (__CODE);} while(false)
                      
//! \brief getting error code  
# define GET_ERROR(__ERROR_ADDR)                    (*(__ERROR_ADDR))

/*============================ TYPES =========================================*/

//! \name standard error code
//! @{
typedef enum {
    GSF_ERR_NONE                       =0,     //!< none error
    GSF_ERR_GENERAL                    =-1,    //!< unspecified error
    GSF_ERR_NOT_SUPPORT                =-2,    //!< function not supported
    GSF_ERR_NOT_READY                  =-3,    //!< service not ready yet
    GSF_ERR_NOT_AVAILABLE              =-4,    //!< service not available
    GSF_ERR_NOT_ACCESSABLE             =-5,    //!< target not acceesable
    GSF_ERR_NOT_ENOUGH_RESOURCES       =-6,    //!< no enough resources
    GSF_ERR_FAIL                       =-7,    //!< failed
    GSF_ERR_INVALID_PARAMETER          =-8,    //!< invalid parameter
    GSF_ERR_INVALID_RANGE              =-9,    //!< invalid range
    GSF_ERR_INVALID_PTR                =-10,   //!< invalid pointer
    GSF_ERR_INVALID_KEY                =-11,   //!< invalid key
    GSF_ERR_IO                         =-12,   //!< IO error
    GSF_ERR_REQ_ALREADY_REGISTERED     =-13,   //!< request all ready exist
    GSF_ERR_UNKNOWN                    =-128,  //!< unknown error
} gsf_err_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
