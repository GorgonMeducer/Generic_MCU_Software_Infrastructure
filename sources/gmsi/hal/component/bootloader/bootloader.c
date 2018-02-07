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
/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"


#if USE_COMPONENT_BOOTLOADER == ENABLED

#include "..\..\..\service\service.h"

/*============================ MACROS ========================================*/
#ifndef IAP_VERSION
#   define IAP_VERSION     1,0
#endif

#ifndef IAP_RSP_ERROR
#   define IAP_RSP_ERROR                    0xF0
#endif
#ifndef IAP_RSP_ERROR_BL_OVERLAP
#   define IAP_RSP_ERROR_BL_OVERLAP         0xF1
#endif
#ifndef IAP_RSP_OK
#   define IAP_RSP_OK                       0xAC
#endif

# define IAP_CMD_ACK                        0x01
# define IAP_CMD_WRITE_MEMORY               0x02
# define IAP_CMD_READ_MEMORY                0x03
# define IAP_CMD_CHIP_ERASE                 0x04
# define IAP_CMD_RESUME                     0x05
# define IAP_CMD_VERIFY_MEMORY              0x06
# define IAP_CMD_PROTECT_MEMORY             0x07
# define IAP_CMD_UNPROTECT_MEMORY           0x08
# define IAP_CMD_SECURE                     0x09


#ifndef IAP_VERSION_STRING
#   define IAP_VERSION_STRING "Snail Bootloader"
#endif

#define IAP_16BIT_ADDR                      0
#define IAP_32BIT_ADDR                      _BV(7)

#ifndef IAP_ADDRESS_BIT_LENGTH
#   define IAP_ADDRESS_BIT_LENGTH           IAP_16BIT_ADDR
#endif

#ifndef IAP_CFG_A
#   define IAP_CFG_A    0
#endif
#ifndef IAP_CFG_B
#   define IAP_CFG_B    0
#endif
#ifndef IAP_CFG_C
#   define IAP_CFG_C    0
#endif

#ifndef IAP_DATA_BUFFER_SIZE
#   define IAP_DATA_BUFFER_SIZE        256
#   warning no defined IAP_DATA_BUFFER_SIZE, default value 256 used.
#endif

#ifndef IAP_DEVICE_SIGNATURE
#   define IAP_DEVICE_SIGNATURE        'G','S','P'
#endif

#if IAP_SUPPORT_UID == ENABLED
#   ifndef IAP_UID_BIT_SIZE
#       error no defined IAP_UID_BIT_SIZE
#   endif
#   ifndef IAP_GET_UID_ADDRESS
#       error no defined IAP_GET_UID_ADDRESS()
#   endif
#   ifndef IAP_UID_DATA_TYPE
#       error no defined IAP_UID_DATA_TYPE
#   endif
#endif

#ifndef BL_MEMORY_TYPE_SET
#   define BL_MEMORY_TYPE_SET      {0}
#   warning no bootloader memory type defined, please add support with macro\
    BL_MEMORTY_TYPE_SET and\
    BL_MEM_TYPE(__ID, __WRITE, __READ, __VERIFY, __EREASE, __PROTECT, __UNPROTECT, __SECURE)
#endif

#ifndef IAP_RESUME
#   define IAP_RESUME(__ADDRESS, __SIZE)            (0)
#endif

#ifndef IAP_IS_DATA_DECRYPTION_ENABLED
#   define IAP_IS_DATA_DECRYPTION_ENABLED()         (true)
#endif

#ifndef IAP_IS_SYSTEM_SECURED
#   define IAP_IS_SYSTEM_SECURED()                  (false)
#endif

#ifndef IAP_IS_DATA_PERMUTATE_ENABLED
#   define IAP_IS_DATA_PERMUTATE_ENABLED()          (false)
#endif

