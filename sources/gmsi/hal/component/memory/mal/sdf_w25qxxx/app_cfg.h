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



//! \brief import up level configuration
#include "..\app_cfg.h"

//! \brief component configuration
#ifndef _USE_SERIAL_DATA_FLASH_CONFIG_H_
#define _USE_SERIAL_DATA_FLASH_CONFIG_H_



//! \name data flash access command
//! @{
#define SDF_CMD_PROG_PAGE           (0x02u)         //!< page programming
#define SDF_CMD_READ                (0x03u)         //!< read memory
#define SDF_CMD_DISABLE_WRITE       (0x04u)         //!< disable write
#define SDF_CMD_READ_STATUS_REG     (0x05u)         //!< read status register 1
#define SDF_CMD_ENABLE_WRITE        (0x06u)         //!< enable write
#define SDF_CMD_SECTOR_ERASE        (0x20u)         //!< erase sector
#define SDF_CMD_CHIP_ERASE          (0xC7u)         //!< erase chip 
#define SDF_CMD_READ_UNIQUE_ID      (0x4Bu)         //!< read unique-id
#define SDF_CMD_READ_DEVICE_ID      (0x90u)         //!< read device ID
//! @}

//! \brief status register bit definition
//! @{
#define BUSY                        (1 << 0)        //!< busy flag
#define WEL                         (1 << 1)        //!< write enable latch flag
//! @}

#endif
