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

/*============================ MACROS ========================================*/
#define CRC8_POLY                  (0x07)
#define CRC8_ROHC_POLY             (0x07)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
#if CRC8_OPTIMIZE == CRC_OPT_SPEED
static const uint8_t c_chCRC8Table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31,
    0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D, 0xE0, 0xE7, 0xEE, 0xE9,
    0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1,
    0xB4, 0xB3, 0xBA, 0xBD, 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA, 0xB7, 0xB0, 0xB9, 0xBE,
    0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A, 0x89, 0x8E, 0x87, 0x80,
    0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8,
    0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44, 0x19, 0x1E, 0x17, 0x10,
    0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F,
    0x6A, 0x6D, 0x64, 0x63, 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13, 0xAE, 0xA9, 0xA0, 0xA7,
    0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF,
    0xFA, 0xFD, 0xF4, 0xF3,
};
#endif

#if CRC8_OPTIMIZE == CRC_OPT_BALANCE
static const uint8_t c_chCRC8Table[16] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
};
#endif

#if CRC8_ROHC == CRC_OPT_SPEED
//! this name is specific
static const uint8_t c_chCRC8_ROHC_Table[256] = {
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C,
    0x09, 0x98, 0xEA, 0x7B, 0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67, 0x38, 0xA9, 0xDB, 0x4A,
    0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58,
    0x2D, 0xBC, 0xCE, 0x5F, 0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B, 0x6C, 0xFD, 0x8F, 0x1E,
    0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34,
    0x41, 0xD0, 0xA2, 0x33, 0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F, 0xE0, 0x71, 0x03, 0x92,
    0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80,
    0xF5, 0x64, 0x16, 0x87, 0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3, 0xC4, 0x55, 0x27, 0xB6,
    0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC,
    0x99, 0x08, 0x7A, 0xEB, 0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7, 0xA8, 0x39, 0x4B, 0xDA,
    0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8,
    0xBD, 0x2C, 0x5E, 0xCF,
};
#endif

#if CRC8_ROHC == CRC_OPT_BALANCE
static const uint8_t c_chCRC8_ROHC_Table[16] = {
    0x00, 0x1C, 0x38, 0x24, 0x70, 0x6C, 0x48, 0x54,
    0xE0, 0xFC, 0xD8, 0xC4, 0x90, 0x8C, 0xA8, 0xB4,
};
#endif


/*============================ IMPLEMENTATION ================================*/
#if CRC8_ROHC == CRC_OPT_SIZE 
/*! \brief data reversal
 *! \param wValue need to reversal data
 *! \param chLength data bit length
 *! \return already reversal data
 */
static uint32_t data_reversal(uint32_t wValue, uint8_t chLength)
{
    uint8_t i;
    uint32_t wTempValue = 0;

    for (i = 1; i < (chLength + 1); i++) {
        if (wValue & 0x01) {
            wTempValue |= ((uint32_t)1 << (chLength - i));
        }
        wValue >>= 1;
    }

    return wTempValue;
}
#endif


#if CRC8_ROHC == CRC_OPT_SIZE
/*! \brief CRC8 ROHC
 *!        Poly: 0x07
 *!        Init: 0xFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC8 result
 */
uint8_t crc8_rohc_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint8_t chCRC8 = 0;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        chCRC8 = (*pchCRCValue) ^ (*pchData++);        
        chCRC8 = (uint8_t)data_reversal(chCRC8, 8);
        for (j = 0; j < 8; j++) {
            if (chCRC8 & 0x80) {
                chCRC8 <<= 1;
                chCRC8 ^= CRC8_ROHC_POLY;
            } else {
                chCRC8 <<= 1;
            }
        }
        chCRC8 = (uint8_t)data_reversal(chCRC8, 8);
        *pchCRCValue = chCRC8;
    }

    return *pchCRCValue;
}


/*! \brief CRC8 ROHC
 *!        Poly: 0x07
 *!        Init: 0xFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC8 result
 */
uint8_t crc8_rohc_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint32_t i;
    uint8_t chCRC8 = 0;

    if (NULL == pchCRCValue) {
        return false;
    }

    chCRC8 = (*pchCRCValue) ^ chData;        
    chCRC8 = (uint8_t)data_reversal(chCRC8, 8);
    for (i = 0; i < 8; i++) {
        if (chCRC8 & 0x80) {
            chCRC8 <<= 1;
            chCRC8 ^= CRC8_ROHC_POLY;
        } else {
            chCRC8 <<= 1;
        }
    }
    chCRC8 = (uint8_t)data_reversal(chCRC8, 8);
    
    return (*pchCRCValue = chCRC8);
}
#endif


#if CRC8_ROHC == CRC_OPT_BALANCE
/*! \brief CRC8 ROHC
 *!        Poly: 0x07
 *!        Init: 0xFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC8 result
 */
uint8_t crc8_rohc_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint8_t chCRC8 = 0;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }
    
    chCRC8 = *pchCRCValue;
    for (i = 0; i < wLength; i++) {
        chCRC8 = (chCRC8 >> 4) ^ c_chCRC8_ROHC_Table[(chCRC8 & 0x0F) ^ (*pchData & 0x0F)];
        chCRC8 = (chCRC8 >> 4) ^ c_chCRC8_ROHC_Table[(chCRC8 & 0x0F) ^ (*pchData++ >> 4)];
    }

    return (*pchCRCValue = chCRC8);
} 