#ifndef IAP_FRAME_PERMUTATE_DATA
#   define IAP_FRAME_PERMUTATE_DATA(__ADDRESS, __SIZE)    
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define SET_IAP_REPLY(__CMD)        do {pchStream[0] = (__CMD);} while(0)

#define BL_MEM_TYPE(__ID, __WRITE, __READ, __VERIFY, __EREASE, __PROTECT, __UNPROTECT, __SECURE)  \
            {(__ID), 0xFF, __WRITE, __READ, __VERIFY, __EREASE, __PROTECT, __UNPROTECT, __SECURE}, 

#define BL_MEM_TYPE_EX(__ID, __MSK,__WRITE, __READ, __VERIFY, __EREASE, __PROTECT, __UNPROTECT, __SECURE)  \
            {(__ID), (__MSK), __WRITE, __READ, __VERIFY, __EREASE, __PROTECT, __UNPROTECT, __SECURE}, 

/*============================ TYPES =========================================*/
//! \brief bootloader command handler
typedef uint_fast16_t bl_cmd_handler_t(uint8_t *pchStream, uint_fast16_t hwSize);

#if __IS_COMPILER_IAR__
#pragma pack(1)
#endif
//! \name bootloader command item
//! @{
typedef struct {
    uint8_t             chCMD;          //!< command
    bl_cmd_handler_t   *fnHandler;      //!< command handler
}bl_cmd_item_t;
//! @}
#if __IS_COMPILER_IAR__
#pragma pack()
#endif

#define MEM_ACCESS_WRITE        0
#define MEM_ACCESS_READ         1
#define MEM_ACCESS_VERIFY       2
#define MEM_ACCESS_ERASE        3
#define MEM_ACCESS_PROTECT      4
#define MEM_ACCESS_UNPROTECT    5
#define MEM_ACCESS_SECURE       6


//! \name memory support item 
//! @{
typedef struct {
    uint8_t         chType;                     //!< memory type
    uint8_t         chMask;                     //!< command mask
    union {  
        bl_cmd_handler_t   *fnAccess[7];
        struct {
            bl_cmd_handler_t   *fnWrite;        //!< write memory
            bl_cmd_handler_t   *fnRead;         //!< read memory
            bl_cmd_handler_t   *fnVerify;       //!< verify memory
            bl_cmd_handler_t   *fnErase;        //!< erase whole memory
            bl_cmd_handler_t   *fnProtect;      //!< protect memory block
            bl_cmd_handler_t   *fnUnprotect;    //!< unprotect memory block
            bl_cmd_handler_t   *fnSecurty;      //!< security memory
        }method;
    }_;
}bl_mem_t;
//! @}

#if IAP_DATA_ENCRYPT_AES == ENABLED
//! \name aes decryption
//! @{
typedef struct {
    uint16_t    hwOffset;
    aes_key_t   chKey;
    aes_data_t  chDataMask;
}aes_decrypt_t;
//! @}
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
static uint_fast16_t cmd_ack_handler(uint8_t *pchStream, uint_fast16_t hwSize);
static uint_fast16_t cmd_memory_handler(
        uint8_t *pchStream, uint_fast16_t hwSize, uint_fast8_t chIndex);

#if IAP_SUPPORT_WRITE_MEMORY == ENABLED
static uint_fast16_t cmd_write_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_READ_MEMORY == ENABLED
static uint_fast16_t cmd_read_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_CHIP_ERASE == ENABLED
static uint_fast16_t cmd_chip_erase_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_RESUME == ENABLED
static uint_fast16_t cmd_resume_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_VERIFY_MEMORY == ENABLED
static uint_fast16_t cmd_verify_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_PROTECT_MEMORY == ENABLED
static uint_fast16_t cmd_protect_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_UNPROTECT_MEMORY == ENABLED
static uint_fast16_t cmd_unprotect_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif
#if IAP_SUPPORT_SECURE == ENABLED
static uint_fast16_t cmd_secure_handler(uint8_t *pchStream, uint_fast16_t hwSize);
#endif

