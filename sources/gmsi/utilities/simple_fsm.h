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
#ifndef __SIMPLE_FSM_H__
#define __SIMPLE_FSM_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/


#ifndef this
#   define this    (*ptThis)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define def_states(...)                                             \
        enum {                                                      \
            START = 0,                                              \
            __VA_ARGS__                                             \
        };

#define def_params(...)         __VA_ARGS__

#define args(...)             ,__VA_ARGS__

#define fsm(__NAME) fsm_##__NAME##_t

#define __simple_fsm(__FSM_TYPE, ...)                               \
        DECLARE_CLASS(__FSM_TYPE)                                   \
        DEF_CLASS(__FSM_TYPE)                                       \
            uint_fast8_t chState;                                   \
            __VA_ARGS__                                             \
        END_DEF_CLASS(__FSM_TYPE)

#define simple_fsm(__NAME, ...)                                     \
        __simple_fsm(fsm(__NAME), __VA_ARGS__)

#define __extern_simple_fsm(__FSM_TYPE, ...)                        \
        DECLARE_CLASS(__FSM_TYPE)                                   \
        EXTERN_CLASS(__FSM_TYPE)                                    \
            uint_fast8_t chState;                                   \
            __VA_ARGS__                                             \
        END_EXTERN_CLASS(__FSM_TYPE)                                



#define extern_simple_fsm(__NAME, ...)                                          \
        __extern_simple_fsm(fsm(__NAME), __VA_ARGS__)  

#define extern_fsm_initialiser(__NAME, ...)                                     \
        extern fsm(__NAME) *__NAME##_init(fsm(__NAME) *ptFSM __VA_ARGS__);

#define __extern_fsm_implementation(__NAME, ...)                                \
        fsm_rt_t __NAME( fsm(__NAME) *ptFSM __VA_ARGS__ )

#define extern_fsm_implementation(__NAME, ...)                                  \
        __extern_fsm_implementation(__NAME, __VA_ARGS__)

#define call_fsm(__NAME, __FSM, ...)                                            \
        __NAME((__FSM) __VA_ARGS__)

#define state(__STATE, ...)                                                     \
        case __STATE:                                                           \
            {__VA_ARGS__;}

#define on_start(...)                       __VA_ARGS__


#define reset_fsm()         do { ptThis->chState = 0; } while(0);
#define fsm_cpl()           do {reset_fsm(); return fsm_rt_cpl;} while(0);
#define fsm_report(__CODE)  return (__ERROR);
#define fsm_on_going()      return fsm_rt_on_going;


#define update_state_to(__STATE)                                                \
        { ptThis->chState = (__STATE); }

#define transfer_to(__STATE)                                                    \
         { update_state_to(__STATE); fsm_on_going() } 


#define fsm_initialiser(__NAME, ...)                                            \
        fsm(__NAME) *__NAME##_init(fsm(__NAME) *ptFSM __VA_ARGS__)              \
        {                                                                       \
            CLASS(fsm_##__NAME##_t) *ptThis = ( CLASS(fsm_##__NAME##_t) *)ptFSM;\
            if (NULL == ptThis) {                                               \
                return NULL;                                                    \
            }                                                                   \
            ptThis->chState = 0;

#define abort_init()     return NULL;

#define init_body(...)                                                          \
            __VA_ARGS__                                                         \
            return ptFSM;                                                       \
        }
            

#define init_fsm(__NAME, __FSM, ...)                                            \
        __NAME##_init((__FSM) __VA_ARGS__)

#define fsm_implementation(__NAME, ...)                                         \
        implement_fsm(__NAME, __VA_ARGS__)

#define implement_fsm(__NAME, ...)                                              \
    fsm_rt_t __NAME( fsm(__NAME) *ptFSM __VA_ARGS__ )                           \
    {                                                                           \
        CLASS(fsm_##__NAME##_t) *ptThis = (CLASS(fsm_##__NAME##_t) *)ptFSM;     \
        if (NULL == ptThis) {                                                   \
            return fsm_rt_err;                                                  \
        }                                                           

#define body(...)                                                               \
        switch (ptThis->chState) {                                              \
            case 0:                                                             \
                ptThis->chState++;                                              \
            __VA_ARGS__                                                         \
        }                                                                       \
                                                                                \
        return fsm_rt_on_going;                                                 \
    }


/*============================ TYPES =========================================*/

#ifndef __FSM_RT_TYPE__
#define __FSM_RT_TYPE__
//! \name finit state machine state
//! @{
typedef enum {
    fsm_rt_err          = -1,    //!< fsm error, error code can be get from other interface
    fsm_rt_cpl          = 0,     //!< fsm complete
    fsm_rt_on_going     = 1,     //!< fsm on-going
    fsm_rt_wait_for_obj = 2,     //!< fsm wait for object
    fsm_rt_asyn         = 3,     //!< fsm asynchronose complete, you can check it later.
} fsm_rt_t;
//! @}

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/




#endif

/* EOF */
