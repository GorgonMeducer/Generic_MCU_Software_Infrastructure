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
//! \brief import configurations
#include ".\app_cfg.h"
#include ".\interface.h"
#include "..\..\page\page.h"

#if USE_COMPONENT_MAL_SRAM == ENABLED

/*============================ MACROS ========================================*/
//! \note sram mal page size infomation
//! @{
#ifndef SRAM_SIZE
#   define SRAM_SIZE            (1024ul)
#endif
     
#define SRAM_PAGE_SIZE          (512ul)
#define SRAM_PAGE_SIZE_BIT      9
//! @}
     
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


/*============================ PROTOTYPES ====================================*/
/*! \brief initialize sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl initialize succeed.
 *! \retval fsm_rt_err initialize failed
 */
static fsm_rt_t sram_init(mem_t *ptMal, void *ptCFG);

/*! \brief finish sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl sram mal finish succeed.
 *! \retval fsm_rt_on_going sram mal finish on-going
 */
static fsm_rt_t sram_finish(mem_t *ptMal);

/*! \brief get sram mal module infomation
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return memory infomation
 */
static mem_info_t sram_info(mem_t *ptMal);

/*! \brief open sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl sram mal open succeed.
 *! \retval fsm_rt_on_going sram mal open on-going
 */
static fsm_rt_t sram_open(mem_t *ptMal);

/*! \brief close sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl sram mal finish succeed.
 *! \retval fsm_rt_on_going sram mal finish on-going
 */
static fsm_rt_t sram_close(mem_t *ptMal);

/*! \brief get sram mal module status
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return sram mal status
 */
static em_mem_status_t sram_status(mem_t *ptMal);

/*! \brief write a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param ptBuffer page bufer 
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_write( mem_t *ptMal, 
                                     uint32_t wPageAddress, 
                                     void *ptBuffer);

/*! \brief read specified data
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_read( mem_t *ptMal, 
                                    uint32_t wPageAddress, 
                                    uint_fast16_t hwOffset, 
                                    uint8_t *pchStream, 
                                    uint_fast16_t hwSize);

/*! \brief verify specified data 
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_verify( mem_t *ptMal, 
                                      uint32_t wPageAddress, 
                                      uint_fast16_t hwOffset, 
                                      uint8_t *pchStream, 
                                      uint_fast16_t hwSize);

/*! \brief erase a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_erase( mem_t *ptMal, 
                                     uint32_t wPageAddress);

/*! \brief chip erase
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_chip_erase(mem_t *ptMal);

/*============================ GLOBAL VARIABLES ==============================*/
//! \brief sram mal object
const i_mem_t I_SRAM = {
        .base__i_mcb_t = {
            .Init       = &sram_init,
            .Finish     = &sram_finish,
            .Info       = &sram_info,
            .Open       = &sram_open,
            .Close      = &sram_close,
            .GetStatus  = &sram_status
        },
        .base__i_mem_page_t = {
            .PageWrite  = &sram_page_write,
            .PageRead   = &sram_page_read,
            .PageVerify = &sram_page_verify,
            .PageErase  = &sram_page_erase,
            .Erase      = &sram_chip_erase
        },
};




INIT_CLASS_OBJ(
        mem_t,  
        SRAM,
        {
            .ptMethod = &I_SRAM,
        }
    );

/*============================ LOCAL VARIABLES ===============================*/
//! \brief system state
static volatile em_mem_status_t s_CurrentState = MEM_NOT_READY;

//! \brief sram mal buffer
NO_INIT static uint8_t s_chSramBuffer[SRAM_SIZE];

/*============================ IMPLEMENTATION ================================*/

/*! \brief initialize sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl initialize succeed.
 *! \retval fsm_rt_err initialize failed
 */
static fsm_rt_t sram_init(mem_t *ptMal, void *ptCFG)
{
    SAFE_ATOM_CODE (
        if (MEM_NOT_READY == s_CurrentState) {
            s_CurrentState = MEM_READY_IDLE;
        }
    )

    return fsm_rt_cpl;  
}

