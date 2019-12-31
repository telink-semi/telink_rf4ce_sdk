
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

