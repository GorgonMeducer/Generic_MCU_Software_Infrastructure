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

#ifndef __OBJECT_ORIENTED_C_H__
#define __OBJECT_ORIENTED_C_H__

/*============================ INCLUDES ======================================*/
#include <stdint.h>


/*! \NOTE the uint_fast8_t used in this header file is defined in stdint.h 
          if you don't have stdint.h supported in your toolchain, you should
          define uint_fast8_t all by yourself with following rule:
          a. if the target processor is 8 bits, define it as uint8_t
          b. if the target processor is 16 bits, define it as uint16_t 
          c. if the target processor is 32 bits, define it as uint32_t
          d. if the target processor is 64 bits, define it as either uint32_t or uint64_t
 */

/*============================ MACROS ========================================*/
#define EVENT_RT_UNREGISTER         4


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


#ifndef this
#   define this         (*ptThis)
#endif
#ifndef base
#   define base         (*ptBase)
#endif
#ifndef target
#   define target       (*ptTarget)
#endif

#ifndef private
#   define private    static
#endif

#ifndef public
#   define public      
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __CLASS(__NAME)             __##__NAME
#define CLASS(__NAME)               __CLASS(__NAME)


//! \brief macro for initializing an event
#define INIT_DELEGATE(__NAME/*,__ASYN*/)           delegate_init(&(__NAME)/*, (__ASYN)*/)

/*! \note add WHICH macro to support multiple inheriting and implementations
 *!       DEF_CLASS( example_base_t )
 *!           ...
 *!       END_DEF_CLASS( example_base_t )
 *!
 *!       DEF_INTERFACE( i_example_t )
 *!           ...
 *!       END_INTERFACE( i_example_t )
 *!     
 *!       DEF_CLASS( example_t, WHICH( INHERIT( example_base_t ) IMPLEMENT( i_example_t ) ) )
 *!           ...
 *!       END_DEF_CLASS( example_t, WHICH( INHERIT( example_base_t ) IMPLEMENT( i_example_t ) ) )
  */
#define WHICH(...)                  struct { __VA_ARGS__ };


#define DECLARE_CLASS(__NAME)                                                   \
     typedef union __NAME __NAME;                

#define __DEF_CLASS(__NAME,...)                                                 \
    typedef struct __##__NAME __##__NAME;                                       \
    struct __##__NAME {                                                         \
        __VA_ARGS__
#define DEF_CLASS(__NAME, ...)      __DEF_CLASS(__NAME, __VA_ARGS__)

          
#define __END_DEF_CLASS(__NAME, ...)                                            \
    };                                                                          \
    union __NAME {                                                              \
        __VA_ARGS__                                                             \
        uint_fast8_t __NAME##__chMask[                                          \
                (sizeof(__##__NAME) + sizeof(uint_fast8_t) - 1)                 \
            /   sizeof(uint_fast8_t)];                                          \
    };
#define END_DEF_CLASS(__NAME, ...)  __END_DEF_CLASS(__NAME, __VA_ARGS__)

/*! \brief macro for initializing class in compiler-time
 *! \param __TYPE class name
 *! \param __OBJ target object
 *! \param ... initialization list
 */
#define __INIT_CLASS_OBJ(__TYPE, __OBJ, ...)                                    \
            union {                                                             \
                CLASS(__TYPE) __##__OBJ;                                        \
                __TYPE;                                                         \
            } __OBJ = {                                                         \
                .__##__OBJ = __VA_ARGS__                                        \
            }
#define INIT_CLASS_OBJ(__TYPE, __OBJ, ...)                                      \
            __INIT_CLASS_OBJ(__TYPE, __OBJ, __VA_ARGS__)

#define __EXTERN_CLASS_OBJ( __TYPE, __OBJ )                                     \
            extern union {                                                      \
                CLASS(__TYPE) __##__OBJ;                                        \
                __TYPE;                                                         \
            }__OBJ;
#define EXTERN_CLASS_OBJ(__TYPE, __OBJ)                                         \
            __EXTERN_CLASS_OBJ( __TYPE, __OBJ )


#define __EXTERN_CLASS(__NAME,...)                                              \
    union __NAME {                                                              \
        __VA_ARGS__                                                             \
        uint_fast8_t __NAME##__chMask[(sizeof(struct{                           \
        __VA_ARGS__
#define EXTERN_CLASS(__NAME, ...)   __EXTERN_CLASS(__NAME, __VA_ARGS__)

#define END_EXTERN_CLASS(__NAME, ...)                                           \
        }) + sizeof(uint_fast8_t) - 1) / sizeof(uint_fast8_t)];                 \
    };

