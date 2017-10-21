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
#define CRC7_POLY                  (0x89)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
#if CRC7_OPTIMIZE == CRC_OPT_SPEED
static const uint8_t c_chCRC7Table[256] = {
    0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x41, 0x5A, 0x53,
    0x6C, 0x65, 0x7E, 0x77, 0x19, 0x10, 0x0B, 0x02, 0x3D, 0x34, 0x2F, 0x26,
    0x51, 0x58, 0x43, 0x4A, 0x75, 0x7C, 0x67, 0x6E, 0x32, 0x3B, 0x20, 0x29,
    0x16, 0x1F, 0x04, 0x0D, 0x7A, 0x73, 0x68, 0x61, 0x5E, 0x57, 0x4C, 0x45,
    0x2B, 0x22, 0x39, 0x30, 0x0F, 0x06, 0x1D, 0x14, 0x63, 0x6A, 0x71, 0x78,
    0x47, 0x4E, 0x55, 0x5C, 0x64, 0x6D, 0x76, 0x7F, 0x40, 0x49, 0x52, 0x5B,
    0x2C, 0x25, 0x3E, 0x37, 0x08, 0x01, 0x1A, 0x13, 0x7D, 0x74, 0x6F, 0x66,
    0x59, 0x50, 0x4B, 0x42, 0x35, 0x3C, 0x27, 0x2E, 0x11, 0x18, 0x03, 0x0A,
    0x56, 0x5F, 0x44, 0x4D, 0x72, 0x7B, 0x60, 0x69, 0x1E, 0x17, 0x0C, 0x05,
    0x3A, 0x33, 0x28, 0x21, 0x4F, 0x46, 0x5D, 0x54, 0x6B, 0x62, 0x79, 0x70,
    0x07, 0x0E, 0x15, 0x1C, 0x23, 0x2A, 0x31, 0x38, 0x41, 0x48, 0x53, 0x5A,
    0x65, 0x6C, 0x77, 0x7E, 0x09, 0x00, 0x1B, 0x12, 0x2D, 0x24, 0x3F, 0x36,
    0x58, 0x51, 0x4A, 0x43, 0x7C, 0x75, 0x6E, 0x67, 0x10, 0x19, 0x02, 0x0B,
    0x34, 0x3D, 0x26, 0x2F, 0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C,
    0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04, 0x6A, 0x63, 0x78, 0x71,
    0x4E, 0x47, 0x5C, 0x55, 0x22, 0x2B, 0x30, 0x39, 0x06, 0x0F, 0x14, 0x1D,
    0x25, 0x2C, 0x37, 0x3E, 0x01, 0x08, 0x13, 0x1A, 0x6D, 0x64, 0x7F, 0x76,
    0x49, 0x40, 0x5B, 0x52, 0x3C, 0x35, 0x2E, 0x27, 0x18, 0x11, 0x0A, 0x03,
    0x74, 0x7D, 0x66, 0x6F, 0x50, 0x59, 0x42, 0x4B, 0x17, 0x1E, 0x05, 0x0C,
    0x33, 0x3A, 0x21, 0x28, 0x5F, 0x56, 0x4D, 0x44, 0x7B, 0x72, 0x69, 0x60,
    0x0E, 0x07, 0x1C, 0x15, 0x2A, 0x23, 0x38, 0x31, 0x46, 0x4F, 0x54, 0x5D,
    0x62, 0x6B, 0x70, 0x79,
};
#endif 

#if CRC7_OPTIMIZE  == CRC_OPT_BALANCE
#error CRC7 NOT HAVE CRC_OPT_BALANCE, ONLY HAVE CRC_OPT_SPEED AND CRC_OPT_SIZE
#endif

/*============================ IMPLEMENTATION ================================*/
#if CRC7_OPTIMIZE == CRC_OPT_SIZE
/*! \brief CRC7 MMC SD
 *!        Poly: 0x09
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC7 result
 */
uint8_t crc7_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint8_t chCRC7 = 0;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        chCRC7 = (*pchData++) ^ (*pchCRCValue << 1);
        for (j = 0; j < 7; j++) {
            if (chCRC7 & 0x80) {
                chCRC7 ^= CRC7_POLY;
            } 
            chCRC7 <<= 1;
        }
        if (chCRC7 & 0x80) {
            chCRC7 ^= CRC7_POLY;
        } 
        *pchCRCValue = chCRC7;
    }

    return *pchCRCValue;
}


/*! \brief CRC7 MMC SD
 *!        Poly: 0x09
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC7 result
 */
uint8_t crc7_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint32_t i;
    uint8_t chCRC7 = 0;

    if (NULL == pchCRCValue) {
        return false;
    }

    chCRC7 = chData ^ (*pchCRCValue << 1);
    for (i = 0; i < 7; i++) {
        if (chCRC7 & 0x80) {
            chCRC7 ^= CRC7_POLY;
        } 
        chCRC7 <<= 1;
    }
    if (chCRC7 & 0x80) {
        chCRC7 ^= CRC7_POLY;
    } 

    return ((*pchCRCValue) = chCRC7);
}
#endif


#if CRC7_OPTIMIZE == CRC_OPT_SPEED
/*! \brief CRC7 MMC SD
 *!        Poly: 0x09
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC7 result
 */
uint8_t crc7_stream_check(uint8_t *pchCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint8_t chCRC7;

    if ((NULL == pchCRCValue) || (NULL == pchData)) {
        return false;
    }

    chCRC7 = *pchCRCValue;
    for (i = 0; i < wLength; i++) {
        chCRC7 = c_chCRC7Table[(chCRC7 << 1) ^ *pchData++];
    }

    return (*pchCRCValue = chCRC7);
}  


/*! \brief CRC7 MMC SD
 *!        Poly: 0x09
 *!        Init: 0x00
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00
 *! \param pchCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC7 result
 */
uint8_t crc7_check(uint8_t *pchCRCValue, uint8_t chData)
{
    uint8_t chCRC7;

    if (NULL == pchCRCValue) {
        return false;
    }

    chCRC7 = *pchCRCValue << 1;
    chCRC7 = c_chCRC7Table[chCRC7 ^ chData];

    return (*pchCRCValue = chCRC7);

}

#endif
