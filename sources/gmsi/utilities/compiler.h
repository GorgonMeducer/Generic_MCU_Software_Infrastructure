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

/*============================ INCLUDES ======================================*/

#define __STR(__N)      #__N  
#define STR(__N)        __STR(__N)    

#include ".\error.h"
#include ".\preprocessor\mrepeat.h"
     
//! \brief CPU io

#if     defined(__CPU_AVR__)                    //!< Atmel AVR series
#   include ".\avr\avr_compiler.h"
#elif   defined(__CPU_ARM__)                //!< ARM series
#   include ".\arm\arm_compiler.h"
#else
//#warning No specified MCU type! use arm as default
#   include ".\arm\arm_compiler.h"
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

#ifndef UNUSED_PARAM
# define UNUSED_PARAM(__VAL)    (__VAL) = (__VAL)
#endif
     
//! \brief This macro convert variable types between different datatypes.
#define __TYPE_CONVERT(__ADDR,__TYPE)       (*((__TYPE *)(__ADDR)))
#define TYPE_CONVERT(__ADDR, __TYPE)        __TYPE_CONVERT((__ADDR), __TYPE)

//! \brief initialize large object
# define OBJECT_INIT_ZERO(__OBJECT)                                             \
            do                                                                  \
            {                                                                   \
                memset((void *)&(__OBJECT), 0, sizeof(__OBJECT));               \
            } while (0)


/*!  \note using the ANSI-C99 standard type,if the file stdint.h dose not exit
 *!        you should define it all by yourself.
 *!
 */
#include ".\app_type.h"

#include ".\ooc.h"

#endif
