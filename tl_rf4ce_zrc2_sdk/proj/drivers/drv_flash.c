
#include "../tl_common.h"
#include "drv_flash.h"

const drv_flash_t myFlashDrv = {
	.write = flash_write_page,
	.read = flash_read_page,
	.erase = flash_erase_sector,
};

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

	default:
		break;
	}
	return ret;
}
