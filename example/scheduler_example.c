/****************************************************************************
*  Copyright 2018 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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
#include ".\app_platform\app_platform.h"
#include <string.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define __def_fnfsm(__NAME, __CALL_DEEPTH, ...)                                 \
    declare_class(fsm(__NAME))                                                  \
    def_class(fsm(__NAME))                                                      \
        __VA_ARGS__                                                             \
    end_def_class(fsm(__NAME))                                                  \
    NO_INIT volatile safe_call_stack_item_t                                     \
        s_tfsm_##__NAME##CallStack[(__CALL_DEEPTH)+1];                          \
    extern fsm_rt_t fsm_##__NAME##_start(void *pArg, void *pTask);
    
#define def_fnfsm(__NAME, ...)              __def_fnfsm(__NAME, __VA_ARGS__)
#define end_def_fnfsm(__NAME, ...)          


#define __def_fnfsm_state(__STATE)                                              \
        static fsm_rt_t fsm_##__STATE(void *pArg, void *pTask);
#define def_fnfsm_state(__STATE)            __def_fnfsm_state(__STATE)

#define implement_fnfsm(__NAME, ...)                                            \
                                                                                \
fsm_rt_t fsm_##__NAME##_start(void *pArg, void *pTask)                          \
{                                                                               \
    fsm_rt_t tFSMReturn = fsm_rt_on_going;                                      \
    class_internal(pArg, ptThis, fsm_##__NAME##_t);                             \
    if (NULL == ptThis) {                                                       \
        return fsm_rt_err;                                                      \
    }                                                                           \
    __VA_ARGS__;                                                                \
    return tFSMReturn;                                                          \
}

#define __fnfsm_state(__STATE, ...)                                             \
    static fsm_rt_t fsm_##__STATE(void *pArg, void *pTask)                      \
    {                                                                           \
        fsm_rt_t tFSMReturn = fsm_rt_on_going;                                  \
        class_internal(pArg, ptThis, fsm_print_str_t);                          \
        {                                                                       \
            __VA_ARGS__;                                                        \
        }                                                                       \
        return tFSMReturn;                                                      \
    }
#define fnfsm_state(__STATE, ...)           __fnfsm_state(__STATE, __VA_ARGS__)


#define __fnfsm_transfer_to(__STATE)                                            \
            do {                                                                \
                SCHEDULER.Call(pTask, fsm_##__STATE, pArg, false);              \
                fnfsm_on_going();                                               \
            } while(0)
            
#define fnfsm_transfer_to(__STATE)          __fnfsm_transfer_to(__STATE)

#define __fnfsm_update_state_to(__STATE)                                        \
        do {                                                                    \
            tFSMReturn = fsm_##__STATE(pArg, pTask);                            \
        } while(0)
#define fnfsm_update_state_to(__STATE)      __fnfsm_update_state_to(__STATE)

#define __fnfsm_cpl(__NAME)                                                     \
            do {                                                                \
                fnfsm_reset(__NAME);                                            \
                return fsm_rt_cpl;                                              \
            } while(0)
#define fnfsm_cpl(__NAME)                   __fnfsm_cpl(__NAME)
            
#define fnfsm_on_going()                    return fsm_rt_on_going;
#define __fnfsm_reset(__NAME)                                                   \
            do {                                                                \
                SCHEDULER.Call(pTask, fsm_##__NAME##_start, pArg, false);       \
            }while(0)
            
#define fnfsm_reset(__NAME)                 __fnfsm_reset(__NAME)

#define fnfsm_body(...)                     __VA_ARGS__

#ifndef on_start
#   define on_start(...)                    __VA_ARGS__
#endif

#ifndef def_params
#   define def_params(...)                  __VA_ARGS__
#endif

#ifndef args
#   define args(...)                        ,__VA_ARGS__
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


def_fnfsm(print_str, 0,
    def_params(
        uint8_t *pchSrc;
    )
)

    def_fnfsm_state(check)
    def_fnfsm_state(putchar)

end_def_fnfsm(print_str)

                                         

implement_fnfsm(print_str,
        on_start(
            //fnfsm_transfer_to(check);  
            fnfsm_update_state_to(check);
        )
    )
    
    fnfsm_body(

        fnfsm_state(check,
            if (0 == *(this.pchSrc)) {
                fnfsm_cpl(print_str);
            }
            fnfsm_transfer_to(putchar);
        )
        
        fnfsm_state(putchar,
            if (STREAM_OUT.Stream.WriteByte(*this.pchSrc)) {
                this.pchSrc++;
                
                fnfsm_transfer_to(check);
            }
        )
        
    )


