/********************************************************************************************************
 * @file    drv_nv.h
 *
 * @brief   This is the header file for drv_nv
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
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
 *
 *******************************************************************************************************/

#pragma once


/* Flash Base Address define */
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)|| defined(MCU_CORE_B92)
	#define FLASH_TLNK_FLAG_OFFSET		8
#endif



/************************************************************************
 * Flash address of factory setting parameters,
 * where are stored the MAC ADDRESS and CALIBRATION information.
 */
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

/************************************************************************/
extern volatile u32 g_u32MacFlashAddr;
extern volatile u32 g_u32CfgFlashAddr;

#define MAC_BASE_ADD					(g_u32MacFlashAddr)
#define FACTORY_CFG_BASE_ADD			(g_u32CfgFlashAddr)
/************************************************************************/

/**************************************************************************************
 * The following is the detailed factory configure information (F_CFG_Info).
 */
/* 8 bytes for MAC address. */
#define CFG_MAC_ADDRESS              	(MAC_BASE_ADD)

/* 2 bytes for USB ID. */
#define CFG_TELINK_USB_ID				(MAC_BASE_ADD + 0x40)

/* 1 byte for frequency offset calibration. */
#define CFG_FREQUENCY_OFFSET			(FACTORY_CFG_BASE_ADD)

/* 2 bytes for TP GAIN.
 * 0x77040(or 0xFE040) for BLE TP_GAIN_0, 0x77041(or 0xFE041) for BLE TP_GAIN_1,
 * 0x77042(or 0xFE042) for zigbee TP_GAIN_0, 0x77043(or 0xFE043) for zigbee TP_GAIN_1. */
#define CFG_TP_GAIN                  	(FACTORY_CFG_BASE_ADD + 0x42)

/* Not supported for current SDK. */
#define CFG_32K_COUNTER_CALIBRATION	 	(FACTORY_CFG_BASE_ADD + 0x80)

/* Not supported for current SDK. */
#define CFG_ADC_CALIBRATION           	(FACTORY_CFG_BASE_ADD + 0xC0)

/* Not supported for current SDK. */
#define CFG_24M_CRYSTAL_CALIBRATION     (FACTORY_CFG_BASE_ADD + 0x100)

/* Not supported for current SDK. */
#define CFG_T_SENSOR_CALIBRATION     	(FACTORY_CFG_BASE_ADD + 0x140)

/* UID-based Firmware Encryption data(16 bytes), 0x77180(or 0xFE180) ~ 0x7718F(or 0xFE18F). */
#define CFG_FIRMWARE_ENCRYPTION			(FACTORY_CFG_BASE_ADD + 0x180)

/* 2 bytes for VDD_F calibration.
 * 0x771C0(or 0xFF1C0) for DCDC trim,
 * 0x771C1(or 0xFF1C1) for LDO trim.
 */
#define CFG_VDD_F_CALIBRATION			(FACTORY_CFG_BASE_ADD + 0x1C0)


/**************************************************************************************
 * The following is the detailed user configure information (U_CFG_Info).
 */
/* 16 bytes for pre-install code. */
#if(FLASH_CAP_SIZE_1M || FLASH_CAP_SIZE_2M || FLASH_CAP_SIZE_4M)
#define CFG_PRE_INSTALL_CODE			(0xFD000)
#else
#define CFG_PRE_INSTALL_CODE			(0x78000)
#endif

/* 1 byte for factory reset.
 * If not 0xFF, means the device is doing factory reset(erase NV).
 * The device will check this byte when powered on, if it is not 0xFF,
 * it will erase NV first.
 */
#if(FLASH_CAP_SIZE_1M || FLASH_CAP_SIZE_2M || FLASH_CAP_SIZE_4M)
#define CFG_FACTORY_RST_CNT			  	(0xFC000)
#else
#define CFG_FACTORY_RST_CNT			  	(0x79000)
#endif

/**************************************************************************************
 * Flash address of NV module.
 */
/* NV modules start address */
#if FLASH_CAP_SIZE_1M
#define NV_BASE_ADDRESS					0x80000

