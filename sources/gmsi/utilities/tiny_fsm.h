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

#ifndef __TINY_FSM_H__
#define __TINY_FSM_H__

/*============================ INCLUDES ======================================*/
#include ".\compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/*! \brief you can use tiny fsm at any where you want with little cost. 
           E.g.
            
/ *! \brief function that output a char with none-block manner
 *! \param chByte target char
 *! \retval true the target char has been put into the output buffer 
 *! \retval false service is busy
 * /
extern bool serial_out(uint8_t chByte);

extern void toggle_led_a(void);
extern void toggle_led_b(void);

#define SERIAL_OUT(__BYTE)      serial_out(__BYTE)

DEF_TINY_FSM(Print_String) 
    DEF_PARAM
        uint8_t *pchString;
    END_DEF_PARAM
    PRIVATE bool m_CriticalSection = false;
    
    PRIVATE TINY_STATE(Print_String, Print_Init);
    PRIVATE TINY_STATE(Print_String, Print_Output);
END_DEF_TINY_FSM

PRIVATE TINY_STATE(Print_String, Print_Init) BEGIN
    if ((NULL == PARAM) || (NULL == PARAM->pchString)) {
        TINY_FSM_END;           //!< end fsm
    } else if ('\0' == (*PARAM->pchString)) {
        TINY_FSM_END;           //!< end fsm
    } else if (m_CriticalSection) {
        TINY_FSM_TRANSFER_TO(Print_Init);   //!< try to enter critical section
    }
    m_CriticalSection = true;
    
    TINY_FSM_TRANSFER_TO(Print_Output)
END

PRIVATE TINY_STATE(Print_String, Print_Output) BEGIN
    
    if (SERIAL_OUT(*(PARAM->pchString))) {
        PARAM->pchString++;
        if ('\0' == (*PARAM->pchString)) {
            //! complete
            m_CriticalSection = false;      //!< leave critical section      
            TINY_FSM_END;                   //!< complete
        }
    }

    TINY_FSM_TRANSFER_TO(Print_Output)      //!< reflexive
END


int main(void)
{
    NEW_TINY_FSM(Print_String, DemoStringA)
    NEW_TINY_FSM(Print_String, DemoStringB)
    static uint8_t chDemoA[] = "Hello world";
    static uint8_t chDemoB[] = "Tiny FSM Demo";

    ...
    
    while(true) {
        //! call DemoStringA, the instance of the tiny FSM Print_String
        CALL_TINY_FSM(Print_String, DemoStringA, Print_Init) 
            PARAM_INIT
                chDemoA     //!< output string "Hello world"
            END_PARAM_INIT

        END_CALL_TINY_FSM(Print_String)

        //! an example of checking whether a sepecified fsm is complete or not
        if (IS_TINY_FSM_CPL(DemoStringA)) {
            toggle_led_a();    //!< do something here. E.g. toggle a LED 
        }

        //! call DemoStringB, the instance of the tiny FSM Print_String
        CALL_TINY_FSM(Print_String, DemoStringB, Print_Init) 
            PARAM_INIT
                chDemoB     //!< output string "Tiny FSM Demo"
            END_PARAM_INIT

        END_CALL_TINY_FSM(Print_String)

        //! an example of checking whether a sepecified fsm is complete or not
        if (IS_TINY_FSM_CPL(DemoStringB)) {
            toggle_led_b();    //!< do something here. E.g. toggle a LED 
        }
    }

    return 0;
}
 */

#define DEF_TINY_FSM(__NAME)  \
    typedef struct tiny_fsm_##__NAME##_arg tiny_fsm_##__NAME##_arg_t;\
    typedef void *(*tiny_fsm_##__NAME##_task)(tiny_fsm_##__NAME##_arg_t *pArg);\
    struct tiny_fsm_##__NAME##_arg

#define DEF_PARAM       {
#define END_DEF_PARAM   };

#define END_DEF_TINY_FSM

#define NEW_TINY_FSM(__NAME, __VAR) \
    tiny_fsm_##__NAME##_task s_TinyFSM##__VAR = NULL;

#define TINY_STATE(__NAME, __STATE_NAME)    \
    void *tiny_fsm_state_##__STATE_NAME(tiny_fsm_##__NAME##_arg_t *pArg) 
#define BEGIN       {

#define END         }
#define PRIVATE     static
#define PUBLIC      
#define INTERNAL    static
#define PARAM       pArg


#define TINY_FSM_END    return NULL;
#define TINY_FSM_TRANSFER_TO(__STATE_NAME)     return (void *)&tiny_fsm_state_##__STATE_NAME;
#define IS_TINY_FSM_CPL(__VAR)      (NULL == s_TinyFSM##__VAR)

#define CALL_TINY_FSM(__NAME,__VAR, __START_STATE)       do {\
    tiny_fsm_##__NAME##_task *s_ptTinyFSMTemp = &s_TinyFSM##__VAR;\
    bool bReset = IS_TINY_FSM_CPL(__VAR);\
    tiny_fsm_##__NAME##_task s_TinyFSMStart = &(tiny_fsm_state_##__START_STATE);\
    static tiny_fsm_##__NAME##_arg_t tParam, tResetParam = 
#define PARAM_INIT  {
#define END_PARAM_INIT };\
    if (bReset) {\
        tParam = tResetParam;\
        *s_ptTinyFSMTemp = s_TinyFSMStart;\
    }

#define NO_PARAM_INIT   {0};\
    if (bReset) {\
        tParam = tResetParam;\
        *s_ptTinyFSMTemp = s_TinyFSMStart;\
    }
 
#define SET_PARAM(__FIELD,__VALUE) do {tParam.__FIELD = (__VALUE);} while(false);

#define END_CALL_TINY_FSM(__NAME)    \
        *s_ptTinyFSMTemp = (tiny_fsm_##__NAME##_task)(*s_ptTinyFSMTemp)( &tParam );\
    } while(false);

    

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