/*============================ LOCAL VARIABLES ===============================*/

//! \brief command map
FLASH static bl_cmd_item_t c_tCMDMap[]= {
        {IAP_CMD_ACK,               &cmd_ack_handler},              //!< 0x01: Acknowledge
#if IAP_SUPPORT_WRITE_MEMORY == ENABLED
        {IAP_CMD_WRITE_MEMORY,      &cmd_write_memory_handler},     //!< 0x02: Write memory
#endif
#if IAP_SUPPORT_READ_MEMORY == ENABLED
        {IAP_CMD_READ_MEMORY,       &cmd_read_memory_handler},      //!< 0x03: Read Memory
#endif
#if IAP_SUPPORT_CHIP_ERASE == ENABLED
        {IAP_CMD_CHIP_ERASE,        &cmd_chip_erase_handler},       //!< 0x04: Chip Erase
#endif
#if IAP_SUPPORT_RESUME == ENABLED
        {IAP_CMD_RESUME,            &cmd_resume_handler},           //!< 0x05: Resume
#endif
#if IAP_SUPPORT_VERIFY_MEMORY == ENABLED
        {IAP_CMD_VERIFY_MEMORY,     &cmd_verify_memory_handler},    //!< 0x06: Verify memory
#endif
#if IAP_SUPPORT_PROTECT_MEMORY == ENABLED
        {IAP_CMD_PROTECT_MEMORY,    &cmd_protect_memory_handler},   //!< 0x07: Protect memory
#endif
#if IAP_SUPPORT_UNPROTECT_MEMORY == ENABLED
        {IAP_CMD_UNPROTECT_MEMORY,  &cmd_unprotect_memory_handler}, //!< 0x08: Unprotect memory
#endif
#if IAP_SUPPORT_SECURE == ENABLED
        {IAP_CMD_SECURE,            &cmd_secure_handler},           //!< 0x09: Secure System
#endif
    };

SECTION(".api_table")
//! \brief bootloader memory set
FLASH static bl_mem_t c_tMemory[]= {
        BL_MEMORY_TYPE_SET
    };

/*============================ IMPLEMENTATION ================================*/

/*! \brief bootloader command parser
 *! \param pchStream data block
 *! \param hwSize data block size
 *! \retval 0 failed in parsing
 *! \retval none-zero-size reply data size
 */
ROOT uint_fast16_t booloader_command_parser(uint8_t *pchStream, uint_fast16_t hwSize)
{
    uint_fast8_t n = 0;
    uint_fast8_t chCMD = pchStream[0];
    for (;n < UBOUND(c_tCMDMap);n++) {
        if (c_tCMDMap[n].chCMD == chCMD) {
            return c_tCMDMap[n].fnHandler(pchStream, hwSize);
        }
    }

    return 0;
}

/*! \brief bootloader ack command handler
 *! \param pchStream data block
 *! \param hwSize data block size
 *! \retval 0 failed in parsing
 *! \retval none-zero-size reply data size
 */
static uint_fast16_t cmd_ack_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    uint8_t chBuffer[12 
#if IAP_SUPPORT_UID == ENABLED
     + IAP_UID_BIT_SIZE
#endif
#if IAP_SUPPORT_VER_STRING == ENABLED
    + sizeof(IAP_VERSION_STRING) 
#endif
    ] = {
            IAP_CMD_ACK,
            IAP_DEVICE_SIGNATURE,
            (uint8_t)(IAP_DATA_BUFFER_SIZE & 0x00FF),
            (IAP_DATA_BUFFER_SIZE >> 8),

#if IAP_SUPPORT_WRITE_MEMORY == ENABLED
            _BV(0) |
#endif
#if IAP_SUPPORT_READ_MEMORY == ENABLED
            _BV(1) |
#endif
#if IAP_SUPPORT_CHIP_ERASE == ENABLED
            _BV(2) |
#endif
#if IAP_SUPPORT_RESUME == ENABLED
            _BV(3) |
#endif
#if IAP_SUPPORT_VERIFY_MEMORY == ENABLED
            _BV(4) |
#endif
#if IAP_SUPPORT_PROTECT_MEMORY == ENABLED
            _BV(5) |
#endif
#if IAP_SUPPORT_UNPROTECT_MEMORY == ENABLED
            _BV(6) |
#endif
#if IAP_SUPPORT_SECURE == ENABLED
            _BV(7) |
#endif
            0,
            IAP_CFG_A,IAP_CFG_B,IAP_CFG_C,
            IAP_ADDRESS_BIT_LENGTH | IAP_VERSION
        };
    uint8_t *pchSrc = chBuffer;