#else
#define	NV_BASE_ADDRESS					0x34000//start from 208K address
#define	NV_BASE_ADDRESS2				0x78000//start from 488K address
#endif

/************************************************************************
 * Flash address of APP firmware.
 */
#define FLASH_ADDR_OF_APP_FW			APP_IMAGE_ADDR


/************************************************************************
 * Flash address of OTA image.
 */
#if !defined(BOOT_LOADER_MODE) || (BOOT_LOADER_MODE == 0)
#if(FLASH_CAP_SIZE_1M || FLASH_CAP_SIZE_2M || FLASH_CAP_SIZE_4M)
	//max size = (0x80000 - 0) / 2 = 256k
	#define FLASH_OTA_IMAGE_MAX_SIZE	((NV_BASE_ADDRESS - FLASH_ADDR_OF_APP_FW) / 2)
#else
	//max size = (0x34000 - 0) = 208k
	#define FLASH_OTA_IMAGE_MAX_SIZE	(NV_BASE_ADDRESS - FLASH_ADDR_OF_APP_FW)
#endif
	//unchangeable address
	#define FLASH_ADDR_OF_OTA_IMAGE		(0x40000)
#else
	//1M   Flash: max size = (0xE6000 - 0x8000) / 2 = 444k
	//512k Flash: max size = (0x6A000 - 0x8000) / 2 = 196k
	#define FLASH_OTA_IMAGE_MAX_SIZE	((NV_BASE_ADDRESS - FLASH_ADDR_OF_APP_FW) / 2)
	//1M   Flash: ota addr = 0x8000 + 444k = 0x77000
	//512k Flash: ota addr = 0x8000 + 196k = 0x39000
	#define FLASH_ADDR_OF_OTA_IMAGE		(FLASH_ADDR_OF_APP_FW + FLASH_OTA_IMAGE_MAX_SIZE)
#endif

/******************************************* DUAL_MODE ********************************************************/
/* Don't care, will be delete. */
#if DUAL_MODE
//NOTE: firmware must less then 192K if UDAL_MODE used
typedef enum{
	TYPE_TLK_MESH  						= 0x000000A3,// don't change, must same with telink mesh SDK
	TYPE_SIG_MESH  						= 0x0000003A,// don't change, must same with telink mesh SDK
	TYPE_TLK_BLE_SDK 					= 0x000000C3,// don't change, must same with telink mesh SDK
	TYPE_TLK_ZIGBEE  					= 0x0000003C,// don't change, must same with telink mesh SDK
	TYPE_DUAL_MODE_STANDBY				= 0x00000065,// dual mode state was standby to be selected
	TYPE_DUAL_MODE_RECOVER 				= 0x00000056,// don't change, must same with telink mesh SDK, recover for mesh
}telink_sdk_type_t;

#define CFG_TELINK_SDK_TYPE				(0x73000)
#define CFG_TELINK_SIG_MESH_CRC			(0x73040)
#define CFG_TELINK_SIG_MESH_CODE_4K		(0x75000)
#define CFG_TELINK_DUAL_MODE_ENABLE		(0x76080)
#endif	/* DUAL_MODE */
/******************************************** END ***********************************************************/




#define NV_SECT_INFO_CHECK_BITS			6
#define NV_SECT_INFO_CHECK_BITMASK		0x3f
#define NV_SECT_INFO_SECTNO_BITS		(8-NV_SECT_INFO_CHECK_BITS)
#define NV_SECT_INFO_SECTNO_BITMASK		0x3

/* sector info(4Bytes) + index info(8Bytes) + index info(8Bytes) + ... */
typedef struct{
	u16 usedFlag;
	u8  idName;
	u8  opSect;    //crcCheckBit(6bits) + opSect(2bits)
}nv_sect_info_t;

