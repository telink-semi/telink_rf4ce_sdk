/********************************************************************************************************
 * @file    mspi.h
 *
 * @brief   This is the header file for B86
 *
 * @author	Zigbee GROUP
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#ifndef _MSPI_H_
#define _MSPI_H_
#include "register_826x.h"
#include "bsp_826x.h"

/** define data register of the master spi interface */ 
#define REG_MASTER_SPI_DATA     REG_ADDR8(0x0c)
/** define control register of the master spi interface */
#define REG_MASTER_SPI_CTRL     REG_ADDR8(0x0d)



static inline void MSPI_Wait(void)
{
    while(REG_MASTER_SPI_CTRL & FLD_MASTER_SPI_BUSY);
}

static inline void MSPI_High(void)
{
    REG_MASTER_SPI_CTRL = FLD_MASTER_SPI_CS;
}

static inline void MSPI_Low(void) {
    REG_MASTER_SPI_CTRL = 0;
}

static inline unsigned char MSPI_Get(void)
{
    return REG_MASTER_SPI_DATA;
}

static inline void MSPI_Write(unsigned char c)
{
    REG_MASTER_SPI_DATA = c;
}

static inline void MSPI_CtrlWrite(unsigned char c)
{
    REG_MASTER_SPI_CTRL = c;
}

static inline unsigned char MSPI_Read(void)
{
    MSPI_Write(0);      // dummy, issue clock 
    MSPI_Wait();
    return MSPI_Get();
}

#endif /* _MSPI_H_ */