#if IAP_SUPPORT_VER_STRING == ENABLED
    const static uint8_t chVersionString[] = {IAP_VERSION_STRING};
#endif
    uint_fast8_t n = 12;
    do {
        *pchStream++ = *pchSrc++;
    } while (--n);
#if IAP_SUPPORT_UID == ENABLED
    do {
        n = IAP_UID_BIT_SIZE / (sizeof(IAP_UID_DATA_TYPE) * 8);
        IAP_UID_DATA_TYPE *ptSrc = (IAP_UID_DATA_TYPE *)IAP_GET_UID_ADDRESS();
        IAP_UID_DATA_TYPE *ptDes = (IAP_UID_DATA_TYPE *)pchStream;
        do {
            *ptDes++ = *ptSrc++;
        } while(--n);
        pchStream = (uint8_t *)ptDes;
    } while(false);
#endif
#if IAP_SUPPORT_VER_STRING == ENABLED
    for (n = 0;n < sizeof(chVersionString);n++) {
         *pchStream++ = chVersionString[n];
    }
#endif
    return 12 
#if IAP_SUPPORT_UID == ENABLED
    + IAP_UID_BIT_SIZE
#endif
#if IAP_SUPPORT_VER_STRING == ENABLED
    + sizeof(IAP_VERSION_STRING)
#endif
    ;
}

#if IAP_DATA_ENCRYPT_AES == ENABLED
static void load_aes_key(uint8_t *pchKey)
{
    uint32_t n = AES_KEY_SIZE;
    do {
        *pchKey++ ^= get_random_u8();
    } while(--n);
}

static void aes_initialize_mask( uint8_t chKey, const uint8_t *pchMask,  uint8_t *pchDes )
{
    uint32_t n = AES_KEY_SIZE;
    reset_random();
    set_random_seed(chKey);
    do {
        *pchDes++ = *pchMask++ ^ get_random_u8();
    } while(--n);
}

static void aes_decryption(uint8_t chKey, uint8_t *pchOrgStream, uint32_t wSize)
{
    aes_decrypt_t tAES;
    wSize &= ~(AES_BLOCK_SIZE - 1);

    //! load first aes key
    do {
        const aes_key_t chKeyMask = {
            0x11,0x45,0x76,0xCF,0xE0,0x2C,0xFE,0x38,
            0x6D,0x9A,0x00,0xB2,0xA5,0x5A,0xFF,0x01};
        aes_initialize_mask(chKey, chKeyMask, tAES.chKey);
    } while(false);

    //! load datamask
    do {
        const aes_data_t chDataMask = {
            0x01,0x48,0xCC,0x7E,0x2F,0x6C,0xEE,0xCD,
            0x55,0xAA,0x39,0xCA,0xBF,0x26,0x69,0x21};
        aes_initialize_mask(chKey, chDataMask, tAES.chDataMask);
    } while(false);

    tAES.hwOffset = 0;
    //! load first aes key
    load_aes_key(tAES.chKey);

    //! decrypt
    do {
        uint32_t wOffset = tAES.hwOffset;
        uint8_t *pchStream = pchOrgStream + wOffset;
        uint8_t *pchMask = tAES.chDataMask;
        uint32_t n = AES_BLOCK_SIZE;

        do {
            uint8_t chTemp = *pchMask;
            *pchMask++ = *pchStream;
            *pchStream++ ^= chTemp;
        } while(--n);

        pchStream = pchOrgStream + wOffset;

        aes_prepare_decrypt(tAES.chKey);
        do {
            aes_decrypt(pchStream,tAES.chKey);
        } while(false);
        
        wOffset += AES_BLOCK_SIZE;
        if (wOffset >= wSize) {
            //! complete
            break;
        } else {
            load_aes_key(tAES.chKey);
        }

        tAES.hwOffset = wOffset;
    } while (true);

}
#endif



