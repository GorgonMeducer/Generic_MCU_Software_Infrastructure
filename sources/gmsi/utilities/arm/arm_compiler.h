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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef __USE_COMPILER_SPECIFIED_CMSIS_PATH__
#   include "CMSIS/Core/Include/cmsis_compiler.h"
#else
#   include "cmsis_compiler.h"
#endif

//! \name The macros to identify the compiler
//! @{

//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                 1
#endif




//! \note for arm compiler 5
#ifdef __IS_COMPILER_ARM_COMPILER_5__
#   undef __IS_COMPILER_ARM_COMPILER_5__
#endif
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__      1
#endif
//! @}

//! \note for arm compiler 6
#ifdef __IS_COMPILER_ARM_COMPILER_6__
#   undef __IS_COMPILER_ARM_COMPILER_6__
#endif
#if ((__ARMCC_VERSION >= 6000000) && (__ARMCC_VERSION < 7000000))
#   define __IS_COMPILER_ARM_COMPILER_6__      1
#endif

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                1
#else
//! \note for gcc
#ifdef __IS_COMPILER_GCC__
#   undef __IS_COMPILER_GCC__
#endif
#if defined(__GNUC__) && !(__IS_COMPILER_ARM_COMPILER_6__ || __IS_COMPILER_LLVM__)
#   define __IS_COMPILER_GCC__                 1
#endif
//! @}
#endif
//! @}

#if __IS_COMPILER_IAR__
#   include <intrinsics.h>
#endif


/* -----------------  Start of section using anonymous unions  -------------- */
#if __IS_COMPILER_ARM_COMPILER_5__
  //#pragma push
  #pragma anon_unions
#elif __IS_COMPILER_ARM_COMPILER_6__
#elif __IS_COMPILER_IAR__
  #pragma language=extended
#elif __IS_COMPILER_GCC__
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/*----------------------------------------------------------------------------*
 * Warning Mitigation                                                         *
 *----------------------------------------------------------------------------*/
#if defined(__clang__) //__IS_COMPILER_LLVM__
#pragma clang diagnostic ignored "-Wmissing-declarations"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wmicrosoft-anon-tag"
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wconstant-conversion"
#pragma clang diagnostic ignored "-Wmicrosoft-enum-forward-reference"
#pragma clang diagnostic ignored "-Wbuiltin-requires-header"
#pragma clang diagnostic ignored "-Winitializer-overrides" 
#pragma clang diagnostic ignored "-Wbraced-scalar-init" 
#pragma clang diagnostic ignored "-Wempty-body" 
#pragma clang diagnostic ignored "-Wgnu-empty-struct"
#pragma clang diagnostic ignored "-Wint-conversion" 
#pragma clang diagnostic ignored "-Wint-to-pointer-cast" 
#pragma clang diagnostic ignored "-Wmicrosoft-include"
#pragma clang diagnostic ignored "-Wpragma-pack" 
#pragma clang diagnostic ignored "-Wunused-function" 
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wembedded-directive"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wduplicate-enum"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wzero-length-array"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wnonportable-include-path"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wc++-compat"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wbad-function-cast"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"

/*! \NOTE do not ignore following warning unless you take the risk by yourself */
//#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
//#pragma clang diagnostic ignored "-Wpointer-integer-compare" 
//#pragma clang diagnostic ignored "-Wunused-value" 
//#pragma clang diagnostic ignored "-Wno-sometimes-uninitialized" 
//#pragma clang diagnostic ignored "-Wdeprecated-declarations" 
//#pragma clang diagnostic ignored "-Wunused-variable"  

