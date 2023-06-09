/********************************************************************************************************
 * @file    drv_i2c.c
 *
 * @brief   This is the source file for drv_i2c.c
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

#include "../tl_common.h"
#include "./drv_i2c.h"

/**
 * @brief      This function set the id of slave device and the speed of I2C interface
 *             note: the param ID contain the bit of writting or reading.
 *             eg:the parameter 0x5C. the reading will be 0x5D and writting 0x5C.
 * @param[in]  SlaveID - the id of slave device.it contains write or read bit,the lsb is write or read bit.
 *                       ID|0x01 indicate read. ID&0xfe indicate write.
 * @param[in]  DivClock - the division factor of I2C clock,
 *             I2C clock = System clock / (4*DivClock);if the datasheet you look at is 2*,pls modify it.
 * @return     none
 */
void drv_i2c_master_init(unsigned char SlaveID, unsigned char DivClock)
{
#if	defined (MCU_CORE_826x)
	I2C_MasterInit(SlaveID, DivClock);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	i2c_master_init(SlaveID, DivClock);
#endif
}

/**
 *  @brief      the function config the ID of slave and mode of slave.
 *  @param[in]  device_ID - it contains write or read bit,the lsb is write or read bit.
 *              ID|0x01 indicate read. ID&0xfe indicate write.
 *  @param[in]  mode - set slave mode. slave has two modes, one is DMA mode, the other is MAPPING mode.
 *  @param[in]  pBuf - if slave mode is MAPPING, set the first address of buffer master write or read slave.
 *  @return     none
 */
void drv_i2c_slave_init(unsigned char device_ID, I2C_SlaveMode mode, unsigned char *pMapBuf)
{
#if	defined (MCU_CORE_826x)
	I2C_SlaveInit(device_ID, mode, pMapBuf);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	i2c_slave_init(device_ID, mode, pMapBuf);
#endif
}

/**
 * @brief      This function writes one byte to the slave device at the specified address
 * @param[in]  Addr - i2c slave address where the one byte data will be written
 * @param[in]  AddrLen - length in byte of the address, which makes this function is
 *             compatible for slave device with both one-byte address and two-byte address
 * @param[in]  Data - the one byte data will be written via I2C interface
 * @return     none
 */
void drv_i2c_write_byte(unsigned int Addr, unsigned int AddrLen, unsigned char Data)
{
#if	defined (MCU_CORE_826x)
	I2C_WriteByte(Addr, AddrLen, Data);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	i2c_write_byte(Addr, AddrLen, Data);
#endif
}



/**
 *  @brief      write continous data to slave
 *  @param[in]  Addr - the register that master write data to slave in. support one byte and two bytes. i.e param2 AddrLen may be 1 or 2.
 *  @param[in]  AddrLen - the length of register. enum 0 or 1 or 2 or 3. based on the spec of i2c slave.
 *  @param[in]  dataBuf - the first SRAM buffer address to write data to slave in.
 *  @param[in]  dataLen - the length of data master write to slave.
 *  @return     none
 */
void drv_i2c_write_series(unsigned int Addr, unsigned int AddrLen, unsigned char *dataBuf, int dataLen)
{
#if	defined (MCU_CORE_826x)
	I2C_Write(Addr, AddrLen, dataBuf, dataLen);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	i2c_write_series(Addr, AddrLen, dataBuf, dataLen);
#endif
}




/**
 * @brief      This function reads one byte from the slave device at the specified address
 * @param[in]  Addr - i2c slave address where the one byte data will be read
 * @param[in]  AddrLen - length in byte of the address, which makes this function is
 *             compatible for slave device with both one-byte address and two-byte address
 * @return     the one byte data read from the slave device via I2C interface
 */
unsigned char drv_i2c_read_byte(unsigned int Addr, unsigned int AddrLen)
{
#if	defined (MCU_CORE_826x)
	return I2C_ReadByte(Addr, AddrLen);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	return i2c_read_byte(Addr, AddrLen);
#endif
}



/**
 * @brief      read continous data from slave
 * @param[in]  Addr - the register master read data from slave in. support one byte and two bytes.
 * @param[in]  AddrLen - the length of register. enum 0 or 1 or 2 or 3 based on the spec of i2c slave.
 * @param[in]  dataBuf - the first address of SRAM buffer master store data in.
 * @param[in]  dataLen - the length of data master read from slave.
 * @return     none.
 */
void drv_i2c_read_series(unsigned int Addr, unsigned int AddrLen, unsigned char *dataBuf, int dataLen)
{
#if	defined (MCU_CORE_826x)
	I2C_Read(Addr, AddrLen, dataBuf, dataLen);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	i2c_read_series(Addr, AddrLen, dataBuf, dataLen);
#endif
}



#if	defined (MCU_CORE_826x)||defined(MCU_CORE_8258)
/**
 * @brief      This function selects a pin port for I2C interface.
 * @param[in]  PinGrp - the pin port selected as I2C interface pin port.
 * @return     none
 * 	A3:5b7[0] set 1 as spi input,set 0 not as spi input ;5b7[4] set 1 as i2c input ,set 0 not as i2c input
 *	A4:5b7[1] set 1 as spi input,set 0 not as spi input ;5b7[5] set 1 as i2c input ,set 0 not as i3c input
 *	B6:5b7[2] set 1 as spi input,set 0 not as spi input ;5b7[6] set 1 as i2c input ,set 0 not as i4c input
 *	D7:5b7[3] set 1 as spi input,set 0 not as spi input ;5b7[7] set 1 as i2c input ,set 0 not as i5c input
 */
void drv_i2c_gpio_set(I2C_GPIO_GroupTypeDef i2c_pin_group)
{
#if	defined (MCU_CORE_826x)
	I2C_PinSelect(i2c_pin_group);
#elif defined(MCU_CORE_8258)
	i2c_gpio_set(i2c_pin_group);
#endif
}
#elif defined(MCU_CORE_8278)
/**
 * @brief      This function selects a pin port for I2C interface.
 * @param[in]  i2c_pin_group - the pin port selected as I2C interface pin port.
 * @return     none
 * 	A3:5b7[0] set 1 as spi input,set 0 not as spi input ;5b7[4] set 1 as i2c input ,set 0 not as i2c input
 *	A4:5b7[1] set 1 as spi input,set 0 not as spi input ;5b7[5] set 1 as i2c input ,set 0 not as i3c input
 *	B6:5b7[2] set 1 as spi input,set 0 not as spi input ;5b7[6] set 1 as i2c input ,set 0 not as i4c input
 *	D7:5b7[3] set 1 as spi input,set 0 not as spi input ;5b7[7] set 1 as i2c input ,set 0 not as i5c input
 */
void drv_i2c_gpio_set(I2C_GPIO_SdaTypeDef sda_pin,I2C_GPIO_SclTypeDef scl_pin)
{
	i2c_gpio_set(sda_pin,scl_pin);
}
#endif
