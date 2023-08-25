/********************************************************************************************************
 * @file    drv_flash.c
 *
 * @brief   This is the source file for drv_flash.c
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
#include "drv_flash.h"



#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
const drv_flash_t myFlashDrv = {
	.write = flash_write_page,
	.read = flash_read_page,
	.erase = flash_erase_sector,
};
#elif defined(MCU_CORE_B92)
///**
// * @brief 		This function serve to change the read function and write function.
// * @param[in]   read	- the read function.
// * @param[in]   write	- the write function.
// * @none
// */
//static inline void drv_flash_func_init(drv_flash_write read, drv_flash_read write,drv_flash_erase erase)
//{
//	if(read)
//		myFlashDrv.read =  read;
//	if(write)
//		myFlashDrv.write = write;
//	if(erase)
//		myFlashDrv.erase = erase;
//}
const drv_flash_t myFlashDrv = {
	.write = flash_write_page,
	.read = flash_read_page,
	.erase = flash_erase_sector,
};
#endif



void flash_op(u8 opmode, u32 addr, u32 len, u8 *buf){
	u32 re = addr%256;

	u32 pageReLen = (re)?(256 -re):256;

	u32 writeLen = 0;
	u32 remainLen = len;

	do{
		if(remainLen <= pageReLen){
			writeLen = remainLen;
			remainLen = 0;
		}else{
			remainLen -= pageReLen;
			writeLen = pageReLen;
			pageReLen = 256;
		}
		if(opmode){
			myFlashDrv.write(addr,writeLen,buf);
		}else{
			myFlashDrv.read(addr,writeLen,buf);
		}
		buf += writeLen;
		addr += writeLen;
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
	}while(remainLen);
}

void flash_write(u32 addr, u32 len, u8 *buf){
	flash_op(1, addr, len, buf);
}


bool flash_writeWithCheck(u32 addr, u32 len, u8 *buf){
	s32 toalLen = (s32)len;
	s8 copyLen = 48;
	u8 pTemp[48];
	u8 wLen = 0;
	u32 sAddr = (u32)addr;
	u8 *pSrc = buf;


#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif


	flash_write_page(addr, len, buf);

	while(toalLen > 0){
		wLen = (toalLen > copyLen) ? copyLen : toalLen;
		flash_read(sAddr, wLen, pTemp);
		if(memcmp(pTemp, pSrc, wLen)){
			return FALSE;
		}
		toalLen -= wLen;
		sAddr += wLen;
		pSrc += wLen;
	}
	return TRUE;
}





void flash_read(u32 addr, u32 len, u8 *buf){
	flash_op(0, addr, len, buf);
}

void flash_erase(u32 addr){
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif

	myFlashDrv.erase(addr);
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
}


//loack all capacity
bool flash_lock(void){
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	u32 mid = flash_read_mid();
	u16 data = 0;
	bool ret = FALSE;
	switch(mid)
	{
#if defined (MCU_CORE_826x)
	case 0x1360c8://GD 512K
	case 0x13325e://ZB 512K
	case 0x134051://MD 512K
		data = (flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)& ~(0x1c))|0x1c;
		flash_write_status(FLASH_TYPE_8BIT_STATUS,data);
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&0x1c;
		ret = ((data==0x1c)?TRUE:FALSE);
		break;
#endif

#if defined (MCU_CORE_8278)
//	case 0x1160c8://128K do not support
	case 0x1360c8://GD 512K
	case 0x1460c8://GD 1024K
//	case 0x11325e://128K do not support
	case 0x13325e://ZB 512K
	case 0x14325e://ZB 1024K
	    data = (flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)& ~(0x1c))|0x1c;
		flash_write_status(FLASH_TYPE_8BIT_STATUS, data);
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&0x1c;
		ret = ((data==0x1c)?TRUE:FALSE);
		break;
#endif

