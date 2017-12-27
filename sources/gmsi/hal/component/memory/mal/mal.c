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

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include ".\interface.h"
#include "..\page\page.h"

/*============================ MACROS ========================================*/
#ifndef this
#   define this                (*ptThis)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

DECLARE_CLASS(mem_helper_t)

//! \name class mem_helper_t
//! @{
DEF_CLASS(mem_helper_t)
    i_mem_t *ptMEM;                         //!< target mal interface
    void    *ptTarget;                      //!< target object
    void    *pPageBuffer;                   
END_DEF_CLASS(mem_helper_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/


/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief mal general page write
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \return state machine state
 */
static fsm_rt_t mal_page_write( 
    void *pObj, uint32_t wAddress, uint8_t *pchStream)
{
    mem_t *ptThis = (mem_t *)pObj;
    const i_mem_t *ptMethod = this.ptMethod;
    
    if (NULL == ptMethod) {
        return fsm_rt_err;
    } else if (NULL == ptMethod->PageWrite) {
        return fsm_rt_cpl;
    }

    uint8_t chShift = ptMethod->Info(ptThis).chPageSizeBit;
    uint16_t hwPageSize = ptMethod->Info(ptThis).hwPageSize;

    return ptMethod->PageWrite( ptThis, 
                            wAddress >> chShift,            //!< page address
                            pchStream
                        );
}

/*! \brief mal general page read
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \return state machine state
 */
static fsm_rt_t mal_page_read( 
    void *pObj, uint32_t wAddress, uint8_t *pchStream)
{
    mem_t *ptThis = (mem_t *)pObj;
    const i_mem_t *ptMethod = this.ptMethod;
    if (NULL == ptMethod) {
        return fsm_rt_err;
    } else if (NULL == ptMethod->PageRead) {
        return fsm_rt_cpl;
    }

    uint8_t chShift = ptMethod->Info(ptThis).chPageSizeBit;
    uint16_t hwPageSize = ptMethod->Info(ptThis).hwPageSize;

    return ptMethod->PageRead( ptThis, 
                            wAddress >> chShift,            //!< page address
                            wAddress & (_BV(chShift) - 1),  //!< page offset
                            pchStream,
                            hwPageSize
                        );
}

/*! \brief mal general page erase
 *! \param hwAddress target memory address
 *! \return state machine state
 */
static fsm_rt_t mal_page_erase(void *pObj, uint32_t wAddress)
{
    mem_t *ptThis = (mem_t *)pObj;
    const i_mem_t *ptMethod = this.ptMethod;
    if (NULL == ptMethod) {
        return fsm_rt_err;
    } else if (NULL == ptMethod->PageErase) {
        return fsm_rt_cpl;
    }

    uint8_t chShift = this.ptMethod->Info(ptThis).chPageSizeBit;

    return this.ptMethod->PageErase(ptThis, wAddress >> chShift);
}

/*! \brief read mal memory disregard page
 *! \param ptMEM memory object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
fsm_rt_t mal_mem_read( mem_t *ptMEM, 
     uint_fast16_t hwAddress, uint8_t *pchStream, uint_fast16_t hwSize )
{
    CLASS(mem_t) *ptThis = (CLASS(mem_t) *)ptMEM;
    return read_memory_disregard_page( this.ptPage, hwAddress, pchStream, hwSize );
}

/*! \brief write mal memory disregard page
 *! \param ptMEM memory object
 *! \param hwAddress target memory address
 *! \param pchStream data buffer
 *! \param hwSize target buffer size
 *! \return state machine state
 */
fsm_rt_t mal_mem_write( mem_t *ptMEM, 
     uint_fast16_t hwAddress, uint8_t *pchStream, uint_fast16_t hwSize )
{
    CLASS(mem_t) *ptThis = (CLASS(mem_t) *)ptMEM;
    return write_memory_disregard_page( this.ptPage, hwAddress, pchStream, hwSize );
}

/*! \brief initialize mal memory
 *! \param ptMEM target memory
 *! \param ptPage page obj
 *! \param pchBuffer page buffer
 *! \param tSize page size
 *! \return access result
 */
bool mal_mem_init(mem_t *ptMEM, page_t *ptPage, uint8_t *pchBuffer, page_size_t tSize)
{
    CLASS(mem_t) *ptThis = (CLASS(mem_t) *)ptMEM;
    page_cfg_t tCFG = {
        .tIO = {
            .fnPageWrite = &mal_page_write,
            .fnPageRead = &mal_page_read,
            .fnPageErase = &mal_page_erase,
        },
        pchBuffer,
        tSize,
        ptMEM
    };
    this.ptPage = ptPage;
    
    return page_init(ptPage, &tCFG);
}


/*! \note initialize memory abstract layer
 *  \param none
 *  \retval true hal initialization succeeded.
 *  \retval false hal initialization failed
 */  
bool mal_init( void )
{


    return true;
}


/* EOF */