static uint_fast16_t cmd_memory_handler(
        uint8_t *pchStream, uint_fast16_t hwSize, uint_fast8_t chIndex)
{
    if (IAP_IS_SYSTEM_SECURED()) {
        do {
            if (MEM_ACCESS_ERASE == chIndex) {
                break;
            }
        #if defined(IAP_SECURE_CHECK_CODE)
            IAP_SECURE_CHECK_CODE
        #endif

            return 0;
        } while(false);
    }

#if IAP_DATA_ENCRYPT_AES == ENABLED
    if (IAP_IS_DATA_DECRYPTION_ENABLED()) {
        aes_decryption(0xC5,pchStream+1,hwSize);
    }
#endif    

#if IAP_DATA_PERMUTATE == ENABLED
    if (IAP_IS_DATA_PERMUTATE_ENABLED()) {
        IAP_FRAME_PERMUTATE_DATA( pchStream+1, hwSize );
    }
#endif

    do {
        uint_fast8_t chMemoryType = pchStream[1];
        uint_fast8_t n;

        //! search for memory type
        for (n = 0; n < UBOUND(c_tMemory); n++) {
            FLASH bl_mem_t *ptMem = &c_tMemory[n];
            if (    (chMemoryType & ptMem->chMask)
                !=  (ptMem->chType & ptMem->chMask)) {
                continue;
            } 
            if (NULL == ptMem->_.fnAccess[chIndex]) {
                //! memory-write is not supported 
                SET_IAP_REPLY(IAP_RSP_ERROR);
                break;
            }

            //! call specified memory command handler
            return ptMem->_.fnAccess[chIndex](pchStream, hwSize);
        }
    } while (false);

    //! illegal memorty type
    SET_IAP_REPLY(IAP_RSP_ERROR);
    return 1;
}

#if IAP_SUPPORT_WRITE_MEMORY == ENABLED
static uint_fast16_t cmd_write_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{   
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_WRITE);
} 
#endif

#if IAP_SUPPORT_READ_MEMORY == ENABLED
static uint_fast16_t cmd_read_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_READ);
}
#endif

#if IAP_SUPPORT_CHIP_ERASE == ENABLED
static uint_fast16_t cmd_chip_erase_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_ERASE);
}
#endif

#if IAP_SUPPORT_RESUME == ENABLED
static uint_fast16_t cmd_resume_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    return IAP_RESUME(pchStream, hwSize);
}
#endif

#if IAP_SUPPORT_VERIFY_MEMORY == ENABLED
static uint_fast16_t cmd_verify_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{   
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_VERIFY);
} 
#endif
  
#if IAP_SUPPORT_PROTECT_MEMORY == ENABLED
static uint_fast16_t cmd_protect_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_PROTECT);
}
#endif

#if IAP_SUPPORT_UNPROTECT_MEMORY == ENABLED
static uint_fast16_t cmd_unprotect_memory_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_UNPROTECT);
}
#endif

#if IAP_SUPPORT_SECURE == ENABLED
static uint_fast16_t cmd_secure_handler(uint8_t *pchStream, uint_fast16_t hwSize)
{
    return cmd_memory_handler(pchStream, hwSize, MEM_ACCESS_SECURE);
}
#endif

#endif
/* EOF */
