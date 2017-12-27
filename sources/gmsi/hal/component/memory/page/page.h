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
#ifndef __MEMORY_PAGE_H__
#define __MEMORY_PAGE_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\interface.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define PAGE_CFG(__PAGE, ...)               \
        do {                                \
            page_cfg_t tCFG = {__VA_ARGS__};\
            page_init((__PAGE),&tCFG);      \
        } while(false)

/*============================ TYPES =========================================*/

//! \name memory page size
//! @{
typedef enum {
    PAGE_SIZE_4,                //!< 4      units
    PAGE_SIZE_8,                //!< 8      units
    PAGE_SIZE_16,               //!< 16     units
    PAGE_SIZE_32,               //!< 32     units
    PAGE_SIZE_64,               //!< 64     units
    PAGE_SIZE_128,              //!< 128    units
    PAGE_SIZE_256,              //!< 256    units
    PAGE_SIZE_512,              //!< 512    units
    PAGE_SIZE_1024,             //!< 1024   units
    PAGE_SIZE_2048,             //!< 2048   units
    PAGE_SIZE_4096              //!< 4096   units
}page_size_t;
//! @}


//! \name page configuration
//! @{
typedef struct {
    i_page_t    tIO;                    //!< IO
    uint8_t     *pchBuffer;             //!< page buffer
    page_size_t tPageSize;              //!< page size(buffer size)
    void        *pObj;
}page_cfg_t;
//! @}

DECLARE_CLASS(page_t)
//! \name class page_t
//! @{
EXTERN_CLASS(page_t)
    const i_page_t  tIO;
    void            *pObj;
    uint8_t         *pchBuffer;
    const uint16_t  hwPageSize;
    locker_t        bCritical;
    uint8_t         bModified;
    uint8_t         chFSMWrite;
    uint8_t         chFSMRead;
    uint8_t         *pchStream;
    uint16_t        hwCount;
    uint32_t        wAddress;
    uint32_t        wPageAddress;
END_EXTERN_CLASS(page_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief initialize page service
 *! \param ptPage page_t object
 *! \param ptCFG configuration structure
 *! \retval true initialize succeed
 *! \retval false intizlization fail.
 */
extern bool page_init(page_t *ptPage, page_cfg_t *ptCFG);

/*! \brief write memory disregard page
 *! \param ptPage page_t object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
extern fsm_rt_t write_memory_disregard_page( page_t *ptPage, 
     uint_fast16_t hwAddress, uint8_t *pchStream, uint_fast16_t hwSize );

/*! \brief read memory disregard page
 *! \param ptPage page_t object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
extern fsm_rt_t read_memory_disregard_page( page_t *ptPage, 
     uint_fast16_t hwAddress, uint8_t *pchStream, uint_fast16_t hwSize );
#endif
/* EOF */