/*! \note Support for protected members
 */
//! @{
#define __DECLARE_PROTECTED(__BELONGS_TO)           DECLARE_CLASS           (__p_##__BELONGS_TO)
#define __DEF_PROTECTED(__BELONGS_TO, ...)          DEF_CLASS               (__p_##__BELONGS_TO, __VA_ARGS__ )
#define __END_DEF_PROTECTED(__BELONGS_TO, ...)      END_DEF_CLASS           (__p_##__BELONGS_TO, __VA_ARGS__ )
#define __EXTERN_PROTECTED(__BELONGS_TO, ...)       EXTERN_CLASS            (__p_##__BELONGS_TO, __VA_ARGS__ )
#define __END_EXTERN_PROTECTED(__BELONGS_TO, ...)   END_EXTERN_CLASS        (__p_##__BELONGS_TO, __VA_ARGS__ )
#define __PROTECTED(__BELONGS_TO)                   __p_##__BELONGS_TO
#define __PROTECTED_CONTENT(__BELONGS_TO)           CLASS                   (__p_##__BELONGS_TO)

#define DECLARE_PROTECTED(__BELONGS_TO)             __DECLARE_PROTECTED     (__BELONGS_TO) 
#define DEF_PROTECTED(__BELONGS_TO, ...)            __DEF_PROTECTED         (__BELONGS_TO, __VA_ARGS__ )
#define END_DEF_PROTECTED(__BELONGS_TO, ...)        __END_DEF_PROTECTED     (__BELONGS_TO, __VA_ARGS__ )
#define EXTERN_PROTECTED(__BELONGS_TO, ...)         __EXTERN_PROTECTED      (__BELONGS_TO, __VA_ARGS__ )
#define END_EXTERN_PROTECTED(__BELONGS_TO, ...)     __END_EXTERN_PROTECTED  (__BELONGS_TO, __VA_ARGS__ )
#define PROTECTED(__BELONGS_TO)                     __PROTECTED             (__BELONGS_TO)
#define PROTECTED_CONTENT(__BELONGS_TO)             __PROTECTED_CONTENT     (__BELONGS_TO)
//! @}

//! \name interface definition
//! @{
#define DEF_INTERFACE(__NAME,...)                                               \
            typedef struct __NAME __NAME;                                       \
            __VA_ARGS__                                                         \
            struct __NAME {

#define END_DEF_INTERFACE(__NAME)                                               \
            };
//! @}

//! \name structure definition
//! @{
#define DEF_STRUCTURE(__NAME,...)                                               \
            typedef struct __NAME __NAME;                                       \
            __VA_ARGS__                                                         \
            struct __NAME {

#define END_DEF_STRUCTURE(__NAME)                                               \
            };
//! @}


//! \brief macro for inheritance

#define INHERIT_EX(__TYPE, __NAME)                                              \
            union {                                                             \
                __TYPE  __NAME;                                                 \
                __TYPE;                                                         \
            };

/*! \note When deriving a new class from a base class, you should use INHERIT 
 *        other than IMPLEMENT, although they looks the same now.
 */
#define __INHERIT(__TYPE)           INHERIT_EX(__TYPE, base__##__TYPE)
#define INHERIT(__TYPE)             __INHERIT(__TYPE)

/*! \note You can only use IMPLEMENT when defining INTERFACE. For Implement 
 *        interface when defining CLASS, you should use DEF_CLASS_IMPLEMENT 
 *        instead.
 */
#define __IMPLEMENT(__INTERFACE)    INHERIT_EX(__INTERFACE, base__##__INTERFACE)
#define IMPLEMENT(__INTERFACE)      __IMPLEMENT(__INTERFACE)  