#elif __IS_COMPILER_GCC__
// TODO: validate diagnositc ignored for GCC
#pragma gcc diagnostic ignored "-Wmissing-declarations"
#pragma gcc diagnostic ignored "-Wdeprecated-declarations"
#pragma gcc diagnostic ignored "-Wmicrosoft-anon-tag"
#pragma gcc diagnostic ignored "-Wmissing-braces"
#pragma gcc diagnostic ignored "-Wconstant-conversion"
#pragma gcc diagnostic ignored "-Wmicrosoft-enum-forward-reference"
#pragma gcc diagnostic ignored "-Wbuiltin-requires-header"
#pragma gcc diagnostic ignored "-Winitializer-overrides" 
#pragma gcc diagnostic ignored "-Wbraced-scalar-init" 
#pragma gcc diagnostic ignored "-Wempty-body" 
#pragma gcc diagnostic ignored "-Wgnu-empty-struct"
#pragma gcc diagnostic ignored "-Wint-conversion" 
#pragma gcc diagnostic ignored "-Wint-to-pointer-cast" 
#pragma gcc diagnostic ignored "-Wmicrosoft-include"
#pragma gcc diagnostic ignored "-Wpragma-pack" 
#pragma gcc diagnostic ignored "-Wunused-function" 
#pragma gcc diagnostic ignored "-Wswitch"
#pragma gcc diagnostic ignored "-Wembedded-directive"
#pragma gcc diagnostic ignored "-Wundef"
#pragma gcc diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma gcc diagnostic ignored "-Wpadded"
#pragma gcc diagnostic ignored "-Wnewline-eof"
#pragma gcc diagnostic ignored "-Wduplicate-enum"
#pragma gcc diagnostic ignored "-Wextra-semi"
#pragma gcc diagnostic ignored "-Wextra-semi-stmt"
#pragma gcc diagnostic ignored "-Wzero-length-array"
#pragma gcc diagnostic ignored "-Wmissing-prototypes"
#pragma gcc diagnostic ignored "-Wimplicit-int-conversion"
#pragma gcc diagnostic ignored "-Wnonportable-include-path"
#pragma gcc diagnostic ignored "-Wmissing-variable-declarations"
#pragma gcc diagnostic ignored "-Wc++-compat"
#pragma gcc diagnostic ignored "-Wsign-conversion"
#pragma gcc diagnostic ignored "-Wimplicit-fallthrough"
#pragma gcc diagnostic ignored "-Wmissing-noreturn"
#pragma gcc diagnostic ignored "-Wcast-qual"
#pragma gcc diagnostic ignored "-Wbad-function-cast"
#pragma gcc diagnostic ignored "-Wswitch-enum"
#pragma gcc diagnostic ignored "-Wdisabled-macro-expansion"
#pragma gcc diagnostic ignored "-Wmissing-field-initializers"
#pragma gcc diagnostic ignored "-Wtautological-pointer-compare"

/*! \NOTE do not ignore following warning unless you take the risk by yourself */
//#pragma gcc diagnostic ignored "-Wbitfield-constant-conversion"
//#pragma gcc diagnostic ignored "-Wpointer-integer-compare" 
//#pragma gcc diagnostic ignored "-Wunused-value" 
//#pragma gcc diagnostic ignored "-Wno-sometimes-uninitialized" 
//#pragma gcc diagnostic ignored "-Wdeprecated-declarations" 
//#pragma gcc diagnostic ignored "-Wunused-variable"  

#elif __IS_COMPILER_IAR__

//! undefined behavior: the order of volatile accesses is undefined in this statement 
#pragma diag_suppress=Pa082

//! Typedef name has already been declared (with same type)
#pragma diag_suppress=pe301

//! enumerated type mixed with another type
#pragma diag_suppress=pe188

//! extra ";" ignored
#pragma diag_suppress=pe381

//! enumerated type mixed with another enumerated type 
#pragma diag_suppress=pa089

//! use of address of unaligned structure member 
#pragma diag_suppress=pa039

//,Pe186,Pe111,,pe128,,,Pe1866,Pe064,Pa039

#endif

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif


/*----------------------------------------------------------------------------*
 * Warning Emphasize                                                          *
 *----------------------------------------------------------------------------*/
#if defined(__clang__) //__IS_COMPILER_LLVM__
#pragma clang diagnostic warning "-Wcast-align"
#elif __IS_COMPILER_GCC__

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


//! \brief The mcu memory align mode
# define MCU_MEM_ALIGN_SIZE             sizeof(int)

#ifndef __volatile__
#define __volatile__                    volatile
#endif

//! \brief 1 cycle nop operation
#ifndef NOP
    #define NOP()                       __asm__ __volatile__ ("nop");
#endif


