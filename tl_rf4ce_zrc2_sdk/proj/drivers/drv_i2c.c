/********************************************************************************************************
 * @file	drv_i2c.c
 *
 * @brief	This is the source file for drv_i2c
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
