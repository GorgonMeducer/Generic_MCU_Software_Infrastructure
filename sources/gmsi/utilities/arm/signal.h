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

#ifndef _USE_SIGNAL_H_
#define _USE_SIGNAL_H_

/*============================ INCLUDES ======================================*/
#include <stdbool.h>

/*============================ MACROS ========================================*/

#if __IS_COMPILER_ARM_COMPILER_5__ || __IS_COMPILER_ARM_COMPILER_6__
//! \brief The safe ATOM code section macro
# define SAFE_ATOM_CODE(...)     {\
        istate_t tState = GET_GLOBAL_INTERRUPT_STATE();\
        __VA_ARGS__;\
        SET_GLOBAL_INTERRUPT_STATE(tState);\
    }

//! \brief Exit from the safe atom operations
# define EXIT_SAFE_ATOM_CODE()          SET_GLOBAL_INTERRUPT_STATE(tState);
#else
//! \brief The safe ATOM code section macro
# define SAFE_ATOM_CODE(...)     {\
        istate_t tState = GET_GLOBAL_INTERRUPT_STATE();\
        DISABLE_GLOBAL_INTERRUPT();\
        __VA_ARGS__;\
        SET_GLOBAL_INTERRUPT_STATE(tState);\
    }

//! \brief Exit from the safe atom operations
# define EXIT_SAFE_ATOM_CODE()          SET_GLOBAL_INTERRUPT_STATE(tState);
#endif

//! \brief ATOM code section macro
# define ATOM_CODE(...)      {\
                DISABLE_GLOBAL_INTERRUPT();\
                __VA_ARGS__;\
                ENABLE_GLOBAL_INTERRUPT();\
            }

//! \brief Exit from the atom operations
# define EXIT_ATOM_CODE()   ENABLE_GLOBAL_INTERRUPT();

//! \name ES_LOCKER value
//! @{
#define LOCKED          true            //!< locked
#define UNLOCKED        false           //!< unlocked
//! @}

//! \note critical code section protection
//! \note LOCKER could be only used among FSMs and there should be no ISR involved.
//! \param __LOCKER ES_LOCKER variable
//! \param __CODE   target code segment
#define LOCK(__LOCKER,...)  \
            {\
                istate_t tState = GET_GLOBAL_INTERRUPT_STATE();\
                locker_t *pLocker = &(__LOCKER);\
                DISABLE_GLOBAL_INTERRUPT();\
                if (!(*pLocker)) {\
                    (*pLocker) = LOCKED;\
                    ENABLE_GLOBAL_INTERRUPT();\
                    __VA_ARGS__;\
                    (*pLocker) = UNLOCKED;\
                }\
                SET_GLOBAL_INTERRUPT_STATE(tState);\
            }


#define LOCK_INIT(__LOCKER)     do {(__LOCKER) = UNLOCKED;}while(false)


#define EXIT_LOCK()     do {\
                            (*pLocker) = UNLOCKED;\
                            SET_GLOBAL_INTERRUPT_STATE(tState);\
                        } while(false)

#define ENTER_LOCK(__LOCKER)            enter_lock(__LOCKER)

#define LEAVE_LOCK(__LOCKER)            leave_lock(__LOCKER)
                        
#define GET_LOCK_STATUS(__LOCKER)       check_lock(__LOCKER)

#define INIT_LOCK(__LOCKER)             init_lock(__LOCKER)
                        
//! \brief exit lock checker structure
#define EXIT_LOCK_CHECKER() EXIT_SAFE_ATOM_CODE()

/*! \note check specified locker and run code segment
 *! \param __LOCKER a ES_LOCKER variable
 *! \param __CODE target code segment
 */
#define LOCK_CHECKER(__LOCKER, ...)  {\
                SAFE_ATOM_CODE  (\
                    if (UNLOCKED == (__LOCKER)) {\
                        __VA_ARGS__;\
                    }\
                )\
            }
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef volatile bool locker_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief try to enter a section
 *! \param ptLock locker object
 *! \retval lock section is entered
 *! \retval The section is locked
 */
extern bool enter_lock(locker_t *ptLock);

/*! \brief leave a section
 *! \param ptLock locker object
 *! \return none
 */
extern void leave_lock(locker_t *ptLock);
            
/*! \brief get locker status
 *! \param ptLock locker object
 *! \return locker status
 */
extern bool check_lock(locker_t *ptLock);            
           
/*! \brief initialize a locker
 *! \param ptLock locker object
 *! \return none
 */
extern void init_lock(locker_t *ptLock);
#endif