//! \brief none standard memory types
#if __IS_COMPILER_IAR__
#   define ROM_FLASH            _Pragma(__STR(location=".rom.flash")) const
#   define ROM_EEPROM           _Pragma(__STR(location=".rom.eeprom")) const
#   define NO_INIT              __no_init
#   define ROOT                 __root
#   define INLINE               inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        inline __attribute__((always_inline))
#   define WEAK                 __weak
#   define RAMFUNC              __ramfunc
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N)))
#   define __AT_ADDR(__ADDR)    @ __ADDR
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                _Pragma(__STR(weak __ORIGIN=__ALIAS))
#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __attribute__((packed))
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#elif __IS_COMPILER_ARM_COMPILER_5__
#   define ROM_FLASH            __attribute__(( section( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( section( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__( ( section( ".bss.noinit"),zero_init) )
#   define ROOT                 __attribute__((used))    
#   define INLINE               __inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        __inline __attribute__((always_inline))
#   define WEAK                 __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N))) 
#   define __AT_ADDR(__ADDR)    __attribute__((at(__ADDR)))
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                __attribute__((weakref(__STR(__ALIAS))))
                                
#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __attribute__((packed))
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#elif __IS_COMPILER_ARM_COMPILER_6__
#   define ROM_FLASH            __attribute__(( section( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( section( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__( ( section( ".bss.noinit")) )
#   define ROOT                 __attribute__((used))    
#   define INLINE               __inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        __inline __attribute__((always_inline))
#   define WEAK                 __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N))) 
#   define __AT_ADDR(__ADDR)    __attribute__((section (".ARM.__at_" #__ADDR)))
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                __attribute__((weakref(__STR(__ALIAS))))

#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __unaligned
#   define TRANSPARENT_UNION    __attribute__((transparent_union))
#elif __IS_COMPILER_LLVM__

#   define ROM_FLASH            __attribute__(( __section__( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( __section__( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__(( __section__( ".bss.noinit")))
#   define ROOT                 __used    
#   define INLINE               inline
#   define NO_INLINE            __noinline
#   define ALWAYS_INLINE        __always_inline
#   define WEAK                 __weak_symbol
#   define RAMFUNC              __attribute__((__section__ (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __aligned(__N)
#   define __AT_ADDR(__ADDR)    __section(".ARM.__at_" #__ADDR) 
#   define __SECTION(__SEC)     __section(__SEC)
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS)                                      \
                                __weak_reference(__ORIGIN,__ALIAS)

#   define PACKED               __packed
#   define UNALIGNED            __packed
#   define TRANSPARENT_UNION    __attribute__((__transparent_union__))

#else  /*__IS_COMPILER_GCC__: Using GCC as default for those GCC compliant compilers*/
#   define ROM_FLASH            __attribute__(( section( ".rom.flash"))) const
#   define ROM_EEPROM           __attribute__(( section( ".rom.eeprom"))) const
#   define NO_INIT              __attribute__(( section( ".bss.noinit")))
#   define ROOT                 __attribute__((used))    
#   define INLINE               inline
#   define NO_INLINE            __attribute__((noinline))
#   define ALWAYS_INLINE        inline __attribute__((always_inline))
#   define WEAK                 __attribute__((weak))
#   define RAMFUNC              __attribute__((section (".textrw")))
#   define __asm__              __asm
#   define __ALIGN(__N)         __attribute__((aligned (__N)))
#   define __AT_ADDR(__ADDR)    __section(".ARM.__at_" #__ADDR) 
#   define __SECTION(__SEC)     __attribute__((section (__SEC)))
#   define __WEAK_ALIAS(__ORIGIN, __ALIAS) \
                                __attribute__((weakref(__STR(__ALIAS))))

#   define PACKED               __attribute__((packed))
#   define UNALIGNED            __attribute__((packed))
#   define TRANSPARENT_UNION    __attribute__((transparent_union))

#endif

#define WEAK_ALIAS(__ORIGIN, __ALIAS)                                           \
                            __WEAK_ALIAS(__ORIGIN, __ALIAS)
#define AT_ADDR(__ADDR)     __AT_ADDR(__ADDR)
#define ALIGN(__N)          __ALIGN(__N)
#define SECTION(__SEC)      __SECTION(__SEC)

/*----------------------------------------------------------------------------*
 * Signal & Interrupt Definition                                              *
 *----------------------------------------------------------------------------*/

  /*!< Macro to enable all interrupts. */
