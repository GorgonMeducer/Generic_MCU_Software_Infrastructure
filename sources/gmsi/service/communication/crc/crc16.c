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
//! \berif This poly contains CRC16_MODBUS and CRC16_USB
#define CRC16_POLY                  (0x8005)
#define CRC16_CCITT_FALSE_POLY      (0x1021)    //!< Equal to CRC16 MODEM

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
#if (CRC16_MODBUS_OPTIMIZE == CRC_OPT_SPEED) || (CRC16_USB_OPTIMIZE == CRC_OPT_SPEED)
//! This table contains CRC16_MODBUS and CRC16_USB
static const uint16_t c_hwCRC16Table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601,
    0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0,
    0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81,
    0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941,
    0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01,
    0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0,
    0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
    0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00,
    0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0,
    0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981,
    0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41,
    0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700,
    0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
    0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281,
    0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01,
    0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1,
    0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80,
    0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541,
    0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101,
    0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0,
    0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481,
    0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841, 0x8801,
    0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1,
    0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581,
    0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
    0x4100, 0x81C1, 0x8081, 0x4040,
};
#endif

#if (CRC16_MODBUS_OPTIMIZE == CRC_OPT_BALANCE) || (CRC16_USB_OPTIMIZE == CRC_OPT_BALANCE)
static const uint16_t c_hwCRC16Tab[16] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 0xA001,
    0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400,
};
#endif

#if (CRC16_CCITT_FALSE_OPTIMIZE == CRC_OPT_SPEED) || (CRC16_MODEM_OPTIMIZE == CRC_OPT_SPEED) 
//! this name is specific
static const uint16_t c_hwCRC16_CCITT_Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108,
    0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 0x1231, 0x0210,
    0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B,
    0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 0x2462, 0x3443, 0x0420, 0x1401,
    0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE,
    0xF5CF, 0xC5AC, 0xD58D, 0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6,
    0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D,
    0xC7BC, 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 0x5AF5,
    0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC,
    0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 0x6CA6, 0x7C87, 0x4CE4,
    0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD,
    0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13,
    0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A,
    0x9F59, 0x8F78, 0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E,
    0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1,
    0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 0xB5EA, 0xA5CB,
    0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0,
    0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xA7DB, 0xB7FA, 0x8799, 0x97B8,
    0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657,
    0x7676, 0x4615, 0x5634, 0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9,
    0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882,
    0x28A3, 0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 0xFD2E,
    0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07,
    0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 0xEF1F, 0xFF3E, 0xCF5D,
    0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74,
    0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};
#endif

#if (CRC16_CCITT_FALSE_OPTIMIZE == CRC_OPT_BALANCE) || (CRC16_MODEM_OPTIMIZE == CRC_OPT_BALANCE) 
static const uint16_t c_hwCRC16_CCITT_Tab[16] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108,
    0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
};
#endif


/*============================ IMPLEMENTATION ================================*/
#if (CRC16_MODBUS_OPTIMIZE == CRC_OPT_SIZE) || (CRC16_USB_OPTIMIZE == CRC_OPT_SIZE)
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


