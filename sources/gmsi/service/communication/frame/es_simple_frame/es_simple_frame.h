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



#ifndef __USE_ES_SIMPLE_FRAME_H__
#define __USE_ES_SIMPLE_FRAME_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"
#include "..\iframe.h"

#if USE_SERVICE_ES_SIMPLE_FRAME == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

//! \brief macro for es-simple frame initialization
#define ES_SIMPLE_FRAME_CFG(__FRAME__, ...)                                     \
            do {                                                                \
                es_simple_frame_cfg_t tCFG = {__VA_ARGS__};                     \
                es_simple_frame_init((__FRAME__),&tCFG);                        \
            } while(false)



/*============================ TYPES =========================================*/
extern_simple_fsm(es_simple_frame_parser,
        def_params(
            i_byte_pipe_t *ptPipe;          //!< pipe
            frame_parser_t *fnParser;       //!< parser
            uint8_t *pchBuffer;
            uint16_t hwSize;
            uint16_t hwLength;
            uint16_t hwCounter;
            uint16_t hwCheckSUM;
        )
    )

declare_class(es_simple_frame_t)

//! \name class: e-snail simple frame
//! @{
extern_class(es_simple_frame_t)

    //uint_fast8_t chState;
    inherit(fsm(es_simple_frame_parser))

end_extern_class(es_simple_frame_t)
//! @}

//! \name es-simple frame configuration structure
//! @{
typedef struct {
    i_byte_pipe_t   *ptPipe; 
    uint8_t         *pchBuffer;
    uint_fast16_t   hwSize;
    frame_parser_t  *fnParser;
}es_simple_frame_cfg_t;
//! @}

//! \name frame interface
//! @{
def_interface(i_es_simple_frame_t)
    bool (*Init)(es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);
    fsm_rt_t (*Task)(es_simple_frame_t *ptFrame);
end_def_interface(i_es_simple_frame_t)
//! @}



/*============================ GLOBAL VARIABLES ==============================*/
extern const i_es_simple_frame_t ES_SIMPLE_FRAME;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if  !defined(__LIB_REFERENCE__)
/*! \brief initlialize es_simple_frame
 *! \param ptFrame es_simple_frame object
 *! \param ptCFG configuration object
 *! \retval true initliazation is successful
 *! \retval false failed in initialization
 */
extern bool es_simple_frame_init(  
    es_simple_frame_t *ptFrame, es_simple_frame_cfg_t *ptCFG);

/*! \brief es simple frame task
 *! \param ptFrame es_simple_frame object
 *! \return FSM status
 */
extern fsm_rt_t es_simple_frame_task(es_simple_frame_t *ptFrame);

#endif

#endif

#endif