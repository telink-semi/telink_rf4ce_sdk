/********************************************************************************************************
 * @file     nv.h
 *
 * @brief	 resource allocation in the flash
 *
 * @author
 * @date     Oct. 8, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#pragma once

#include "../common/types.h"


#define NV_ACTIVE_PAGE_FLAGS           0x2192

#define FLASH_4K_PAGE_NUM              16
#define FLASH_PAGE_SIZE                256
#define NV_SUBPAGE_LENGHT				32
#define OTA_IMAGE_SIZE					(192*1024)

#define NV_SECTION_SIZE                (FLASH_PAGE_SIZE * FLASH_4K_PAGE_NUM)

#define FLASH_4K_32B_NUM				(NV_SECTION_SIZE/NV_SUBPAGE_LENGHT)

#define NV_ALIGN_LENTH(len)                ( ( ((len) % 4 ) == 0) ? len : ( ((len)/4 + 1) * 4 ) )
/* 1920*256 = 480K */
/* 384*256 = 92K 0x17000 */
#define START_PAGE_FOR_NV       (768 * FLASH_PAGE_SIZE)
#define START_PAGE_FOR_IR       (384 * 1024)
#define END_PAGE_FOR_IR       	(471 * 1024)
#define START_PAGE_FOR_IR_IDX  	START_PAGE_FOR_IR
#define START_PAGE_FOR_IR_DAT  	(START_PAGE_FOR_IR + 1024)
#define FLASH_TOTAL_SIZE        ((512 * 1024) - 1)

#define CFG_FREQUENCY_OFFSET			(0x77000)
#define CFG_TP_GAIN                  	(0x77042)

#define	FLASH_OTA_NEWIMAGE_ADDR			0x40000//256K

#define	TELINK_START_FLAG_OFFSET		0x08

#define NV_HEADER_TABLE_SIZE           10

#define INVALID_NV_VALUE               0xFF

#define MOUDLES_START_ADDR(modules)   (START_PAGE_FOR_NV + 2 * NV_SECTION_SIZE * (modules))

#define MOUDLES_SECOND_HALF_ADDR(modules)     (START_PAGE_FOR_NV + NV_SECTION_SIZE * (1 + 2 * (modules)))

#define MOUDLES_AVALIABLE_SIZE(usedSize)  (FLASH_4K_PAGE_NUM * FLASH_PAGE_SIZE - (usedSize))

#define PAGE_AVALIABLE_SIZE(offset)       (FLASH_PAGE_SIZE - (offset % FLASH_PAGE_SIZE))

#define PAGE_HEADER_SISE  (sizeof(page_header_t))
#define NV_HEADER_SIZE  (sizeof(nv_header_t))

#define READ_BYTES_PER_TIME                 16


#define NV_NEXT_PAGE_ADDR(addr)            ( ((addr)/FLASH_PAGE_SIZE + 1) * FLASH_PAGE_SIZE)

#define NV_NEXT_SECTOR_ADDR(addr)            ( ((addr)/NV_SECTION_SIZE + 1) * NV_SECTION_SIZE)
#define NV_CURRENT_SECTOR_ADDR(addr)            ( ((addr)/NV_SECTION_SIZE) * NV_SECTION_SIZE)

#define NV_PAGE_HEADER_LEN                 4

#define NV_IN_SAME_SECTOR(a,b)             ( (a)/NV_SECTION_SIZE == (b)/NV_SECTION_SIZE )

#define FLASH_GLOBAL_PROTECT_GD         0x6c  // protect , gigadevice GD25Q40B/20B
#define FLASH_GLOBAL_PROTECT_AT         0x3c  // protect , adesto      AT25TE002/004

#define FLASH_PROTECT_NONE              0x00  // unprotect


#define FLASH_PROTECT_UPPER_32_KB                0x00

#define FLASH_FOR_ACTION_MAPPING				 0x7A000 //0x78000
#define FLASH_FOR_TEMP_ACTION_MAPPING			 0x7B000  //0x79000

typedef enum {
    DS_MAC_PHY_MODULE = 0,
    DS_NWK_MODULE = 1,
    DS_PROFILE_MODULE = 2,
	DS_USER_MODULS = 3,
#if (RF4CE_MSO_ENABLE)
	DS_MSO_IR_RF_DB = DS_PROFILE_MODULE,
#endif

#if (RF4CE_ZRC2_ENABLE)
    DS_ZRC2_ATTR_DB = DS_PROFILE_MODULE,
#endif
    DS_APP_LAYER = 4,
    DS_NWK_FRAMECOUNT_MODULE = 5,
    DS_MAX_MODULS,
} nv_module_t;

typedef struct {
    u16 offset;
    u8  len;
    u8 id;
} nv_header_t;


typedef struct {
    nv_header_t hdrList[NV_HEADER_TABLE_SIZE];
    u16 usedSize;
	u16 activeFalgs;
} page_header_t;

typedef struct {
    nv_header_t hdrInfo;
    /* addr of header */
	u32 startAddr;
	u16 usedSize;
	u16 nvOffset;
} item_info_t;

typedef enum nv_sts_e {
	NV_SUCC,
    NV_INVALID_MODULS = 1,
    NV_INVALID_ID ,
    NV_ITEM_NOT_FOUND,
    NV_NOT_ENOUGH_SAPCE,
    NV_ITEM_LEN_NOT_MATCH,
    NV_CHECK_SUM_ERROR,
    NV_ENABLE_PROTECT_ERROR,
    NV_NO_MEDIA
} nv_sts_t;


nv_sts_t nv_write(u8 modules, u8 id, u16 len, u8 *buf);
nv_sts_t nv_init(u8 rst);
nv_sts_t nv_read(u8 modules, u8 id, u16 len, u8 *buf);
nv_sts_t nv_resetModule(u8 modules);

nv_sts_t nv_userSaveToFlash(u8 id, u16 len, u8 *buf);
nv_sts_t nv_userLoadFromFlash(u8 id, u16 len, u8 *buf);
nv_sts_t nv_userReset(void);
void ds_flashWrite(u32 startAddr, u16 len, u8 *buf);

nv_sts_t nv_nwkFrameCountSaveToFlash(u32 frameCount);
nv_sts_t nv_nwkFrameCountFromFlash(u32 *frameCount);


nv_sts_t nv_ir_read(u8 keyCode, u32 *freq, u16 *buf, u16 *len);
nv_sts_t nv_ir_write(u8 keycode, u32 freq, u16 *buf, u16 len);
nv_sts_t nv_ir_reset(void);