/*! \brief finish sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl sram mal finish succeed.
 *! \retval fsm_rt_on_going sram mal finish on-going
 */
static fsm_rt_t sram_finish(mem_t *ptMal)
{    
    SAFE_ATOM_CODE (
        s_CurrentState = MEM_NOT_READY; //!< set idle state
    )
    
    return fsm_rt_cpl;                  //!< success
}

/*! \brief get sram mal module infomation
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return memory infomation
 */
static mem_info_t sram_info(mem_t *ptMal)
{
	static const mem_info_t tReturn = {
        .hwProperty = MEM_BLOCK_ACCESS,
        .hwPageSize = SRAM_PAGE_SIZE,
        .wPageCount = SRAM_SIZE / SRAM_PAGE_SIZE,
        .chPageSizeBit = SRAM_PAGE_SIZE_BIT,
    };
    
    return tReturn;
}

/*! \brief open sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl sram mal open succeed.
 *! \retval fsm_rt_on_going sram mal open on-going
 */
static fsm_rt_t sram_open(mem_t *ptMal)
{
    SAFE_ATOM_CODE (
        if (MEM_NOT_READY == s_CurrentState) {
            s_CurrentState = MEM_READY_IDLE;
        }
    )

    return fsm_rt_cpl;  
}

/*! \brief close sram mal module
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_cpl sram mal finish succeed.
 *! \retval fsm_rt_on_going sram mal finish on-going
 */
static fsm_rt_t sram_close(mem_t *ptMal)
{   
    fsm_rt_t tResult = fsm_rt_on_going;

    SAFE_ATOM_CODE (
        if (MEM_READY_BUSY != s_CurrentState) {
            s_CurrentState = MEM_NOT_READY;
            tResult = fsm_rt_cpl;
        }
    )

    return tResult;  
}

/*! \brief get sram mal module status
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \return sram mal status
 */
static em_mem_status_t sram_status(mem_t *ptMal)
{
    em_mem_status_t tResult;
    
    SAFE_ATOM_CODE(
        tResult = s_CurrentState;
    )
        
    return tResult;
}