/*! \note if you have used INHERIT or IMPLEMENT to define a CLASS / INTERFACE, 
          you can use OBJ_CONVERT_AS to extract the reference to the inherited 
          object. 
  \*/
#define __OBJ_CONVERT_AS(__OBJ, __INTERFACE)    (__OBJ.base__##__INTERFACE)
#define OBJ_CONVERT_AS(__OBJ, __INTERFACE)      __OBJ_CONVERT_AS((__OBJ), __INTERFACE)          

#define __REF_OBJ_AS(__OBJ, __TYPE)             (&(__OBJ.base__##__TYPE))
#define REF_OBJ_AS(__OBJ, __TYPE)               __REF_OBJ_AS((__OBJ), __TYPE)

#define REF_INTERFACE(__INTERFACE)      const __INTERFACE *ptMethod;
           
/*----------------------------------------------------------------------------*          
 * new standard (lower case)                                                  *
 *----------------------------------------------------------------------------*/
#define declare_class(__NAME)               DECLARE_CLASS(__NAME)
#define class(__NAME)                       CLASS(__NAME)
#define def_class(__NAME,...)               DEF_CLASS(__NAME, __VA_ARGS__)
#define end_def_class(__NAME,...)           END_DEF_CLASS(__NAME, __VA_ARGS__)
#define extern_class(__NAME,...)            EXTERN_CLASS(__NAME, __VA_ARGS__)
#define end_extern_class(__NAME,...)        END_EXTERN_CLASS(__NAME, __VA_ARGS__)
#define inherit(__TYPE)                     INHERIT(__TYPE)
#define implement(__TYPE)                   IMPLEMENT(__TYPE)
#define inherit_ex(__TYPE, __NAME)          INHERIT_EX(__TYPE, __NAME)
#define which(...)                          WHICH(__VA_ARGS__)
#define ref_interface(__NAME)               REF_INTERFACE(__NAME)
#define convert_obj_as(__OBJ, __TYPE)       OBJ_CONVERT_AS(__OBJ, __TYPE)
#define obj_convert_as(__OBJ, __TYPE)       OBJ_CONVERT_AS(__OBJ, __TYPE)       /*  obsolete */
#define ref_obj_as(__OBJ, __TYPE)           REF_OBJ_AS(__OBJ, __TYPE)
#define type_convert(__ADDR, __TYPE)        TYPE_CONVERT(__ADDR, __TYPE)
#define def_interface(__NAME, ...)          DEF_INTERFACE(__NAME, __VA_ARGS__)
#define end_def_interface(__NAME)           END_DEF_INTERFACE(__NAME)
#define def_structure(__NAME, ...)          DEF_STRUCTURE(__NAME, __VA_ARGS__)
#define end_def_structure(__NAME)           END_DEF_STRUCTURE(__NAME)

#define def_protected(__BELONGS_TO, ...)            DEF_PROTECTED         (__BELONGS_TO, __VA_ARGS__ )
#define end_def_protected(__BELONGS_TO, ...)        END_DEF_PROTECTED     (__BELONGS_TO, __VA_ARGS__ )
#define extern_protected(__BELONGS_TO, ...)         EXTERN_PROTECTED      (__BELONGS_TO, __VA_ARGS__ )
#define end_extern_protected(__BELONGS_TO, ...)     END_EXTERN_PROTECTED  (__BELONGS_TO, __VA_ARGS__ )
#define protected(__BELONGS_TO)                     PROTECTED             (__BELONGS_TO)
#define protected_content(__BELONGS_TO)             PROTECTED_CONTENT     (__BELONGS_TO)
#define declare_protected(__BELONGS_TO)             DECLARE_PROTECTED     (__BELONGS_TO)

#define this_protected(__TYPE)          type_convert(ref_obj_as(this, protected(__TYPE)), protected_content(__TYPE))
#define this_interface(__INTERFACE)     convert_obj_as(this, __INTERFACE)
#define base_obj(__TYPE)                convert_obj_as(this, __TYPE)


#define __class_internal(__SRC, __DES, __TYPE)                                  \
            class(__TYPE) *(__DES) = (CLASS(__TYPE) *)(__SRC)   
#define class_internal(__SRC, __DES, __TYPE)                                    \
            __class_internal(__SRC, __DES, __TYPE)                    

        
