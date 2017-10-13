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

#ifndef __USE_AVR_COMPILER_H__
#define __USE_AVR_COMPILER_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#ifndef NOP
    #define NOP()               __asm__ __volatile__ ("nop");
#endif


//! ALU integer width in byte
# define ATOM_INT_SIZE           1

//! \brief The mcu memory align mode
# define MCU_MEM_ALIGN_SIZE     ATOM_INT_SIZE


//! \brief The mcu memory endian mode
# define __BIG_ENDIAN__         false


//! \brief none standard memory types
#if __IS_COMPILER_IAR__
    # define FLASH		__flash
    # define EEPROM     __eeprom
    # define NO_INIT    __no_init
    # define ROOT       __root
    # define RAM
#elif __IS_COMPILER_GCC__
    # define FLASH		const
    # define EEPROM     const
    # define NO_INIT
    # define ROOT       
    # define RAM
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

//! \brief include I/O head file
#if __IS_COMPILER_ICC__
    #include <iccioavr.h>
#elif __IS_COMPILER_GCC__
    #include <avr/io.h>
    #include <avr/pgmspace.h>
#elif __IS_COMPILER_IAR__
#ifndef _USE_IGNORE_IAR_IOAVR_H_
    #ifndef _DEVICE_IO
    #define _DEVICE_IO
        #include <ioavr.h>
    #endif
    #include<intrinsics.h>
#endif
#endif


/*!  \note using the ANSI-C99 standard type,if the file stdint.h dose not exit
 *!        you should define it all by yourself.
 *!
 */
#include ".\app_type.h"

//! \brief for interrupt 
#include "signal.h"

#endif