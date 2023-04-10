/********************************************************************************************************
 * @file    nv.h
 *
 * @brief   This is the header file for nv.h
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

#include "../common/types.h"


#define FLASH_4K_PAGE_NUM				16

#define FLASH_PAGE_SIZE					256
#define	FLASH_SECTOR_SIZE				4096//4K
#define	NV_SECTION_SIZE					(FLASH_PAGE_SIZE*FLASH_4K_PAGE_NUM)
/* Flash address of MAC address. */
enum{
	MAC_ADDR_512K_FLASH		= 0x76000,
	MAC_ADDR_1M_FLASH		= 0xFF000,
};

/* Flash address of factory pre-configured parameters. */
enum{
	CFG_ADDR_512K_FLASH		= 0x77000,
	CFG_ADDR_1M_FLASH		= 0xFE000,
};

extern u32 g_u32MacFlashAddr;
extern u32 g_u32CfgFlashAddr;

/*******************************************************************************************************
 * Following configuration could NOT be changed by customer.
 */
/* NV modules start address */
#if FLASH_CAP_SIZE_1M
#define NV_BASE_ADDRESS					0x80000
#define MODULES_START_ADDR(id)			(NV_BASE_ADDRESS + FLASH_SECTOR_SIZE * (2 * id))
#else
#define	NV_BASE_ADDRESS					0x34000//start from 208K address
#define	NV_BASE_ADDRESS2				0x78000//start from 488K address
#define MODULES_START_ADDR(id)			((id<6) ? (NV_BASE_ADDRESS + FLASH_SECTOR_SIZE * (2 * id)) : (NV_BASE_ADDRESS2 + FLASH_SECTOR_SIZE * (2 * (id-6))))
#endif


/* Chipset pre-configured parameters */
#define MAC_BASE_ADD					(g_u32MacFlashAddr)
#define FACTORY_CFG_BASE_ADD			(g_u32CfgFlashAddr)

/* 8 bytes for IEEE address */
#define CFG_MAC_ADDRESS              	(MAC_BASE_ADD)
#define CFG_TELINK_USB_ID				(MAC_BASE_ADD + 0x40)

/*0x76180~0x76fff is for vendor specific use, to store parameters which don't need to
 * be calibrated, can be defined by user*/

/*The block start from 0x77000~0x77fff is used to store the parameters which need to be calibrated */

/*0x77000~0x7703f, 1 byte, used for frequency offset calibration*/
#define CFG_FREQUENCY_OFFSET			(FACTORY_CFG_BASE_ADD)
/*0x77040~0x7707F, 2 byte, 0x77040 for BLE TP_GAIN_0, 0x77041 for BLE TP_GAIN_1,
  0x77042 for zigbee TP_GAIN_0, 0x77043 for zigbee TP_GAIN_1 */
#define CFG_TP_GAIN                  	(FACTORY_CFG_BASE_ADD + 0x42)
/*Not supported for current SDK*/
#define CFG_32K_COUNTER_CALIBRATION	 	(FACTORY_CFG_BASE_ADD + 0x80)
/*Not supported for current SDK*/
#define CFG_ADC_CALIBRATION           	(FACTORY_CFG_BASE_ADD + 0xC0)
/*Not supported for current SDK*/
#define CFG_24M_CRYSTAL_CALIBRATION     (FACTORY_CFG_BASE_ADD + 0x100)
/*Not supported for current SDK*/
#define CFG_T_SENSOR_CALIBRATION     	(FACTORY_CFG_BASE_ADD + 0x140)
/*UID-based Firmware Encryption data(16 bytes), 0x77180 ~ 0x7718F*/
#define CFG_FIRMWARE_ENCRYPTION			(FACTORY_CFG_BASE_ADD + 0x180)

/******************************************** END ***********************************************************/


/* flash write protect */
#define FLASH_PROTECT_NONE              0x00  // unprotect
#define FLASH_PROTECT_CMD_GIGADEVICE  	0x6c  //flash of telink evb
#define FLASH_PROTECT_CMD_ADESTO      	0x3c  //flash of customer selection

#define FLASH_PROTECT_CMD       		FLASH_PROTECT_CMD_ADESTO

#define PAGE_AVALIABLE_SIZE(offset)     (FLASH_PAGE_SIZE - offset)

#define NV_HEADER_TABLE_SIZE           	10
#define NV_ALIGN_LENTH(len)             ((((len) % 4) == 0) ? len : (((len)/4 + 1) * 4))

#define INVALID_NV_VALUE            	0xFF

#define NV_SECTOR_VALID					0x5a5a
#define NV_SECTOR_INVALID				0x5050
#define NV_SECTOR_IDLE					0xFFFF

#define ITEM_FIELD_VALID				0x5a
#define ITEM_FIELD_INVALID				0x50
#define ITEM_FIELD_OPERATION			0xFa
#define ITEM_FIELD_IDLE					0xFF

#define FLASH_FIELD_IDLE				0xFFFF
#define FLASH_FIELD_IDLE_WORD			0xFFFFFFFF

/* sector info(4Bytes) + index info(8Bytes) + index info(8Bytes) + ... */
typedef struct{
	u16 usedFlag;
	u8  idName;
	u8  opSect;
}nv_sect_info_t;

typedef struct{
	u32 offset;
	u16 size;
	u8  itemId;
	u8  usedState;
}nv_info_idx_t;

/* item:  item_hdr(8Bytes) + payload*/
typedef struct{
	u32 checkSum;
	u16 size;
	u8  itemId;
	u8  used;
}itemHdr_t;

