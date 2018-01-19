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


#ifndef __USE_ARM_COMPILER_H__
#define __USE_ARM_COMPILER_H__

/*============================ INCLUDES ======================================*/
#if __IS_COMPILER_IAR__
#   include<intrinsics.h>
#endif

/*============================ MACROS ========================================*/

#ifndef DEF_REG
#define DEF_REG                     \
        union  {                    \
            struct {
#endif
    
#ifndef END_DEF_REG
#define END_DEF_REG(__NAME)         \
            };                      \
            reg32_t Value;          \
        }__NAME;
#endif

#ifndef __REG_MACRO__
#define __REG_MACRO__
#endif


#ifndef REG_RSVD_0x10
#define REG_RSVD_0x10                   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   \
    reg32_t                     : 32;   
#endif
#ifndef REG_RSVD_0x80       
#define REG_RSVD_0x80                   \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10               \
            REG_RSVD_0x10
#endif

#ifndef REG_RSVD_0x100                 
#define REG_RSVD_0x100                  \
            REG_RSVD_0x80               \
            REG_RSVD_0x80
#endif

#ifndef REG_RSVD_0x800
#define REG_RSVD_0x800                  \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100              \
            REG_RSVD_0x100
#endif

//! ALU integer width in byte
# define ATOM_INT_SIZE                   4

//! \brief The mcu memory align mode
# define MCU_MEM_ALIGN_SIZE             ATOM_INT_SIZE

#ifndef __volatile__
#define __volatile__
#endif

//! \brief 1 cycle nop operation
#ifndef NOP
    #define NOP()                       __asm__ __volatile__ ("nop");
#endif


//! \brief none standard memory types
#if __IS_COMPILER_IAR__
#   define FLASH                const
#   define EEPROM               const
#   define NO_INIT              __no_init
#   define ROOT                 __root
#   define INLINE               inline
#   define ALWAYS_INLINE        __attribute__((always_inline))
#   define WEAK                 __weak
#   define RAMFUNC              __ramfunc
#   define __asm__              __asm
#   define __ALIGN(__N)         _Pragma(__STR(data_alignment=__N))
#   define __AT_ADDR(__ADDR)    @ __ADDR
#   define __SECTION(__SEC)     _Pragma(__STR(location=__SEC))

#   define PACKED               __packed
#   define UNALIGNED            __packed
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#elif __IS_COMPILER_GCC__
#   define FLASH                const
#   define EEPROM               const
#   define NO_INIT              __attribute__(( section( ".bss.noinit")))
#   define ROOT                 __attribute__((used))    
#   define INLINE               inline
#   define ALWAYS_INLINE        __attribute__((always_inline))
#   define WEAK                 __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N)))
#   define __AT_ADDR(__ADDR)    __attribute__((at(__ADDR))) 
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))

#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __attribute__((packed))
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#elif __IS_COMPILER_ARM_COMPILER_5__
#   define FLASH                const
#   define EEPROM               const
#   define NO_INIT              __attribute__( ( section( ".bss.noinit"),zero_init) )
#   define ROOT                 __attribute__((used))    
#   define INLINE               __inline
#   define ALWAYS_INLINE        __attribute__((always_inline))
#   define WEAK                 __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N))) 
#   define __AT_ADDR(__ADDR)    __attribute__((at(__ADDR)))
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))


#   define PACKED               __packed
#   define UNALIGNED            __packed
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#elif __IS_COMPILER_ARM_COMPILER_6__
#   define FLASH                const
#   define EEPROM               const
#   define NO_INIT              __attribute__( ( section( ".bss.noinit")) )
#   define ROOT                 __attribute__((used))    
#   define INLINE               __inline
#   define ALWAYS_INLINE        __attribute__((always_inline))
#   define WEAK                 __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N))) 
#   define __AT_ADDR(__ADDR)    __attribute__((section (".ARM.__at_" #__ADDR)))
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))

#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __unaligned
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#endif


#define AT_ADDR(__ADDR)     __AT_ADDR(__ADDR)
#define ALIGN(__N)          __ALIGN(__N)
#define SECTION(__SEC)      __SECTION(__SEC)

