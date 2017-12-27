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

#ifndef __MAL_INTERFACES_H__
#define __MAL_INTERFACES_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\interface.h"
#include "..\page\page.h"

//! \name memory status
//! @{
#define MEM_READY_MSK   _BV(0)
#define MEM_BUSY_MSK    _BV(1)


//! \name mem_status_t
//! @{
typedef enum {
    //! memory is not ready
    MEM_NOT_READY       = 0x00,
    //! memory is ready and idle
    MEM_READY_IDLE      = MEM_READY_MSK,
    //! memory is ready and busy
    MEM_READY_BUSY      = MEM_READY_MSK | MEM_BUSY_MSK,
} em_mem_status_t;
//! @}
//! @}

//! \brief memory property
//! @{
typedef enum {
    MEM_BLOCK_ACCESS        = 0,            //!< block access (default)
    MEM_RANDOM_ACCESS,                      //!< random access
    MEM_STREAM_ACCESS                       //!< stream access
}em_mem_property_t;
//! @}

//! \name memory description block
//! @{
typedef struct {
    uint16_t        hwProperty;             //!< memory property
    uint16_t        hwPageSize;             //!< memory page size (type)
    uint32_t        wPageCount;             //!< memory page count
    uint8_t         chPageSizeBit;
} mem_info_t;
//! @}

/*============================ DEPENDENT TYPES ===============================*/

DECLARE_CLASS(mem_t)

//! \name memory control block
//! @{
DEF_INTERFACE(i_mcb_t)

    mem_info_t      (*Info)     (mem_t *ptMal);
                                                                                  
    fsm_rt_t        (*Init)     (mem_t *ptMal, void *ptCFG);                
    fsm_rt_t        (*Finish)   (mem_t *ptMal);
                             
    fsm_rt_t        (*Open)     (mem_t *ptMal);                             
    fsm_rt_t        (*Close)    (mem_t *ptMal);

    fsm_rt_t        (*Config)   (mem_t *ptMal, void *ptCFG);
    em_mem_status_t (*GetStatus)(mem_t *ptMal);

END_DEF_INTERFACE(i_mcb_t)   
//! @}

//! \name memory page access interface
//! @{
DEF_INTERFACE(i_mem_page_t)

    fsm_rt_t (*PageWrite)   (   mem_t *ptMal, 
                                uint32_t wPageAddress, void *ptBuffer);
    fsm_rt_t (*PageErase)   (   mem_t *ptMal, uint32_t wPageAddress);
    fsm_rt_t (*PageRead)    (   mem_t *ptMal, 
                                uint32_t wPageAddress, 
                                uint_fast16_t hwOffset, 
                                uint8_t *pchStream, 
                                uint_fast16_t hwSize);
    fsm_rt_t (*PageVerify)  (   mem_t *ptMal, 
                                uint32_t wPageAddress, 
                                uint_fast16_t hwOffset, 
                                uint8_t *pchStream, 
                                uint_fast16_t hwSize);
    fsm_rt_t (*Erase)       (   mem_t *ptMal);

END_DEF_INTERFACE(i_mem_page_t)
//! @}
    

//! \name Memory Abstraction Layers
//! @{
DEF_INTERFACE(i_mem_t)  
    IMPLEMENT(i_mcb_t) 
    IMPLEMENT(i_mem_page_t)
    
END_DEF_INTERFACE(i_mem_t)
//! @}

//! \brief define abstract class mem_t
DEF_CLASS(mem_t, REF_INTERFACE(i_mem_t))
    page_t *ptPage;

END_DEF_CLASS(mem_t, REF_INTERFACE(i_mem_t))



/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