/*! \brief CRC8 ROHC
 *!        Poly: 0x07
 *!        Init: 0xFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC8 result
 */
uint8_t crc8_rohc_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint8_t chCRC8 = 0;

    if (NULL == pchCRCValue) {
        return false;
    }
    
    chCRC8 = *pchCRCValue;
    chCRC8 = (chCRC8 >> 4) ^ c_chCRC8_ROHC_Table[(chCRC8 & 0x0F) ^ (chData & 0x0F)];
    chCRC8 = (chCRC8 >> 4) ^ c_chCRC8_ROHC_Table[(chCRC8 & 0x0F) ^ (chData >> 4)];

    return (*pchCRCValue = chCRC8);
} 
#endif


#if CRC8_ROHC == CRC_OPT_SPEED
/*! \brief CRC8 ROHC
 *!        Poly: 0x07
 *!        Init: 0xFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC8 result
 */
uint8_t crc8_rohc_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint8_t chCRC8 = 0;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }

    chCRC8 = *pchCRCValue;
    for (i = 0; i < wLength; i++) {
        chCRC8 = c_chCRC8_ROHC_Table[chCRC8 ^ *pchData++];
    }

    return (*pchCRCValue = chCRC8);
}  


/*! \brief CRC8 ROHC
 *!        Poly: 0x07
 *!        Init: 0xFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC8 result
 */
uint8_t crc8_rohc_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint16_t chCRC8;

    if (NULL == pchCRCValue) {
        return false;
    }

    chCRC8 = *pchCRCValue;
    chCRC8 = c_chCRC8_ROHC_Table[chCRC8 ^ chData];

    return (*pchCRCValue = chCRC8);

}
#endif


#if CRC8_OPTIMIZE == CRC_OPT_SIZE
/*! \brief CRC8
 *!        Poly: 0x07
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC8 result
 */
uint8_t crc8_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint8_t chCRC8 = 0;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        chCRC8 = (*pchData++) ^ (*pchCRCValue);
        for (j = 0; j < 8; j++) {
            if (chCRC8 & 0x80) {
                chCRC8 <<= 1;
                chCRC8 ^= CRC8_POLY;
            } else {
                chCRC8 <<= 1;
            }
        }
        *pchCRCValue = chCRC8;
    }

    return *pchCRCValue;
}


/*! \brief CRC8
 *!        Poly: 0x07
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC8 result
 */
uint8_t crc8_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint32_t i;
    uint8_t chCRC8 = 0;

    if (NULL == pchCRCValue) {
        return false;
    }

    chCRC8 = chData ^ (*pchCRCValue);
    for (i = 0; i < 8; i++) {
        if (chCRC8 & 0x80) {
            chCRC8 <<= 1;
            chCRC8 ^= CRC8_POLY;
        } else {
            chCRC8 <<= 1;
        }
    }

    return ((*pchCRCValue) = chCRC8);
}
#endif


#if CRC8_OPTIMIZE  == CRC_OPT_BALANCE
/*! \brief CRC8
 *!        Poly: 0x07
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC8 result
 */
uint8_t crc8_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint8_t chCRC8 = 0;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }
    
    chCRC8 = *pchCRCValue;
    for (i = 0; i < wLength; i++) {
        chCRC8 = (chCRC8 << 4) ^ c_chCRC8Table[(chCRC8 >> 4) ^ (*pchData >> 4)];
        chCRC8 = (chCRC8 << 4) ^ c_chCRC8Table[(chCRC8 >> 4) ^ (*pchData++ & 0x0F)];
    }

    return (*pchCRCValue = chCRC8);
} 


/*! \brief CRC8
 *!        Poly: 0x07
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \return CRC8 result
 */
uint8_t crc8_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint8_t chCRC8 = 0;

    if (NULL == pchCRCValue) {
        return false;
    }
    
    chCRC8 = *pchCRCValue;
    chCRC8 = (chCRC8 << 4) ^ c_chCRC8Table[(chCRC8 >> 4) ^ (chData >> 4)];
    chCRC8 = (chCRC8 << 4) ^ c_chCRC8Table[(chCRC8 >> 4) ^ (chData & 0x0F)];

    return (*pchCRCValue = chCRC8);
} 
#endif


#if CRC8_OPTIMIZE == CRC_OPT_SPEED
/*! \brief CRC8
 *!        Poly: 0x07
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC8 result
 */
uint8_t crc8_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t chCRC8;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }

    chCRC8 = *pchCRCValue;
    for (i = 0; i < wLength; i++) {
        chCRC8 = c_chCRC8Table[chCRC8 ^ *pchData++];
    }

    return (*pchCRCValue = chCRC8);
}  


/*! \brief CRC8
 *!        Poly: 0x07
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC8 result
 */
uint8_t crc8_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint16_t chCRC8;

    if (NULL == pchCRCValue) {
        return false;
    }

    chCRC8 = *pchCRCValue;
    chCRC8 = c_chCRC8Table[chCRC8 ^ chData];

    return (*pchCRCValue = chCRC8);

}
#endif

/* EOF */
