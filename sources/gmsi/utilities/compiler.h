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

#ifndef _USE_COMPILER_H_
#define _USE_COMPILER_H_            

//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
    #undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#define __IS_COMPILER_IAR__                 1
#endif

//! \note for gcc
#ifdef __IS_COMPILER_GCC__
    #undef __IS_COMPILER_GCC__
#endif
#if defined(__GNUC__)
#define __IS_COMPILER_GCC__                 1
#endif
//! @}

//! \note for arm compiler 5
#ifdef __IS_COMPILER_ARM_COMPILER_5__
    #undef __IS_COMPILER_ARM_COMPILER_5__
#endif
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#define __IS_COMPILER_ARM_COMPILER_5__      1
#endif
//! @}

//! \note for arm compiler 6
#ifdef __IS_COMPILER_ARM_COMPILER_6__
    #undef __IS_COMPILER_ARM_COMPILER_6__
#endif
#if ((__ARMCC_VERSION >= 6000000) && (__ARMCC_VERSION < 7000000))
#define __IS_COMPILER_ARM_COMPILER_6__      1
#endif
//! @}


/* -------------------  Start of section using anonymous unions  ------------------ */
#if     __IS_COMPILER_ARM_COMPILER_5__
    //#pragma push
    #pragma anon_unions
#elif   __IS_COMPILER_IAR__
    #pragma language=extended
#elif   __IS_COMPILER_GCC__ || __IS_COMPILER_ARM_COMPILER_6__
    /* anonymous unions are enabled by default */
#else
    #warning Not supported compiler type
#endif

#define __STR(__N)      #__N  
#define STR(__N)        __STR(__N)    

#include ".\error.h"
#include ".\preprocessor\mrepeat.h"
     
//! \brief CPU io
#if     defined(__CPU_ARM__)                    //!< ARM series
    #include ".\arm\arm_compiler.h"
#elif   defined(__CPU_AVR__)                    //!< Atmel AVR series
    #include ".\avr\avr_compiler.h"
#else
#error No specified MCU type!
#endif
   
#ifndef ATOM_INT_SIZE
#define ATOM_INT_SIZE           2
#endif 
     

//! \brief system macros
#define MAX(__A,__B)  (((__A) > (__B)) ? (__A) : (__B))
#define MIN(__A,__B)  (((__A) < (__B)) ? (__A) : (__B))

#define UBOUND(__ARRAY) (sizeof(__ARRAY)/sizeof(__ARRAY[0]))

#define ABS(__NUM)    (((__NUM) < 0)?(-(__NUM)):(__NUM))

#ifndef BIT
#define BIT(__N)		((uint32_t)1 << (__N))
#endif
#ifndef _BV
#define _BV(__N)        ((uint32_t)1 << (__N))
#endif
#ifndef _BM
#define __MASK(__N)     (_BV(__N) - 1)
#define _BM(__FROM,__TO)\
                        (__MASK((__TO)+1)-__MASK(__FROM))
#endif

#ifndef __CONNECT
#	define __CONNECT(a, b)			a ## b
#endif

#ifndef REFERENCE_PARAMETER
# define REFERENCE_PARAMETER(a)		(a) = (a)
#endif

#ifndef dimof
#	define dimof(arr)				(sizeof(arr) / sizeof((arr)[0]))
#endif
     
     
//! \brief This macro convert variable types between different datatypes.
#define __TYPE_CONVERT(__ADDR,__TYPE)       (*((__TYPE *)(__ADDR)))
#define TYPE_CONVERT(__ADDR, __TYPE)        __TYPE_CONVERT((__ADDR), __TYPE)

//! \brief initialize large object
# define OBJECT_INIT_ZERO(__OBJECT) \
            do\
            {\
                struct OBJECT_INIT\
                {\
                    uint8_t StructMask[sizeof(__OBJECT)];\
                }NULL_OBJECT = {{0}};\
                \
                (*((struct OBJECT_INIT *)&(__OBJECT))) = NULL_OBJECT;\
            }\
            while (false)


#include ".\ooc.h"

#endif