#if CRC16_USB_OPTIMIZE == CRC_OPT_SIZE
/*! \brief CRC16 USB
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFF
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_usb_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        hwCRC16 = ((*phwCRCValue) ^ (*pchData++)) & 0x00FF;        
        hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
        for (j = 0; j < 8; j++) {
            if (hwCRC16 & 0x8000) {
                hwCRC16 <<= 1;
                hwCRC16 ^= CRC16_POLY;
            } else {
                hwCRC16 <<= 1;
            }
        }
         hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
        *phwCRCValue = (*phwCRCValue >> 8) ^ hwCRC16;
    }

    return (*phwCRCValue = ~(*phwCRCValue));
}


/*! \brief CRC16 USB
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFF
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_usb_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = ((*phwCRCValue) ^ chData) & 0x00FF;        
    hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
    for (i = 0; i < 8; i++) {
        if (hwCRC16 & 0x8000) {
            hwCRC16 <<= 1;
            hwCRC16 ^= CRC16_POLY;
        } else {
            hwCRC16 <<= 1;
        }
    }
     hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
    *phwCRCValue = (*phwCRCValue >> 8) ^ hwCRC16;
    
    return *phwCRCValue;
}
#endif


#if CRC16_USB_OPTIMIZE == CRC_OPT_BALANCE
/*! \brief CRC16 USB
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFF
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_usb_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (*pchData & 0x0F)];
        hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (*pchData++ >> 4)];
    }

    return (*phwCRCValue = ~hwCRC16);
} 


/*! \brief CRC16 USB
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFF
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_usb_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (chData & 0x0F)];
    hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (chData >> 4)];

    return (*phwCRCValue = hwCRC16);
} 
#endif


#if CRC16_USB_OPTIMIZE == CRC_OPT_SPEED
/*! \brief CRC16 USB
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFF
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_usb_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 >> 8) ^ c_hwCRC16Table[(hwCRC16 ^ *pchData++) & 0xFF];
    }

    return (*phwCRCValue = ~hwCRC16);

}  


/*! \brief CRC16 USB
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFF
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_usb_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 >> 8) ^ c_hwCRC16Table[(hwCRC16 ^ chData) & 0xFF];

    return (*phwCRCValue = hwCRC16);

}
#endif


#if CRC16_MODBUS_OPTIMIZE == CRC_OPT_SIZE
/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_modbus_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        hwCRC16 = ((*phwCRCValue) ^ (*pchData++)) & 0x00FF;        
        hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
        for (j = 0; j < 8; j++) {
            if (hwCRC16 & 0x8000) {
                hwCRC16 <<= 1;
                hwCRC16 ^= CRC16_POLY;
            } else {
                hwCRC16 <<= 1;
            }
        }
         hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
        *phwCRCValue = (*phwCRCValue >> 8) ^ hwCRC16;
    }

    return *phwCRCValue;
}


/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_modbus_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = ((*phwCRCValue) ^ chData) & 0x00FF;        
    hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
    for (i = 0; i < 8; i++) {
        if (hwCRC16 & 0x8000) {
            hwCRC16 <<= 1;
            hwCRC16 ^= CRC16_POLY;
        } else {
            hwCRC16 <<= 1;
        }
    }
     hwCRC16 = (uint16_t)data_reversal(hwCRC16, 16);
    *phwCRCValue = (*phwCRCValue >> 8) ^ hwCRC16;
    
    return *phwCRCValue;
}
#endif


#if CRC16_MODBUS_OPTIMIZE == CRC_OPT_BALANCE
/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_modbus_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (*pchData & 0x0F)];
        hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (*pchData++ >> 4)];
    }

    return (*phwCRCValue = hwCRC16);
} 


/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_modbus_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (chData & 0x0F)];
    hwCRC16 = (hwCRC16 >> 4) ^ c_hwCRC16Tab[(hwCRC16 & 0x000F) ^ (chData >> 4)];

    return (*phwCRCValue = hwCRC16);
} 
#endif


#if CRC16_MODBUS_OPTIMIZE == CRC_OPT_SPEED
/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_modbus_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 >> 8) ^ c_hwCRC16Table[(hwCRC16 ^ *pchData++) & 0xFF];
    }

    return (*phwCRCValue = hwCRC16);

}  


/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_modbus_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 >> 8) ^ c_hwCRC16Table[(hwCRC16 ^ chData) & 0xFF];

    return (*phwCRCValue = hwCRC16);

}
#endif


#if CRC16_CCITT_FALSE_OPTIMIZE == CRC_OPT_SIZE
/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_ccitt_false_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        hwCRC16 = ((uint16_t)(*pchData++) << 8) ^ (*phwCRCValue & 0xFF00);
        for (j = 0; j < 8; j++) {
            if (hwCRC16 & 0x8000) {
                hwCRC16 <<= 1;
                hwCRC16 ^= CRC16_CCITT_FALSE_POLY;
            } else {
                hwCRC16 <<= 1;
            }
        }
        *phwCRCValue = (*phwCRCValue << 8) ^ hwCRC16;
    }

    return *phwCRCValue;
}


/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC16 result
 */
uint16_t crc16_ccitt_false_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = ((uint32_t)chData << 8) ^ (*phwCRCValue & 0xFF00);
    for (i = 0; i < 8; i++) {
        if (hwCRC16 & 0x8000) {
            hwCRC16 <<= 1;
            hwCRC16 ^= CRC16_CCITT_FALSE_POLY;
        } else {
            hwCRC16 <<= 1;
        }
    }
    *phwCRCValue = (*phwCRCValue << 8) ^ hwCRC16;
    
    return *phwCRCValue;
}
#endif


#if CRC16_CCITT_FALSE_OPTIMIZE == CRC_OPT_BALANCE
/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_ccitt_false_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (*pchData >> 4)];
        hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (*pchData++ & 0x0F)];
    }

    return (*phwCRCValue = hwCRC16);
} 


/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \return CRC16 result
 */
uint16_t crc16_ccitt_false_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (chData >> 4)];
    hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (chData & 0x0F)];

    return (*phwCRCValue = hwCRC16);
} 
#endif