typedef struct{
	u32 offset;
	u16 size;
	u8  itemId;
	u8  usedState;
#if defined(MCU_CORE_B92)
	u8  resv[8];   //PUYA flash only supports re-write 64 times
#endif
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
typedef enum{
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
}nv_module_t;

typedef enum{
	NV_ITEM_ID_INVALID				= 0,/* Item id 0 should not be used. */
	NV_ITEM_ID_MAX					= 0xFF,/* Item id 0xFF should not be used. */
}nv_item_t;

typedef enum{
	NV_SUCC,
    NV_INVALID_MODULS 				= 1,
    NV_INVALID_ID ,
    NV_ITEM_NOT_FOUND,
    NV_NOT_ENOUGH_SAPCE,
    NV_ITEM_LEN_NOT_MATCH,
    NV_CHECK_SUM_ERROR,
    NV_ENABLE_PROTECT_ERROR,
    NV_NO_MEDIA,
    NV_DATA_CHECK_ERROR,
	NV_ITEM_CHECK_ERROR,
	NV_MODULE_NOT_FOUND,
	NV_MODULE_ERASE_NEED
} nv_sts_t;


#define FLASH_PAGE_SIZE							256
#define	FLASH_SECTOR_SIZE						4096//4K

#define NV_SECTOR_VALID							0x5A5A
#define NV_SECTOR_VALID_CHECKCRC				0x7A7A
#define NV_SECTOR_VALID_READY_CHECKCRC			0xFAFA
#define NV_SECTOR_INVALID						0x5050
#define NV_SECTOR_IDLE							0xFFFF

#define ITEM_FIELD_VALID						0x5A
#define ITEM_FIELD_VALID_SINGLE					0x7A
#define ITEM_FIELD_INVALID						0x50
#define ITEM_FIELD_OPERATION					0xFA
#define ITEM_FIELD_IDLE							0xFF

#define ITEM_HDR_FIELD_VALID_CHECKSUM			0x5A
#define ITEM_HDR_FIELD_VALID_CHECKCRC			0x7A



#if FLASH_CAP_SIZE_1M
#define MODULES_START_ADDR(id)					(NV_BASE_ADDRESS + FLASH_SECTOR_SIZE * (2 * id))
#define NV_SECTOR_SIZE(id)						((id == DS_IR_LEARN_MODULE) ?  (FLASH_SECTOR_SIZE*4) : (FLASH_SECTOR_SIZE))
#define MODULE_INFO_SIZE(id)					((id == DS_IR_LEARN_MODULE ) ? (FLASH_PAGE_SIZE*2) : (FLASH_PAGE_SIZE))
#else
#define MODULES_START_ADDR(id)					((id<6) ? (NV_BASE_ADDRESS + FLASH_SECTOR_SIZE * (2 * id)) : (NV_BASE_ADDRESS2 + FLASH_SECTOR_SIZE * (2 * (id-6))))
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



nv_sts_t nv_resetAll(void);
nv_sts_t nv_resetModule(u8 modules);
nv_sts_t nv_flashWriteNew(u8 single, u16 id, u8 itemId, u16 len, u8 *buf);
nv_sts_t nv_flashReadNew(u8 single, u8 id, u8 itemId, u16 len, u8 *buf);
nv_sts_t nv_itemDeleteByIndex(u8 id, u8 itemId, u8 opSect, u16 opIdx);
nv_sts_t nv_flashSingleItemRemove(u8 id, u8 itemId, u16 len);
nv_sts_t nv_flashReadByIndex(u8 id, u8 itemId, u8 opSect, u16 opIdx, u16 len, u8 *buf);
void nv_itemLengthCheckAdd(u8 itemId, u16 len);
nv_sts_t nv_resetToFactoryNew(void);
bool nv_facrotyNewRstFlagCheck(void);
void nv_facrotyNewRstFlagSet(void);
void nv_facrotyNewRstFlagClear(void);
nv_sts_t nv_nwkFrameCountSaveToFlash(u32 frameCount);
nv_sts_t nv_nwkFrameCountFromFlash(u32 *frameCount);
nv_sts_t nv_flashSingleItemSizeGet(u8 id, u8 itemId, u16 *len);
///*********************************************************************
// * @fn      internalFlashSizeCheck
// *
// * @brief   This function is provided to get and update to the correct flash address
// * 			where are stored the right MAC address and pre-configured parameters.
// * 			NOTE: It should be called before ZB_RADIO_INIT().
// *
// * @param   None
// *
// * @return  None
// */
//void internalFlashSizeCheck(void);