#if __IS_COMPILER_IAR__
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_interrupt()
#else
#   define ENABLE_GLOBAL_INTERRUPT()            __enable_irq()
#endif

  /*!< Macro to disable all interrupts. */
#if __IS_COMPILER_IAR__
#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

static ALWAYS_INLINE uint32_t ____disable_irq(void) 
{
    uint32_t wPRIMASK = __get_interrupt_state();
    __disable_irq();
    return wPRIMASK & 0x1;
}

#elif __IS_COMPILER_ARM_COMPILER_5__
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#elif __IS_COMPILER_ARM_COMPILER_6__ && !defined(__CMSIS_ARMCC_V6_H)
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#elif __IS_COMPILER_GCC_
#   define DISABLE_GLOBAL_INTERRUPT()           __disable_irq()
#else /* for other compilers, using gcc assembly syntax to implement */

#   define DISABLE_GLOBAL_INTERRUPT()           ____disable_irq()

static ALWAYS_INLINE uint32_t ____disable_irq(void) 
{
    uint32_t cpsr;

    __asm__ __volatile__("mrs %[cpsr], primask\n"
                        "cpsid i\n"
                        : [cpsr] "=r"(cpsr));
    return cpsr & 0x1;
}
#endif

#if __IS_COMPILER_IAR__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_interrupt_state()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_interrupt_state(__STATE)
typedef __istate_t   istate_t;
#elif __IS_COMPILER_ARM_COMPILER_5__ || __IS_COMPILER_ARM_COMPILER_6__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_PRIMASK()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_PRIMASK(__STATE)
typedef int   istate_t;
#elif __IS_COMPILER_GCC__
#   define GET_GLOBAL_INTERRUPT_STATE()         __get_PRIMASK()
#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  __set_PRIMASK(__STATE)
typedef uint32_t   istate_t;
#else
typedef uint32_t   istate_t;
#   define GET_GLOBAL_INTERRUPT_STATE()         ____get_PRIMASK()

/**
  \brief   Get Priority Mask
  \details Returns the current state of the priority mask bit from the Priority Mask Register.
  \return               Priority Mask value
 */
__attribute__((always_inline)) static inline uint32_t ____get_PRIMASK(void)
{
    unsigned int result;

    __asm__ volatile ("MRS %0, primask" : "=r" (result) );
    return(result);
}

#   define SET_GLOBAL_INTERRUPT_STATE(__STATE)  ____set_PRIMASK(__STATE)

/**
  \brief   Set Priority Mask
  \details Assigns the given value to the Priority Mask Register.
  \param [in]    priMask  Priority Mask
 */
__attribute__((always_inline)) static inline void ____set_PRIMASK(uint32_t priMask)
{
    __asm__ volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}
#endif

/*----------------------------------------------------------------------------*
 * Startup Source Code                                                        *
 *----------------------------------------------------------------------------*/
#if     __IS_COMPILER_IAR__
#ifndef __VECTOR_TABLE
#   define __VECTOR_TABLE               __vector_table
#endif
#ifndef __VECTOR_TABLE_ATTRIBUTE
#   define __VECTOR_TABLE_ATTRIBUTE     @".intvec"
#endif
#ifndef __PROGRAM_START
#   define __PROGRAM_START              __iar_program_start
#endif
#ifndef __INITIAL_SP
#   define __INITIAL_SP                 CSTACK$$Limit
#endif
#elif   __IS_COMPILER_ARM_COMPILER_6__ || __IS_COMPILER_ARM_COMPILER_5__
#ifndef __VECTOR_TABLE
#   define __VECTOR_TABLE               __Vectors
#endif
#ifndef __VECTOR_TABLE_ATTRIBUTE
#   define __VECTOR_TABLE_ATTRIBUTE     ROOT SECTION("RESET")
#endif
#ifndef __PROGRAM_START
#   define __PROGRAM_START              __main
#endif
#ifndef __INITIAL_SP
#   define __INITIAL_SP                 Image$$ARM_LIB_STACK$$ZI$$Limit
#endif
#elif   //__IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
#   error Unsupported compiler detected. Please contact vsf team for support.
#endif

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

//! \brief for interrupt 
#include "./signal.h"

/*============================ PROTOTYPES ====================================*/

#endif
