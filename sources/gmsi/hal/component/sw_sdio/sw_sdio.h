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

#ifndef __SW_SDIO_H__
#define __SW_SDIO_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

#if USE_COMPONENT_SW_SDIO == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name sdio config clock
//! @{ 
typedef struct {
    uint8_t    chClockDiv;
} sdio_cfg_t;
//! @}

//! \name sdio return value type
//! @{ 
typedef enum {	
    SDIO_ON_GOING      = 0,    //!< SDIO is transfering 			
    SDIO_CMD_SET       = 1,	   //!< Command already send, but no response required
    SDIO_CMD_RESPONSE  = 2,    //!< Command already receive, CRC check passed
    SDIO_CMD_CRCFAIL   = 3,	   //!< Command already receive, CRC check failed
    SDIO_CMD_TIMEOUT   = 4,    //!< Command response timeout
    SDIO_DATA_CRCFALL  = 5,    //!< Wirte or read data CRC check failed
    SDIO_DATA_PASSED   = 6,    //!< Wirte or read data and CRC check passed
    SDIO_DATA_TIMEOUT  = 7,    //!< read data timeout
    SDIO_DATA_ERROR    = 8 ,   //!< Write or read data error
} em_sdio_rt_t;		
//! @}
		   
//! \name sdio response type
//! @{ 
typedef enum {
    SDIO_NO_RESPONSE = 0,       
    SDIO_STANDARD_RESPONSE,     //!< Response 48 bits format
    SDIO_EXPAND_RESPONSE,       //!< Response 136 bits format
    SDIO_SYNCHRONOUS_RECEIVE,   //!< Synchronous receive 48 bits response and data
} em_sdio_response_t;
//! @}

//! \name sdio direction type
//! @{ 
typedef enum {
    SDIO_READ = 0,
    SDIO_WRITE,
} em_sdio_direction_t;
//! @}

//! \name sdio block size type
//! @{ 
typedef enum {
    SDIO_1B_BLOCK    = 1,
    SDIO_2B_BLOCK    = 2,
    SDIO_4B_BLOCK    = 4,
    SDIO_8B_BLOCK    = 8,
    SDIO_16B_BLOCK   = 16,
    SDIO_32B_BLOCK   = 32,
    SDIO_64B_BLOCK   = 64,
    SDIO_128B_BLOCK  = 128,
    SDIO_256B_BLOCK  = 256,
    SDIO_512B_BLOCK  = 512,
    SDIO_1024B_BLOCK = 1024,
    SDIO_2048B_BLOCK = 2048
} em_sdio_block_size_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*! \brief sdio initialize
 *! \param ptSDIOcfg sdio_cfg_t type pointer
 *! \return state machine status
 */
extern fsm_rt_t sdio_init(sdio_cfg_t *ptSDIOCfg);


/*! \brief sdio change clock frequency
 *! \param chClockDiv set sdio clock frequency
 *! \retval none
 */
extern void sdio_change_clock(uint8_t chClockDiv);


/*! \brief sdio command transfer
 *! \param chCMDIndex command index
 *! \param wArgument  command argument
 *! \param tResponseExpected response type, refer to em_sdio_response_t type
 *! \param pwResponse save response value 
 *! \return sdio state, refer to em_sdio_rt_t type
 */
extern em_sdio_rt_t sdio_cmd_transfer( uint8_t  chCMDIndex, 
                                    uint32_t wArgument, 
                                    em_sdio_response_t tResponseExpected, 
                                    uint32_t *pwResponse);
 

/*! \brief sdio data transfer 
 *! \param tDirection sdio transfer direction, refer to em_sdio_direction_t type
 *! \param tBlockSize set block size
 *! \param pchData data buffer pointer
 *! \param hwDataSize data size
 *! \return sdio state, refer to em_sdio_rt_t type
 */
extern em_sdio_rt_t sdio_data_transfer( em_sdio_direction_t  tDirection, 
                                     em_sdio_block_size_t tBlockSize, 
                                     uint8_t  *pchData, 
                                     uint16_t hwDataSize); 

#endif

#endif
/* EOF */
