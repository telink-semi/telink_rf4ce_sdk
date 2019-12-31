
#pragma once

#include "../common/types.h"
#include "../common/compiler.h"


typedef void (*drv_flash_write)(unsigned long addr, unsigned long len, unsigned char *buf);
typedef void (*drv_flash_read)(unsigned long addr, unsigned long len, unsigned char *buf);
typedef void (*drv_flash_erase)(unsigned long addr);

typedef struct{
	drv_flash_write write;
	drv_flash_read read;
	drv_flash_erase erase;
}drv_flash_t;

void flash_write(u32 addr, u32 len, u8 *buf);
void flash_read(u32 addr, u32 len, u8 *buf);
void flash_erase(u32 addr);


