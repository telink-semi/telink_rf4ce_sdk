/********************************************************************************************************
 * @file    tl_specific_data_ota.h
 *
 * @brief   This is the header file for tl_specific_data_ota.h
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

#ifndef TL_SPECIFIC_DATA_OTA_H_
#define TL_SPECIFIC_DATA_OTA_H_
#include "tl_specific_data.h"
#if RF4CE_ZRC2_ENABLE
#include "../../../net/rf4ce/profile/zrc2/zrc.h"
#elif  RF4CE_MSO_ENABLE
#include "../../../net/rf4ce/profile/mso/mso.h"
#endif
#include "../../../net/rf4ce/rf4ce_includes.h"
#include "../../../proj/tl_common.h"
#include "../../../proj/drivers/drv_flash.h"
#include "../../../proj/drivers/drv_pm.h"
#if (RF4CE_CONTROLLER)
	#include "../../rc_app/rc_info.h"
#elif (RF4CE_TARGET)
	#include "../../dongle_app/dongle_info.h"
#endif


/*
 * ota command
 * */
typedef enum{
	TL_CMD_OTA_START_REQ 		= 0x01,
	TL_CMD_OTA_START_RSP 		= 0x02,
	TL_CMD_OTA_STOP_REQ 		= 0x03,
	TL_CMD_OTA_STOP_RSP 		= 0x04,
	TL_CMD_OTA_DATA_REQ 		= 0x05,
	TL_CMD_OTA_DATA_RSP 		= 0x06,
}tl_otaCmdId_e;


enum{
	OTA_STA_SUCCESS   = 0x00,		//!<  No Error Occurred
	OTA_STA_FAILED    = 0x01,		//!<  Operation not permitted
	OTA_STA_STARTING  = 0x02,		//!<  Operation not permitted
	OTA_STA_DATAING   = 0x03,		//!<  Operation not permitted
	OTA_STA_END  	  = 0x04,		//!<  Operation not permitted
}tl_otaState_e;


enum{
	OTA_BIN_VALID      = 0x00,		//!<  No Error Occurred
	OTA_BIN_INVALID    = 0x01,		//!<  Operation not permitted
}tl_otabin_e;


enum{
	OTA_TYPE_BIN       = 0x00,		//image
}image_type_e;


enum{
	OTA_HEAD_LENGTH   = 0x06,		//!<  No Error Occurred
	OTA_HEAD_MAUNCODE    = 0x0a,		//!<  Operation not permitted
	OTA_HEAD_IMAGETYPE  = 0x0c,		//!<  Operation not permitted
	OTA_HEAD_FILEVER   = 0x0e,		//!<  Operation not permitted
	OTA_HEAD_IMAGESIZE   = 0x3a,		//!<  Operation not permitted
}tl_otaHeaderLocation_e;


typedef struct{
	u32			fileVer;
	u16			chipType;
	u16			manufaurerCode;
	u32			imageSize;	//The value represents the total image size in bytes.
}ota_preamble_t;

typedef struct{
	u32			fileVer;
	u16			chipType;
	u16			manufaurerCode;
	u32			imageSize;	//The value represents the total image size in bytes.
	u8			payloadSize;
	u8			payload[0];
}ota_dataFrame_t;

typedef struct{
	/*The value is a unique 4-byte value that is included at the beginning of all ZigBee OTA upgrade image
	files in order to quickly identify and distinguish the file as being a ZigBee OTA cluster upgrade file,
	without having to examine the whole file content. This helps distinguishing the file from other file
	types on disk. The value is defined to be ¡°0x0BEEF11E¡±.*/
	u32			otaUpgradeFileID;

	/*The current OTA header version shall be 0x0100 with major version of ¡°01¡± and minor version of ¡°00¡±.*/
	u16			otaHdrVer;

	/*This value indicates full length of the OTA header in bytes, including the OTA upgrade file identifier,
	OTA header length itself to any optional fields.*/
	u16			otaHdrLen;
//	The bit mask indicates whether additional information such as Image Signature or Signing Certificate
//	are included as part of the OTA Upgrade Image.
	u16			otaHdrFC;

	u16			manufaurerCode;

	/*The manufacturer should assign an appropriate and unique image type value to each of its devices in
	651 order to distinguish the products.*/
	u16			chipType;

	//
	u32			fileVer;
	u16			zbStackVer;
	//TLNK
	u8			otaHdrString[32];
	//The value represents the total image size in bytes.
	u32			totalImageSize;

//	u8			securityCredVer;//option 
//	addrExt_t	fileDest;
//	u16			minHdrVer;
//	u16			maxHdrVer;
}ota_hdrFields_t;



typedef struct{
	u16			BinType;
	u32			BinSize;
}ota_signInfo_t;



//unsigned int xcrc32(const unsigned char *buf, int len, unsigned int init);
typedef void (*ota_UserCb)(u8 status);
void tl_appOtaCmdHandler(u8 *pd);
void ota_initInfo(void);
int ota_startReqCb(void *arg);
void ota_startReq(u8 pairingRef,ota_UserCb  Cb);
void ota_headDataProcess(u8 len, u8 *pData);

int ota_stopRspCb(void *pd);
int ota_callBackRestore(void *arg);
s8 ota_dataReqCb(void);
int ota_dataTryReqCb(void *arg);


extern int app_saveInfoPm(void);
#endif /* TL_AUDIO_H_ */