/*! \brief write a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param ptBuffer page bufer 
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_write(mem_t *ptMal, uint32_t wPageAddress, void *ptBuffer)
{
    uint8_t *pchStream;    
    
    if (    (NULL == ptBuffer) 
        ||  (wPageAddress >= (SRAM_SIZE / SRAM_PAGE_SIZE))) {
        return fsm_rt_err;
    } 

    SAFE_ATOM_CODE(           
        //! whether system is initialized
        if (MEM_NOT_READY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return fsm_rt_err;
        } else if (MEM_READY_BUSY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return (fsm_rt_t)GSF_ERR_NOT_READY;
        }     
        s_CurrentState = MEM_READY_BUSY;        //!< set current state
    )

    pchStream = ptBuffer;

    for (int i = 0; i < SRAM_PAGE_SIZE; i++) {
        s_chSramBuffer[i + wPageAddress * SRAM_PAGE_SIZE] = pchStream[i];
    }                
    
    SAFE_ATOM_CODE(
        s_CurrentState = MEM_READY_IDLE;//!< set idle state
    )

    return fsm_rt_cpl;
}

/*! \brief read a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_read(mem_t *ptMal, uint32_t wPageAddress, 
    uint_fast16_t hwOffset, uint8_t *pchStream, uint_fast16_t hwSize)
{
    uint32_t wOffset = wPageAddress * SRAM_PAGE_SIZE + hwOffset;
    
    if (    (NULL == pchStream) 
        ||  (wOffset + hwSize > SRAM_SIZE)) {
        return fsm_rt_err;
    } else if (0 == hwSize) {
        return fsm_rt_cpl;
    }

    SAFE_ATOM_CODE(           
        //! whether system is initialized
        if (MEM_NOT_READY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return fsm_rt_err;
        } else if (MEM_READY_BUSY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return (fsm_rt_t)GSF_ERR_NOT_READY;
        }     
        s_CurrentState = MEM_READY_BUSY;        //!< set current state
    )

    for (int i = 0; i < hwSize; i++) {
        pchStream[i] = s_chSramBuffer[i + wOffset];
    }            

    SAFE_ATOM_CODE(
        s_CurrentState = MEM_READY_IDLE;//!< set idle state
    )    
    
    return fsm_rt_cpl;
}

/*! \brief verify specified data 
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! \param hwOffset offset within a page
 *! \param pchStream data buffer
 *! \param hwSize buffer size
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_verify(mem_t *ptMal, uint32_t wPageAddress, 
    uint_fast16_t hwOffset, uint8_t *pchStream, uint_fast16_t hwSize)
{
    uint32_t wOffset = wPageAddress * SRAM_PAGE_SIZE + hwOffset;
    fsm_rt_t tResult = fsm_rt_cpl;
    
    if (    (NULL == pchStream) 
        ||  (wOffset + hwSize > SRAM_SIZE)) {
        return fsm_rt_err;
    } else if (0 == hwSize) {
        return fsm_rt_cpl;
    }

    SAFE_ATOM_CODE(           
        //! whether system is initialized
        if (MEM_NOT_READY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return fsm_rt_err;
        } else if (MEM_READY_BUSY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return (fsm_rt_t)GSF_ERR_NOT_READY;
        }     
        s_CurrentState = MEM_READY_BUSY;        //!< set current state
    )

    for (int i = 0; i < hwSize; i++) {
        if (pchStream[i] != s_chSramBuffer[i + wOffset]) {
            tResult = fsm_rt_err;
            break;
        }
    }      

    SAFE_ATOM_CODE(
        s_CurrentState = MEM_READY_IDLE;//!< set idle state
    )          
    
    return tResult;
}

/*! \brief write a sector
 *! 
 *! \param ptMal memory abstract layer pointer
 *! \param wPageAddress page address
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_page_erase(mem_t *ptMal, uint32_t wPageAddress)
{   
    if ((wPageAddress >= (SRAM_SIZE / SRAM_PAGE_SIZE))) {
        return fsm_rt_err;
    } 

    SAFE_ATOM_CODE(           
        //! whether system is initialized
        if (MEM_NOT_READY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return fsm_rt_err;
        } else if (MEM_READY_BUSY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return (fsm_rt_t)GSF_ERR_NOT_READY;
        }     
        s_CurrentState = MEM_READY_BUSY;        //!< set current state
    )

    for (int i = 0; i < SRAM_PAGE_SIZE; i++) {
        s_chSramBuffer[i + wPageAddress * SRAM_PAGE_SIZE] = 0xFF;
    }             

    SAFE_ATOM_CODE(
        s_CurrentState = MEM_READY_IDLE;//!< set idle state
    )   
    
    return fsm_rt_cpl;
}

/*! \brief chip erase
 *! 
 *! \param ptMal memory abstract layer pointer
 *! 
 *! \retval fsm_rt_err current state is busy or illegal parameter
 *! \retval fsm_rt_cpl access succeed
 *! \retval GSF_ERR_NOT_READY system is busy
 *! \retval fsm_rt_on_going current access is on going.
 */
static fsm_rt_t sram_chip_erase(mem_t *ptMal)
{
    SAFE_ATOM_CODE(           
        //! whether system is initialized
        if (MEM_NOT_READY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return fsm_rt_err;
        } else if (MEM_READY_BUSY == s_CurrentState) {
            EXIT_SAFE_ATOM_CODE();
            return (fsm_rt_t)GSF_ERR_NOT_READY;
        }     
        s_CurrentState = MEM_READY_BUSY;        //!< set current state
    )

    for (int i = 0; i < SRAM_SIZE; i++) {
        s_chSramBuffer[i] = 0xFF;
    }       

    SAFE_ATOM_CODE(
        s_CurrentState = MEM_READY_IDLE;//!< set idle state
    )         
    
    return fsm_rt_cpl;
}

/* End of File */

#endif

