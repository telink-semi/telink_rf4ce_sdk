/********************************************************************************************************
 * @file    tl_putchar.c
 *
 * @brief   This is the source file for tl_putchar.c
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

#include "tl_putchar.h"
#include "../tl_common.h"


#ifndef		BIT_INTERVAL
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
#define BAUDRATE			1000000//1M
#define	BIT_INTERVAL	 	(16*1000*1000 / BAUDRATE)
#else
#define BAUDRATE			2000000//2M
#define	BIT_INTERVAL	 	(CLOCK_SYS_CLOCK_HZ / BAUDRATE)
#endif
#endif

_attribute_ram_code_ int soft_uart_putc(char byte) {
#if UART_PRINTF_MODE
	u8 j = 0;
	u32 t1 = 0, t2 = 0;
	u32 pcTxReg = (0x800583 + ((DEBUG_INFO_TX_PIN >> 8) << 3));; //register GPIO output
	u8 tmp_bit0 = read_reg8(pcTxReg) & (~(DEBUG_INFO_TX_PIN & 0xff));
	u8 tmp_bit1 = read_reg8(pcTxReg) | (DEBUG_INFO_TX_PIN & 0xff);

	u8 bit[10] = {0};
	bit[0] = tmp_bit0;
	bit[1] = (byte & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[2] = ((byte >> 1) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[3] = ((byte >> 2) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[4] = ((byte >> 3) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[5] = ((byte >> 4) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[6] = ((byte >> 5) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[7] = ((byte >> 6) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[8] = ((byte >> 7) & 0x01) ? tmp_bit1 : tmp_bit0;
	bit[9] = tmp_bit1;
	//u8 r = irq_disable();// enable this may disturb time sequence, but if disable unrecognizable code will show
	t1 = reg_system_tick;
	for(j = 0; j < 10; j++) {
		t2 = t1;

		while(t1 - t2 < BIT_INTERVAL) {
			t1  = reg_system_tick;
		}

		write_reg8(pcTxReg, bit[j]);       //send bit0
	}
	//irq_restore(r);
	return byte;

#else
	return -1;
#endif
}

int putchar(int c) {
#if UART_PRINTF_MODE
	return soft_uart_putc((char)c);
#elif USB_PRINTF_MODE
	return hw_usb_putc((char)c);
#endif
    return -1;
}

