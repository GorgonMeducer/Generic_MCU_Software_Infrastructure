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

#ifndef __CRC_H__
#define __CRC_H__

/*============================ INCLUDES ======================================*/
#include ".\app_cfg.h"

/*============================ MACROS ========================================*/
//! \brief  CRC initialized value
#define CRC_INIT                    (0xFFFF)
#define CRC7_INIT                   (0x00)
#define CRC8_INIT                   (0x00)
#define CRC8_ROHC_INIT              (0xFF)

#define CRC16_MODBUS_INIT           (0xFFFF)
#define CRC16_USB_INIT              (0xFFFF)
#define CRC16_CCITT_FALSE_INIT      (0xFFFF)
#define CRC16_MODEM_INIT            (0x0000)

#define CRC32_INIT                  (0xFFFFFFFF)
#define CRC32_IEEE802_3_INIT        (0xFFFFFFFF)


/*============================ MACROFIED FUNCTIONS ===========================*/
#define CRC(__CRCVAL,__NEWCHAR)             crc16_check(&(__CRCVAL),(__NEWCHAR))
#define CRC16(__CRCVAL,__NEWCHAR)           crc16_check(&(__CRCVAL),(__NEWCHAR))
#define CRC8(__CRCVAL,__NEWCHAR)            crc8_check(&(__CRCVAL),(__NEWCHAR))
#define CRC32_IEEE802_3(__CRCVAL,__NEWCHAR) crc32_ieee802_3_check(&(__CRCVAL),(__NEWCHAR))

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

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
extern uint8_t crc7_stream_check( uint8_t  *pchCRCValue, 
                                  uint8_t  *pchData, 
                                  uint32_t wLength );
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
extern uint8_t crc7_check(uint8_t *pchCRCValue, uint8_t chData);

/*! \brief crc16
 *! \param pwCRCValue CRC Variable
 *! \param chData target byte
 *! \return CRC16 result
 */
extern uint16_t crc16_check(uint16_t *pwCRCValue,uint8_t chData);

/*! \brief CRC8
 *! \param pchCRCValue CRC Variable
 *! \param chData target byte
 *! \return CRC8 result
 */
extern uint8_t crc8_check(uint8_t *pchCRCValue, uint8_t chByte);

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
extern uint8_t crc8_stream_check( uint8_t  *pchCRCValue, 
                                  uint8_t  *pchData, 
                                  uint32_t wLength );


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
extern uint8_t crc8_check(uint8_t *pchCRCValue, uint8_t chData);


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
extern uint8_t crc8_rohc_stream_check( uint8_t  *pchCRCValue, 
                                       uint8_t  *pchData,
                                       uint32_t wLength );


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
extern uint8_t crc8_rohc_check(uint8_t *pchCRCValue, uint8_t chData);


/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC16 result
 */
extern uint16_t crc16_ccitt_false_stream_check( uint16_t *phwCRCValue, 
                                                uint8_t  *pchData, 
                                                uint32_t wLength );
                                         

/*! \brief CRC16 CCITT FALSE
 *!        Poly: 0x1021
 *!        Init: 0xFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x0000
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC16 result
 */
extern uint16_t crc16_ccitt_false_check(uint16_t *phwCRCValue, uint8_t chData);


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
extern uint16_t crc16_usb_stream_check( uint16_t *phwCRCValue, 
                                        uint8_t  *pchData, 
                                        uint32_t wLength );


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
extern uint16_t crc16_usb_check(uint16_t *phwCRCValue, uint8_t chData);


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
extern uint16_t crc16_modbus_stream_check( uint16_t *phwCRCValue, 
                                           uint8_t  *pchData, 
                                           uint32_t wLength );

/*! \brief CRC16 MODBUS
 *!        Poly: 0x8005
 *!        Init: 0xFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0x0000
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC16 result
 */
extern uint16_t crc16_modbus_check(uint16_t *phwCRCValue, uint8_t chData);

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
extern uint16_t crc16_modem_stream_check( uint16_t *phwCRCValue, 
                                          uint8_t *pchData, 
                                          uint32_t wLength );


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
extern uint16_t crc16_modem_check(uint16_t *phwCRCValue, uint8_t chData);

/*! \brief CRC32 IEEE802.3
 *!        Poly: 0x04C11DB7
 *!        Init: 0xFFFFFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00000000
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC32 result
 */
extern uint32_t crc32_ieee802_3_stream_check( uint32_t *pwCRCValue, 
                                              uint8_t  *pchData, 
                                              uint32_t wLength );

/*! \brief CRC32 IEEE802.3
 *!        Poly: 0x04C11DB7
 *!        Init: 0xFFFFFFFF
 *!        RefIn: False
 *!        RefOut: False
 *!        XorOut: 0x00000000
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC32 result
 */
extern uint32_t crc32_ieee802_3_check(uint32_t *pwCRCValue, uint8_t chData);


/*! \brief CRC32 
 *!        Poly: 0x04C11DB7
 *!        Init: 0xFFFFFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFFFFFF
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data stream address
 *! \param wLength the number of target data stream
 *! \return CRC32 result
 */
extern uint32_t crc32_stream_check( uint32_t *pwCRCValue, 
                                    uint8_t  *pchData, 
                                    uint32_t wLength );

/*! \brief CRC32 
 *!        Poly: 0x04C11DB7
 *!        Init: 0xFFFFFFFF
 *!        RefIn: True
 *!        RefOut: True
 *!        XorOut: 0xFFFFFFFF
 *! \param pwCRCValue CRC Variable
 *! \param pchData target data byte
 *! \return CRC32 result
 */
extern uint32_t crc32_check(uint32_t *pwCRCValue, uint8_t chData);



#endif

/* EOF */