typedef struct{
	u16 opIndex;
	u8 	opSect;
}itemIfno_t;


//If OTA enabled, the maximum space used for nv module t is 56K, thus the item num cannot over 14
/*****************************************************************************************************************************
 * Store zigbee information in flash.
 * 		Module ID				|			512K Flash				|			1M Flash				|
 * -----------------------------|-----------------------------------|-----------------------------------|
 * DS_MAC_PHY_MODULE			|		0x34000 - 0x36000			|		0x80000 - 0x82000			|
 * DS_NWK_MODULE				|		0x36000 - 0x38000			|		0x82000 - 0x84000			|
 * DS_PROFILE_MODULE			|		0x38000 - 0x3a000			|		0x84000 - 0x86000			|
 * DS_USER_MODULS				|		0x3a000 - 0x3c000			|		0x86000 - 0x88000			|
 * DS_APP_LAYER					|		0x3c000 - 0x3e000			|		0x88000 - 0x8a000			|
 * DS_NWK_FRAMECOUNT_MODULE		|		0x3e000 - 0x40000			|		0x8a000 - 0x8c000			|
 * DS_IR_LEARN_MODULE			|		0x78000 - 0x80000			|		0x8c000 - 0x94000			|
 * 								|	*16K - can store 127 nodes		|	*32K - can store 302 nodes		|
 * NV_MAX_MODULS
 */
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
    DS_IR_LEARN_MODULE = 6,
    DS_MAX_MODULS,
} nv_module_t;

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

#if FLASH_SIZE_1M
#define NV_SECTOR_SIZE(id)						((id == DS_IR_LEARN_MODULE) ?  (FLASH_SECTOR_SIZE*4) : (FLASH_SECTOR_SIZE))
#define MODULE_INFO_SIZE(id)					((id == DS_IR_LEARN_MODULE ) ? (FLASH_PAGE_SIZE*2) : (FLASH_PAGE_SIZE))
#else
#define NV_SECTOR_SIZE(id)						((id == DS_IR_LEARN_MODULE) ?  (FLASH_SECTOR_SIZE*4) : (FLASH_SECTOR_SIZE))
#define MODULE_INFO_SIZE(id)					((id == DS_IR_LEARN_MODULE ) ? (FLASH_PAGE_SIZE*2) : (FLASH_PAGE_SIZE))
#endif

#define MODULE_SECTOR_NUM						(2)

#define MODULE_IDX_SIZE(id)						(MODULE_INFO_SIZE(id) - sizeof(nv_sect_info_t))
#define MODULE_SECT_START(id, sectNo)   		(MODULES_START_ADDR(id) + sectNo * (NV_SECTOR_SIZE(id)))
#define MODULE_SECT_END(id, sectNo)     		(MODULES_START_ADDR(id) + (sectNo + 1) * (NV_SECTOR_SIZE(id)))
#define MODULE_IDX_START(id, sectNo)    		(MODULE_SECT_START(id, sectNo) + sizeof(nv_sect_info_t))
#define MODULE_IDX_NUM(id)    					(MODULE_IDX_SIZE(id)/(sizeof(nv_info_idx_t)))
#define MODULE_CONTEXT_START(id, sectNo, len)  	(MODULE_SECT_START(id, sectNo)+MODULE_INFO_SIZE(id))
#define ITEM_TOTAL_LEN(len)						(len + sizeof(itemHdr_t))
#define FRAMECOUNT_PAYLOAD_START(opSect)		(((MODULE_SECT_START(DS_NWK_FRAMECOUNT_MODULE, opSect)+sizeof(nv_sect_info_t)) + 0x03) & (~0x03))
#define FRAMECOUNT_NUM_PER_SECT()				((((FLASH_SECTOR_SIZE - sizeof(nv_sect_info_t)) & (~0x03)))/4)



nv_sts_t nv_init(u8 rst);
nv_sts_t nv_resetAll(void);
nv_sts_t nv_resetModule(u8 modules);
nv_sts_t nv_flashWriteNew(u8 single, u16 id, u8 itemId, u16 len, u8 *buf);
nv_sts_t nv_flashReadNew(u8 single, u8 id, u8 itemId, u16 len, u8 *buf);
nv_sts_t nv_itemDeleteByIndex(u8 id, u8 itemId, u8 opSect, u16 opIdx);
nv_sts_t nv_flashReadByIndex(u8 id, u8 itemId, u8 opSect, u16 opIdx, u16 len, u8 *buf);
nv_sts_t nv_resetToFactoryNew(void);
nv_sts_t nv_nwkFrameCountSaveToFlash(u32 frameCount);
nv_sts_t nv_nwkFrameCountFromFlash(u32 *frameCount);
nv_sts_t nv_userSaveToFlash(u8 id, u16 len, u8 *buf);
nv_sts_t nv_userLoadFromFlash(u8 id, u16 len, u8 *buf);
nv_sts_t nv_read(u8 modules, u8 id, u16 len, u8 *buf);
nv_sts_t nv_write(u8 modules, u8 id, u16 len, u8 *buf);
/*********************************************************************
 * @fn      internalFlashSizeCheck
 *
 * @brief   This function is provided to get and update to the correct flash address
 * 			where are stored the right MAC address and pre-configured parameters.
 * 			NOTE: It should be called before ZB_RADIO_INIT().
 *
 * @param   None
 *
 * @return  None
 */
void internalFlashSizeCheck(void);
