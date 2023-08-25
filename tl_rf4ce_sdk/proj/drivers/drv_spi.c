/********************************************************************************************************
 * @file	drv_spi.c
 *
 * @brief	This is the source file for drv_spi
 *
 * @author	Zigbee Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "../tl_common.h"
#include "drv_spi.h"

#if defined(MCU_CORE_B92)
#define SPI_MODULE_SEL	    			        GSPI_MODULE
#define SPI_CLK	    					  		1000000
#endif

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] spiClock - SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] mode - the selected working mode of SPI module,mode 0~mode 3
 * @return    none
 */
void drv_spi_master_init(u32 spiClock, drv_spi_mode_type_def mode)
{

#if	defined(MCU_CORE_826x)
	u8 divClock = (u8)(SPI_CLOCK_SOURCE / (2 * spiClock) - 1);
	SPI_MasterInit(divClock, mode);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	u8 divClock = (u8)(SPI_CLOCK_SOURCE / (2 * spiClock) - 1);
	spi_master_init(divClock, mode);
#elif defined(MCU_CORE_B92)
	drv_spi_mode_type_def m =  mode;
	spi_master_init(SPI_MODULE_SEL, (u16)(SPI_CLOCK_SOURCE/SPI_CLK), SPI_MODE0);
	spi_master_config(SPI_MODULE_SEL, SPI_SINGLE_MODE);
#endif
}

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] mode - the selected working mode of SPI module,mode 0~mode 3
 * @return    none
 */
void drv_spi_slave_init(drv_spi_mode_type_def mode)
{
#if	defined(MCU_CORE_826x)
	SPI_SlaveInit(0, mode);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_slave_init(0, mode);
#elif defined(MCU_CORE_B92)
	drv_spi_mode_type_def m =  mode;
	spi_slave_init(SPI_MODULE_SEL, SPI_MODE0);
	spi_tx_irq_trig_cnt(SPI_MODULE_SEL,4);
	spi_set_dummy_cnt(SPI_MODULE_SEL, 32);
#endif
}

/**
 * @brief     This function selects a pin port for the SPI master interface
 * @param[in] Pin Group or Pins
 * @return    none
 */
#if	defined(MCU_CORE_826x)
void drv_spi_master_pin_select(SPI_PinTypeDef pinGroup)
{
	SPI_MasterPinSelect(pinGroup);
}
#elif defined(MCU_CORE_8258)
void drv_spi_master_pin_select(SPI_GPIO_GroupTypeDef pinGroup)
{
	spi_master_gpio_set(pinGroup);
}
#elif defined(MCU_CORE_8278)
void drv_spi_master_pin_select(SPI_GPIO_SclkTypeDef sclk_pin, SPI_GPIO_CsTypeDef cs_pin, SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin)
{
	spi_master_gpio_set(sclk_pin, cs_pin, sdo_pin, sdi_pin);
}
#elif defined(MCU_CORE_B92)
void drv_spi_master_pin_select(gpio_pin_e sclk_pin, gpio_pin_e cs_pin, gpio_pin_e mosi_pin, gpio_pin_e miso_pin)
{
	lspi_pin_config_t gspi_pin_config;

	gspi_pin_config.spi_clk_pin			= sclk_pin;
	gspi_pin_config.spi_csn_pin 		= cs_pin;
	gspi_pin_config.spi_mosi_io0_pin    = mosi_pin;
	gspi_pin_config.spi_miso_io1_pin    = miso_pin;//3line mode set 0
	gspi_pin_config.spi_io2_pin     	= NULL;//set quad mode otherwise set 0
	gspi_pin_config.spi_io3_pin   		= NULL;//set quad mode otherwise set 0

	gspi_set_pin(&gspi_pin_config);
}
#endif

/**
 * @brief     This function selects a pin port for the SPI slave interface
 * @param[in] Pin Group or Pins
 * @return    none
 */
#if	defined(MCU_CORE_826x)
void drv_spi_slave_pin_select(SPI_PinTypeDef pinGroup)
{
	SPI_SlavePinSelect(pinGroup);
}
#elif defined(MCU_CORE_8258)
void drv_spi_slave_pin_select(SPI_GPIO_GroupTypeDef pinGroup)
{
	spi_slave_gpio_set(pinGroup);
}
#elif defined(MCU_CORE_8278)
void drv_spi_slave_pin_select(SPI_GPIO_SclkTypeDef sclk_pin, SPI_GPIO_CsTypeDef cs_pin, SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin)
{
	spi_slave_gpio_set(sclk_pin, cs_pin, sdo_pin, sdi_pin);
}
#elif defined(MCU_CORE_B92)
void drv_spi_slave_pin_select(gpio_pin_e sclk_pin, gpio_pin_e cs_pin, gpio_pin_e mosi_pin, gpio_pin_e miso_pin)
{
	lspi_pin_config_t gspi_pin_config;

	gspi_pin_config.spi_clk_pin			= sclk_pin;
	gspi_pin_config.spi_csn_pin 		= cs_pin;
	gspi_pin_config.spi_mosi_io0_pin    = mosi_pin;
	gspi_pin_config.spi_miso_io1_pin    = miso_pin;//3line mode set 0
	gspi_pin_config.spi_io2_pin     	= NULL;//set quad mode otherwise set 0
	gspi_pin_config.spi_io3_pin   		= NULL;//set quad mode otherwise set 0

	gspi_set_pin(&gspi_pin_config);
}
#endif

/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  cmd - pointer to the command bytes needed written into the
 *             slave device first before the writing operation of actual data
 * @param[in]  cmdLen - length in byte of the command bytes
 * @param[in]  data - pointer to the data need to write
 * @param[in]  dataLen - length in byte of the data need to write
 * @param[in]  csPin - the CS pin specifing the slave device
 * @return     none
 */
void drv_spi_write(u8 *cmd, int cmdLen, u8 *data, int dataLen, u32 csPin)
{
#if	defined(MCU_CORE_826x)
	SPI_Write(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_write(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_B92)
	u8 *pBuf = (u8 *)ev_buf_allocate(cmdLen + dataLen);
	if(pBuf){
		u32 pin = csPin;

		u8 *pData = pBuf;

		memcpy(pData, cmd, cmdLen);
		pData += cmdLen;
		memcpy(pData, data, dataLen);
		pData += dataLen;

		spi_master_write(SPI_MODULE_SEL, pBuf, cmdLen + dataLen);

		ev_buf_free(pBuf);
	}
#endif
}


/**
 * @brief      This function serves to read a bulk of data from the SPI slave
 *             device specified by the CS pin
 * @param[in]  cmd - pointer to the command bytes needed written into the
 *             slave device first before the reading operation of actual data
 * @param[in]  cmdLen - length in byte of the command bytes
 * @param[out] data - pointer to the buffer that will cache the reading out data
 * @param[in]  dataLen - length in byte of the data need to read
 * @param[in]  csPin - the CS pin specifing the slave device
 * @return     none
 */
void drv_spi_read(u8 *cmd, int cmdLen, u8 *data, int dataLen, u32 csPin)
{
#if	defined(MCU_CORE_826x)
	SPI_Read(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	spi_read(cmd, cmdLen, data, dataLen, csPin);
#elif defined(MCU_CORE_B92)
	u32 pin = csPin;
	spi_master_write_read(SPI_MODULE_SEL, cmd, cmdLen, data, dataLen);
#endif
}