/*============================ TYPES =========================================*/


typedef fsm_rt_t DELEGATE_HANDLE_FUNC(void *pArg, void *pParam);

DECLARE_CLASS( DELEGATE_HANDLE )
//! \name general event handler
//! @{
EXTERN_CLASS( DELEGATE_HANDLE )
    DELEGATE_HANDLE_FUNC   *fnHandler;                                          //!< event handler
    void                   *pArg;                                               //!< Argument
    DELEGATE_HANDLE        *ptNext;                                             //!< next 
END_EXTERN_CLASS(DELEGATE_HANDLE)
//! @}

DECLARE_CLASS( DELEGATE )
//! \name event
//! @{
EXTERN_CLASS(DELEGATE)
    DELEGATE_HANDLE     *ptEvent;
    DELEGATE_HANDLE     *ptBlockedList;
    DELEGATE_HANDLE     **pptHandler;
END_EXTERN_CLASS(DELEGATE)
//! @}

//! \name interface: u32_property_t
//! @{
DEF_INTERFACE(u32_property_t)
    bool (*Set)(uint32_t wValue);
    uint32_t (*Get)(void);
END_DEF_INTERFACE(u32_property_t)
//! @}

//! \name interface: u16_property_t
//! @{
DEF_INTERFACE(u16_property_t)
    bool (*Set)(uint_fast16_t wValue);
    uint_fast16_t (*Get)(void);
END_DEF_INTERFACE(u16_property_t)
//! @}

//! \name interface: u8_property_t
//! @{
DEF_INTERFACE(u8_property_t)
    bool (*Set)(uint_fast8_t wValue);
    uint_fast8_t (*Get)(void);
END_DEF_INTERFACE(u8_property_t)
//! @}


//! \name interface: i32_property_t
//! @{
DEF_INTERFACE(i32_property_t)
    bool (*Set)(int32_t wValue);
    int32_t (*Get)(void);
END_DEF_INTERFACE(i32_property_t)
//! @}

//! \name interface: i16_property_t
//! @{
DEF_INTERFACE(i16_property_t)
    bool (*Set)(int_fast16_t wValue);
    int_fast16_t (*Get)(void);
END_DEF_INTERFACE(i16_property_t)
//! @}

//! \name interface: u8_property_t
//! @{
DEF_INTERFACE(i8_property_t)
    bool (*Set)(int_fast8_t wValue);
    int_fast8_t (*Get)(void);
END_DEF_INTERFACE(i8_property_t)
//! @}

//! \name interface: bool_property_t
//! @{
DEF_INTERFACE(bool_property_t)
    bool (*Set)(bool bValue);
    bool (*Get)(void);
END_DEF_INTERFACE(bool_property_t)
//! @}

//! \name interface: bool_property_t
//! @{
DEF_INTERFACE(en_property_t)
    bool (*Enable)(void);
    bool (*Disable)(void);
END_DEF_INTERFACE(en_property_t)
//! @}


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief initialize event 
 *! \param ptEvent target event
 *! \return the address of event item
 */
extern DELEGATE *delegate_init(DELEGATE *ptEvent);

/*! \brief initialize event handler item
 *! \param ptHandler the target event handler item
 *! \param fnRoutine event handler routine
 *! \param pArg handler extra arguments
 *! \return the address of event handler item
 */
extern DELEGATE_HANDLE *delegate_handler_init(
    DELEGATE_HANDLE *ptHandler, DELEGATE_HANDLE_FUNC *fnRoutine, void *pArg);

/*! \brief register event handler to specified event
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
extern gsf_err_t register_delegate_handler(DELEGATE *ptEvent, DELEGATE_HANDLE *ptHandler);

/*! \brief unregister a specified event handler
 *! \param ptEvent target event
 *! \param ptHandler target event handler
 *! \return access result
 */
extern gsf_err_t unregister_delegate_handler( DELEGATE *ptEvent, DELEGATE_HANDLE *ptHandler);

/*! \brief raise target event
 *! \param ptEvent the target event
 *! \param pArg event argument
 *! \return access result
 */
extern fsm_rt_t invoke_delegate( DELEGATE *ptEvent, void *pParam);
#endif
/* EOF */