/*----------------------------------------------------------------------------*
 * Signal & Interrupt Definition                                              *
 *----------------------------------------------------------------------------*/

  /*!< Macro to enable all interrupts. */
#if __IS_COMPILER_IAR__
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#elif __IS_COMPILER_ARM_COMPILER_5__ 
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_irq()
#elif __IS_COMPILER_ARM_COMPILER_6__
#   define ENABLE_GLOBAL_INTERRUPT()            __asm__ __volatile__ (" CPSIE i")
#else
#   define ENABLE_GLOBAL_INTERRUPT()            __asm__ __volatile__ (" CPSIE i")
#endif

  /*!< Macro to disable all interrupts. */
#if __IS_COMPILER_IAR__
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_interrupt()
#elif __IS_COMPILER_ARM_COMPILER_5__
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#elif __IS_COMPILER_ARM_COMPILER_6__
#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
____disable_irq(void) {
  unsigned int cpsr;

  __asm__ __volatile__("mrs %[cpsr], primask\n"
                       "cpsid i\n"
                       : [cpsr] "=r"(cpsr));
  return cpsr & 0x1;
}

/**
  \brief   Set Priority Mask
  \details Assigns the given value to the Priority Mask Register.
  \param [in]    priMask  Priority Mask
 */
__attribute__((always_inline)) static inline void ____set_PRIMASK(unsigned int priMask)
{
    __asm__ volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

#else
#   define DISABLE_GLOBAL_INTERRUPT()           __asm__ __volatile__ (" CPSID i");
#endif

#if __IS_COMPILER_IAR__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef __istate_t   istate_t;
#elif __IS_COMPILER_ARM_COMPILER_5__ 
#   define GET_GLOBAL_INTERRUPT_STATE()         __disable_irq()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  if (!__STATE) { __enable_irq(); }
typedef int   istate_t;
#elif __IS_COMPILER_ARM_COMPILER_6__
#   define GET_GLOBAL_INTERRUPT_STATE()         ____disable_irq()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  ____set_PRIMASK(__STATE)        
typedef int   istate_t;
#elif __IS_COMPILER_GCC__
typedef int   istate_t;
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_PRIMASK()

/**
  \brief   Get Priority Mask
  \details Returns the current state of the priority mask bit from the Priority Mask Register.
  \return               Priority Mask value
 */
__attribute__((always_inline)) static inline uint32_t __get_PRIMASK(void)
{
    unsigned int result;

    __asm__ volatile ("MRS %0, primask" : "=r" (result) );
    return(result);
}

#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_PRIMASK(__STATE)

/**
  \brief   Set Priority Mask
  \details Assigns the given value to the Priority Mask Register.
  \param [in]    priMask  Priority Mask
 */
__attribute__((always_inline)) static inline void __set_PRIMASK(unsigned int priMask)
{
    __asm__ volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}


#else
#error No support for interrupt state access
#endif

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

/*----------------------------------------------------------------------------*
 * Device Dependent Compiler Files                                            *
 *----------------------------------------------------------------------------*/
#if     defined(__CORTEX_M0__)
#include "cortex_m0_compiler.h"
#elif   defined(__CORTEX_M0P__)
#include "cortex_m0p_compiler.h"
#elif   defined(__CORTEX_M1__)
#include "cortex_m1_compiler.h"
#elif   defined(__CORTEX_M3__)
#include "cortex_m3_compiler.h"
#elif   defined(__CORTEX_M4__)
#include "cortex_m4_compiler.h"
#elif   defined(__CORTEX_M7__)
#include "cortex_m7_compiler.h"
#elif   defined(__CORTEX_M23__)
#include "cortex_m23_compiler.h"
#elif   defined(__CORTEX_M33__)
#include "cortex_m33_compiler.h"
#else
#error No supported compiler.h file!
#endif

/*!  \note using the ANSI-C99 standard type,if the file stdint.h dose not exit
 *!        you should define it all by yourself.
 *!
 */
#include ".\app_type.h"

//! \brief for interrupt 
#include ".\signal.h"

#endif