#if CRC16_CCITT_FALSE_OPTIMIZE == CRC_OPT_SPEED
/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_ccitt_false_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 << 8) ^ c_hwCRC16_CCITT_Table[(hwCRC16 >> 8) ^ *pchData++];
    }

    return (*phwCRCValue = hwCRC16);
}  


/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_ccitt_false_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 << 8) ^ c_hwCRC16_CCITT_Table[(hwCRC16 >> 8) ^ chData];

    return (*phwCRCValue = hwCRC16);

}
#endif


#if CRC16_MODEM_OPTIMIZE == CRC_OPT_SIZE
/*! \brief CRC16 XMODEM or ZMODEM or ACORN
 *!        Poly: 0x1021
 *!        Init: 0x0000
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_modem_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i,j;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    for (i = 0; i < wLength; i++) {
        hwCRC16 = ((uint16_t)(*pchData++) << 8) ^ (*phwCRCValue & 0xFF00);
        for (j = 0; j < 8; j++) {
            if (hwCRC16 & 0x8000) {
                hwCRC16 <<= 1;
                hwCRC16 ^= CRC16_CCITT_FALSE_POLY;
            } else {
                hwCRC16 <<= 1;
            }
        }
        *phwCRCValue = (*phwCRCValue << 8) ^ hwCRC16;
    }

    return *phwCRCValue;
}


/*! \brief CRC16 XMODEM or ZMODEM or ACORN
 *!        Poly: 0x1021
 *!        Init: 0x0000
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC16 result
 */
uint16_t crc16_modem_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = ((uint32_t)chData << 8) ^ (*phwCRCValue & 0xFF00);
    for (i = 0; i < 8; i++) {
        if (hwCRC16 & 0x8000) {
            hwCRC16 <<= 1;
            hwCRC16 ^= CRC16_CCITT_FALSE_POLY;
        } else {
            hwCRC16 <<= 1;
        }
    }
    *phwCRCValue = (*phwCRCValue << 8) ^ hwCRC16;
    
    return *phwCRCValue;
}
#endif


#if CRC16_MODEM_OPTIMIZE == CRC_OPT_BALANCE
/*! \brief CRC16 XMODEM or ZMODEM or ACORN
 *!        Poly: 0x1021
 *!        Init: 0x0000
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_modem_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16 = 0;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (*pchData >> 4)];
        hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (*pchData++ & 0x0F)];
    }

    return (*phwCRCValue = hwCRC16);
} 


/*! \brief CRC16 XMODEM or ZMODEM or ACORN
 *!        Poly: 0x1021
 *!        Init: 0x0000
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \return CRC16 result
 */
uint16_t crc16_modem_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16 = 0;

    if (NULL == phwCRCValue) {
        return false;
    }
    
    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (chData >> 4)];
    hwCRC16 = (hwCRC16 << 4) ^ c_hwCRC16_CCITT_Tab[(hwCRC16 >> 12) ^ (chData & 0x0F)];

    return (*phwCRCValue = hwCRC16);
} 
#endif


#if CRC16_MODEM_OPTIMIZE == CRC_OPT_SPEED
/*! \brief CRC16 XMODEM or ZMODEM or ACORN
 *!        Poly: 0x1021
 *!        Init: 0x0000
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
uint16_t crc16_modem_stream_check(uint16_t *phwCRCValue, uint8_t *pchData, uint32_t wLength)
{
    uint32_t i;
    uint16_t hwCRC16;

    if ((NULL == phwCRCValue) || (NULL == pchData)) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    for (i = 0; i < wLength; i++) {
        hwCRC16 = (hwCRC16 << 8) ^ c_hwCRC16_CCITT_Table[(hwCRC16 >> 8) ^ *pchData++];
    }

    return (*phwCRCValue = hwCRC16);
}  


/*! \brief CRC16 XMODEM or ZMODEM or ACORN
 *!        Poly: 0x1021
 *!        Init: 0x0000
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param phwCRCValue CRC Variable
 *! \param pchData target data
 *! \return CRC16 result
 */
uint16_t crc16_modem_check(uint16_t *phwCRCValue, uint8_t chData)
{
    uint16_t hwCRC16;

    if (NULL == phwCRCValue) {
        return false;
    }

    hwCRC16 = *phwCRCValue;
    hwCRC16 = (hwCRC16 << 8) ^ c_hwCRC16_CCITT_Table[(hwCRC16 >> 8) ^ chData];

    return (*phwCRCValue = hwCRC16);

}
#endif

/* EOF */
