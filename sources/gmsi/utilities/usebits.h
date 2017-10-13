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

#ifndef _USE_BITS_H_
#define _USE_BITS_H_

/*============================ INCLUDES ======================================*/
#include "compiler.h"

/*============================ MACROS ========================================*/
//! \name binary compatibility definition
//! @{
# define BIT00      BIT0                //!< bit 0 
# define BIT01      BIT1                //!< bit 1
# define BIT02      BIT2                //!< bit 2
# define BIT03      BIT3                //!< bit 3
# define BIT04      BIT4                //!< bit 4
# define BIT05      BIT5                //!< bit 5
# define BIT06      BIT6                //!< bit 6
# define BIT07      BIT7                //!< bit 7
# define BIT08      BIT8                //!< bit 8
# define BIT09      BIT9                //!< bit 9
//! @}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef union {
    uint8_t     Value; 
    uint8_t     Byte;    
    struct {
    #if __BIG_ENDIAN__ != true
        unsigned BIT0:1;
        unsigned BIT1:1;
        unsigned BIT2:1;
        unsigned BIT3:1;
        unsigned BIT4:1;
        unsigned BIT5:1;
        unsigned BIT6:1;
        unsigned BIT7:1;
    #else
        unsigned BIT7:1;
        unsigned BIT6:1;
        unsigned BIT5:1;
        unsigned BIT4:1;
        unsigned BIT3:1;
        unsigned BIT2:1;
        unsigned BIT1:1;
        unsigned BIT0:1;
    #endif
    }           Bits;
    
}byte_t;

typedef union
{
    uint16_t    Value;
    uint8_t     Byte[2];
    
    struct {
    #if !defined(__BIG_ENDIAN) || __BIG_ENDIAN__ != true
        unsigned BIT0:1;
        unsigned BIT1:1;
        unsigned BIT2:1;
        unsigned BIT3:1;
        unsigned BIT4:1;
        unsigned BIT5:1;
        unsigned BIT6:1;
        unsigned BIT7:1;
    
        unsigned BIT8:1;
        unsigned BIT9:1;
        unsigned BIT10:1;
        unsigned BIT11:1;
        unsigned BIT12:1;
        unsigned BIT13:1;
        unsigned BIT14:1;
        unsigned BIT15:1;
    #else
        unsigned BIT15:1;
        unsigned BIT14:1;
        unsigned BIT13:1;
        unsigned BIT12:1;
        unsigned BIT11:1;
        unsigned BIT10:1;
        unsigned BIT9:1;
        unsigned BIT8:1;
    
        unsigned BIT7:1;
        unsigned BIT6:1;
        unsigned BIT5:1;
        unsigned BIT4:1;
        unsigned BIT3:1;
        unsigned BIT2:1;
        unsigned BIT1:1;
        unsigned BIT0:1;
    #endif
    }           Bits;
    struct {
    #if !defined(__BIG_ENDIAN) || __BIG_ENDIAN__ != true
        uint8_t ByteL;
        uint8_t ByteH;
    #else
        uint8_t ByteH;
        uint8_t ByteL;
    #endif
    }           HWord;
}hword_t;

typedef union {
    uint32_t    Value;
    uint8_t     Byte[4];
    hword_t     HWord[2];
    struct {    
    #if !defined(__BIG_ENDIAN) || __BIG_ENDIAN__ != true
        unsigned BIT0:1;
        unsigned BIT1:1;
        unsigned BIT2:1;
        unsigned BIT3:1;
        unsigned BIT4:1;
        unsigned BIT5:1;
        unsigned BIT6:1;
        unsigned BIT7:1;
    
        unsigned BIT8:1;
        unsigned BIT9:1;
        unsigned BIT10:1;
        unsigned BIT11:1;
        unsigned BIT12:1;
        unsigned BIT13:1;
        unsigned BIT14:1;
        unsigned BIT15:1;
    
        unsigned BIT16:1;
        unsigned BIT17:1;
        unsigned BIT18:1;
        unsigned BIT19:1;
        unsigned BIT20:1;
        unsigned BIT21:1;
        unsigned BIT22:1;
        unsigned BIT23:1;
    
        unsigned BIT24:1;
        unsigned BIT25:1;
        unsigned BIT26:1;
        unsigned BIT27:1;
        unsigned BIT28:1;
        unsigned BIT29:1;
        unsigned BIT30:1;
        unsigned BIT31:1;
    #else
        unsigned BIT31:1;
        unsigned BIT30:1;
        unsigned BIT29:1;
        unsigned BIT28:1;
        unsigned BIT27:1;
        unsigned BIT26:1;
        unsigned BIT25:1;
        unsigned BIT24:1;
    
        unsigned BIT23:1;
        unsigned BIT22:1;
        unsigned BIT21:1;
        unsigned BIT20:1;
        unsigned BIT19:1;
        unsigned BIT18:1;
        unsigned BIT17:1;
        unsigned BIT16:1;
    
        unsigned BIT15:1;
        unsigned BIT14:1;
        unsigned BIT13:1;
        unsigned BIT12:1;
        unsigned BIT11:1;
        unsigned BIT10:1;
        unsigned BIT9:1;
        unsigned BIT8:1;
    
        unsigned BIT7:1;
        unsigned BIT6:1;
        unsigned BIT5:1;
        unsigned BIT4:1;
        unsigned BIT3:1;
        unsigned BIT2:1;
        unsigned BIT1:1;
        unsigned BIT0:1;
    #endif
    }           Bits;
    struct {
    #if !defined(__BIG_ENDIAN) || __BIG_ENDIAN__ != true
        uint8_t ByteA;
        uint8_t ByteB;
        uint8_t ByteC;
        uint8_t ByteD;
    #else
        uint8_t ByteD;
        uint8_t ByteC;
        uint8_t ByteB;
        uint8_t ByteA;
    #endif
    }           Word;
}word_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