#if defined (MCU_CORE_8258)
//	case 0x1060c8://GD 64K do not support
	case 0x13325e://ZB 512K
	case 0x134051://MD 512K
	case 0x1360c8://GD 512K
	case 0x14325e://ZB 512K
	case 0x1460c8://GD 1024K
	    data = (flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)& ~(0x1c))|(0x1c);
		flash_write_status(FLASH_TYPE_8BIT_STATUS, data);
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&(0x1c);
		ret = ((data==0x1c)?TRUE:FALSE);
		break;

	case 0x136085://P 512K
	case 0x1360eb://TH 512K
	case 0x011460c8://GD 1024K
	    data = ((flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)|(flash_read_status(FLASH_READ_STATUS_CMD_HIGHBYTE)<<8))& ~(0x407c))|(0x407c);
		flash_write_status(FLASH_TYPE_16BIT_STATUS_ONE_CMD, data);
		data = (flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)|(flash_read_status(FLASH_READ_STATUS_CMD_HIGHBYTE)<<8))&(0x407c);
		ret = ((data==0x407c)?TRUE:FALSE);
		break;
#endif

#if defined (MCU_CORE_B92)
	case 0x146085://1M
		flash_lock_mid146085(FLASH_LOCK_ALL_1M_MID146085);
		break;

	case 0x156085://2M
		flash_lock_mid156085(FLASH_LOCK_ALL_2M_MID156085);
		break;

	case 0x1560c8://2M
		flash_lock_mid1560c8(FLASH_LOCK_ALL_2M_MID1560c8);
		break;

	case 0x166085://4M
		flash_lock_mid166085(FLASH_LOCK_ALL_4M_MID166085);
		break;

	case 0x186085://16M
		flash_lock_mid186085(FLASH_LOCK_ALL_16M_MID186085);
		break;
#endif
	default:
		break;
	}
	return ret;
}





bool flash_unlock(void){
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	u32 mid = flash_read_mid();
	u16 data = 0;
	bool ret = FALSE;
	switch(mid)
	{
#if defined (MCU_CORE_826x)
	case 0x1360c8://GD 512K
	case 0x13325e://ZB 512K
	case 0x134051://MD 512K
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&(~0x1c);
		flash_write_status(FLASH_TYPE_8BIT_STATUS,data);
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&0x1c;
		ret = ((data==0)?TRUE:FALSE);
		break;
#endif

#if defined (MCU_CORE_8278)
//	case 0x1160c8://128K do not support
	case 0x1360c8://GD 512K
	case 0x1460c8://GD 1024K
//	case 0x11325e://128K do not support
	case 0x13325e://ZB 512K
	case 0x14325e://ZB 1024K
		 data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&(~0x1c);
		flash_write_status(FLASH_TYPE_8BIT_STATUS, data);
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)& 0x1c;
		ret = ((data==0)?TRUE:FALSE);
		break;
#endif

#if defined (MCU_CORE_8258)
//	case 0x1060c8://GD 64K do not support
	case 0x13325e://ZB 512K
	case 0x134051://MD 512K
	case 0x1360c8://GD 512K
	case 0x14325e://ZB 512K
	case 0x1460c8://GD 1024K
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&(~0x1c);
		flash_write_status(FLASH_TYPE_8BIT_STATUS, data);
		data = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)&0x1c;
		ret = ((data==0)?TRUE:FALSE);
		break;

	case 0x136085://P 512K
	case 0x1360eb://TH 512K
	case 0x011460c8://GD 1024K
		data = ((flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)|(flash_read_status(FLASH_READ_STATUS_CMD_HIGHBYTE)<<8))& ~(0x407c));
		flash_write_status(FLASH_TYPE_16BIT_STATUS_ONE_CMD, data);
		data = (flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE)|(flash_read_status(FLASH_READ_STATUS_CMD_HIGHBYTE)<<8))&0x407c;
		ret = ((data==0)?TRUE:FALSE);
		break;
#endif

#if defined (MCU_CORE_B92)
	case 0x146085://1M
		flash_unlock_mid146085();
		break;

	case 0x156085://2M
		flash_unlock_mid156085();
		break;

	case 0x1560c8://2M
		flash_unlock_mid1560c8();
		break;

	case 0x166085://4M
		flash_unlock_mid166085();
		break;

	case 0x186085://16M
		flash_unlock_mid186085();
		break;
#endif

	default:
		break;
	}
	return ret;
}
