/********************************************************************************************************
 * @file    drv_spi.h
 *
 * @brief   This is the header file for drv_spi.h
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

#pragma once



/**
 *  @brief  Define the mode for SPI interface
 */
typedef enum {
    SPI_MODE_0 = 0,
    SPI_MODE_2,
    SPI_MODE_1,
    SPI_MODE_3,
}drv_spi_mode_type_def;

/**
 *  @brief  Define the pin port for SPI interface
 */
typedef enum {
    SPI_PIN_GPIO1 = 0,	//826x:SPI_PIN_GPIOA 8258/8278:SPI_GPIO_GROUP_A2A3A4D6
    SPI_PIN_GPIO2,		//826x:SPI_PIN_GPIOB 8258/8278:SPI_GPIO_GROUP_B6B7D2D7
} drv_spi_pin_group;

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] divClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] mode - the selected working mode of SPI module,mode 0~mode 3
 * @return    none
 */
void drv_spi_master_init(u8 divClock, drv_spi_mode_type_def mode);

#if	defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
/**
 * @brief     This function selects a pin port for the SPI interface
 * @param[in] PinGrp - the selected pin group port
 * @return    none
 */
void drv_spi_master_pin_select(drv_spi_pin_group pinGrp);
#elif defined(MCU_CORE_8278)
/**
 * @brief     This function selects a pin port for the SPI interface
 * @param[in]
 * @return    none
 */
void drv_spi_master_pin_select(SPI_GPIO_SclkTypeDef sclk_pin,SPI_GPIO_CsTypeDef cs_pin,SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin);
#endif


/**
 * @brief     This function selects a GPIO pin as CS of SPI function.
 * @param[in] CSPin - the selected CS pin
 * @return    none
 */
#if	defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
void drv_spi_master_cspin_select(GPIO_PinTypeDef CSPin);
#endif

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] divClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] mode - the selected working mode of SPI module,mode 0~mode 3
 * @return    none
 */
void drv_spi_slave_init(u8 divClock, drv_spi_mode_type_def mode);

#if	defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
/**
 * @brief     This function selects a pin port for the SPI interface
 * @param[in] pinGrp - the selected pin port
 * @return    none
 */
void drv_spi_slave_pin_select(drv_spi_pin_group pinGrp);
#elif defined(MCU_CORE_8278)
void drv_spi_slave_pin_select(SPI_GPIO_SclkTypeDef sclk_pin, SPI_GPIO_CsTypeDef cs_pin, SPI_GPIO_SdoTypeDef sdo_pin, SPI_GPIO_SdiTypeDef sdi_pin);
#endif

/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  cmd - pointer to the command bytes needed written into the
 *             slave device first before the writing operation of actual data
 * @param[in]  cmdLen - length in byte of the command bytes
 * @param[in]  data - pointer to the data need to write
 * @param[in]  dataLen - length in byte of the data need to write
 * @param[in]  csPin - the CS pin specific the slave device
 * @return     none
 */
void drv_spi_write(u8 *cmd, int cmdLen, u8 *data, int dataLen, u32 csPin);


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
void drv_spi_read(u8 *cmd, int cmdLen, u8 *data, int dataLen, u32 csPin);

