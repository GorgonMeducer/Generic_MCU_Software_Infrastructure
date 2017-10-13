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
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
//! \brief interrupt / signal
#if __IS_COMPILER_GCC__
    #include <avr/interrupt.h>

    #define REGISTER_ISR(__VECTOR, __ISR)       \
            extern void __ISR(void);\
            ISR(__VECTOR)\
            {\
                __ISR();\
            }

#elif __IS_COMPILER_IAR__
    #ifndef PRAGMA
        #define PRAGMA(__STRING)   _Pragma(#__STRING)
    #endif
    #ifndef ISR
        #define ISR(num) PRAGMA( vector = num ) __interrupt void isr_##num(void)
    #endif

    #define REGISTER_ISR(__VECTOR, __ISR)       \
            extern __task void __ISR(void);\
            PRAGMA( vector = __VECTOR ) __interrupt void isr_##__VECTOR(void)\
            {\
                __ISR();\
            }
#endif

#define ENABLE_GLOBAL_INTERRUPT()      __asm__ __volatile__ ("sei")
#define DISABLE_GLOBAL_INTERRUPT()     __asm__ __volatile__ ("cli")

#define GLOBAL_INTERRUPT_ENABLED()     ((0 != (SREG & 0x80)) ? true : false)

            
//! \brief The safe ATOM code section macro
# define SAFE_ATOM_CODE(__CODE)     {\
        volatile uint8_t chSREG = SREG;\
        DISABLE_GLOBAL_INTERRUPT();\
        __CODE;\
        SREG = chSREG;\
    }

//! \brief Exit from the safe atom operations
# define EXIT_SAFE_ATOM_CODE()          SREG = chSREG;\

//! \brief ATOM code section macro
# define ATOM_CODE(__CODE)      {\
                DISABLE_GLOBAL_INTERRUPT();\
                __CODE;\
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
//! \param __LOCKER ES_LOCKER variable
//! \param __CODE   target code segment
#define LOCK(__LOCKER, __CODE)  \
            {\
                volatile uint8_t chSREG = SREG;\
                locker_t *pLocker = &(__LOCKER);\
                DISABLE_GLOBAL_INTERRUPT();\
                if (!(*pLocker)) {\
                    (*pLocker) = LOCKED;\
                    __CODE;\
                    (*pLocker) = UNLOCKED;\
                }\
                SREG = chSREG;\
            }


#define LOCK_INIT(__LOCKER)     do {(__LOCKER) = UNLOCKED;}while(false)


#define EXIT_LOCK()    do {\
        (*pLocker) = UNLOCKED;\
        SREG = chSREG;\
    } while(false)

//! \brief exit lock checker structure
#define EXIT_LOCK_CHECKER() EXIT_SAFE_ATOM_CODE()

/*! \note check specified locker and run code segment
 *! \param __LOCKER a ES_LOCKER variable
 *! \param __CODE target code segment
 */
#define LOCK_CHECKER(__LOCKER, __CODE)  {\
                SAFE_ATOM_CODE  (\
                    if (UNLOCKED == (__LOCKER)) {\
                        __CODE;\
                    }\
                )\
            }

/*============================ TYPES =========================================*/
typedef volatile bool locker_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif
