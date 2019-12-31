/********************************************************************************************************
 * @file     tl_specific_data_ota.c
 *
 * @brief	 includes command handler to process the Telink specific audio data
 *
 * @author
 * @date     Aug. 1, 2019
 *
 * @par      Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

/*
 * tl_specific_data_ota.c
 */
#include "tl_specific_data_ota.h"
/**********************************************************************
 * LOCAL VARIABLES
 */

//local
ev_time_event_t otaTimer;

//external

/**********************************************************************
 * LOCAL DEFINES
 */
#define   TELINK_MANUFACTURER_CODE                  APP_VENDOR_ID_LIST		//used in OTA

#define   STARTREQ_RETRYCNT							5
#define   DATAREQ_RETRYCNT							5
#define   STARTREQ_TIME_S							1
#define   DATAREQ_TIME_MS							20
#define   DATATRY_TIME_MS							1000
#define   STOPREQ_TIME_MS							100
#define   OTA_FRAMESIZE								0x30
#define   OTA_UPGRADE_FILE_ID						0x0BEEF11E




//flash address
#define	FLASH_OTA_NEWIMAGE_ADDR						0x40000//256K
#define	OTA_TLNK_KEYWORD_ADDROFFSET					8
#define	OTA_MAX_IMAGE_SIZE							0x30000
ota_preamble_t rc_otaInfo;

//for process
u8 	ota_RetryCnt=0;
u8 	ota_pairingRef=0;
u8 	ota_state=0;
u32 ota_headercnt=0;
u32 ota_headersize=0;
ota_UserCb 	otaCb;
u8 mcuBootAddr = 0;
u32 crcValue=0;
//for Target
u32 flashOffsetAddr=0;

u32 rcDataCount=0;
u32 targetDataCount=0;

ev_time_event_t *autoPollingCb;
ev_time_event_t *tryPollingCb;
//for Debug
//u32 	T_OTA_Debug[40]={0};
//u8 		T_OTA_Data[64]={0};
/**********************************************************************
 * @brief		get mcu reboot address
 *
 * return 		0 - reboot form address 0x0
 * 				1 - reboot from address 0x40000
 */
u8 mcuBootAddrGet(void)
{
	u8 flashInfo = 0;
	flash_read(OTA_TLNK_KEYWORD_ADDROFFSET, 1, &flashInfo);
	return ((flashInfo == 0x4b) ? 0 : 1);
}

void ota_mcuReboot(void)
{
	u32 baseAddr = 0;
	u32 newAddr = FLASH_OTA_NEWIMAGE_ADDR;
	u8 flashInfo = 0x4b;
	if(mcuBootAddr){
		baseAddr = FLASH_OTA_NEWIMAGE_ADDR;
		newAddr = 0;
	}
	flash_write((newAddr + 8),1,&flashInfo);//enable boot-up flag
	flashInfo = 0;
	flash_write((baseAddr + 8),1,&flashInfo);//disable boot-up flag
	mcu_reset();
}





void ota_initInfo(void)
{
	rc_otaInfo.fileVer = 0;
	rc_otaInfo.chipType = 0;
	rc_otaInfo.manufaurerCode = 0;
	rc_otaInfo.imageSize = 0;
	crcValue = 0xffffffff;
	flashOffsetAddr = 0;
	rcDataCount=0;
	targetDataCount=0;
	mcuBootAddr = mcuBootAddrGet();
}

void ota_startReq(u8 pairingRef,ota_UserCb Cb)
{
	ota_pairingRef = pairingRef;
	ota_initInfo();
	ota_RetryCnt = STARTREQ_RETRYCNT;
	otaCb = Cb;
	ota_headersize=0;
	ota_headercnt=0;
	if(autoPollingCb)
	{
		ev_unon_timer(&autoPollingCb);
	}
	if(tryPollingCb)
	{
		ev_unon_timer(&tryPollingCb);
	}
	autoPollingCb = ev_on_timer(ota_startReqCb, 0, STARTREQ_TIME_S*1000*1000);
}

int ota_startReqCb(void *arg)
{
	s8 ret = 0;
	if(ota_RetryCnt--)
	{
		tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
		if(msg){
			tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
			pHdr->appId = TL_SPECIFIC_ID_OTA;
			pHdr->cmdId = TL_CMD_OTA_START_REQ;
			pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
			pHdr->len = sizeof(ota_preamble_t)-4;

			ota_preamble_t *req = (ota_preamble_t *)msg->payload;
			req->fileVer = firmwareVersion;//rc_otaInfo.fileVer;
			req->chipType = CHIP_ID;//rc_otaInfo.imageType;
			req->manufaurerCode = TELINK_MANUFACTURER_CODE;//rc_otaInfo.manufaurerCode;
//			req->totalImageSize = rc_otaInfo.totalImageSize;

			zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(ota_preamble_t)-4, 1);
			ev_buf_free((u8 *)msg);
		}else{
			while(1);
		}
	}
	else
	{
//		T_OTA_Debug[2]++;
		ota_state = OTA_STA_FAILED;
		ev_on_timer(ota_callBackRestore, 0, 500*1000);
//		otaCb(ota_state);
		ret = -1;
	}
	return ret;
}

int ota_callBackRestore(void *arg)
{
//	T_OTA_Debug[0]++;
//	T_OTA_Debug[1] = ota_state;
	otaCb(ota_state);
	return -1;
}




void ota_startRspHandler(u8 *pd)
{
	ota_preamble_t *pHdr = (ota_preamble_t *)pd;

//	T_OTA_Debug[3] = pHdr->manufaurerCode;
//	T_OTA_Debug[4] = pHdr->imageType;
//	T_OTA_Debug[5] = pHdr->fileVer;
//	T_OTA_Debug[6] = pHdr;
	if((pHdr->manufaurerCode==TELINK_MANUFACTURER_CODE)&&
	   (pHdr->chipType==CHIP_ID)&&
	   (pHdr->fileVer>firmwareVersion))
	{
//		T_OTA_Debug[8] += 1;
		if(autoPollingCb)
		   ev_unon_timer(&autoPollingCb);
		rc_otaInfo.fileVer = pHdr->fileVer;
		rc_otaInfo.manufaurerCode = pHdr->manufaurerCode;
		rc_otaInfo.chipType = pHdr->chipType;
		rc_otaInfo.imageSize = pHdr->imageSize;

		if(pHdr->imageSize<OTA_MAX_IMAGE_SIZE)
		{
			u32 baseAddr = (mcuBootAddr) ? 0 : FLASH_OTA_NEWIMAGE_ADDR;
			u16 sectorNumUsed = pHdr->imageSize / FLASH_SECTOR_SIZE + 1;
			for(u16 i = 0; i < sectorNumUsed; i++){
				flash_erase(baseAddr + i * FLASH_SECTOR_SIZE);
			}
			ota_state = OTA_STA_STARTING;
			ota_dataReqCb();
		}
		else
		{
			ota_state = OTA_STA_FAILED;
			ota_RetryCnt = 0;
			if(autoPollingCb)
				ev_unon_timer(&autoPollingCb);
			ev_on_timer(ota_callBackRestore, 0, 500*1000);
		}
//		ota_RetryCnt = DATAREQ_RETRYCNT;
//		autoPollingCb = ev_on_timer(ota_dataReqCb, 0, DATAREQ_TIME_MS*1000);
	}
	else
	{
//		T_OTA_Debug[7] += 1;
		ota_state = OTA_STA_FAILED;
		ota_RetryCnt = 0;
		if(autoPollingCb)
			ev_unon_timer(&autoPollingCb);
		ev_on_timer(ota_callBackRestore, 0, 500*1000);
//		otaCb(ota_state);
	}
}



s8 ota_dataReqCb(void)
{
	if(tryPollingCb)  ev_unon_timer(&tryPollingCb);
//	T_OTA_Debug[9] += 1;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_OTA;
		pHdr->cmdId = TL_CMD_OTA_DATA_REQ;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(ota_preamble_t);

		ota_preamble_t *req = (ota_preamble_t *)msg->payload;
		req->fileVer = rc_otaInfo.fileVer;//rc_otaInfo.fileVer;
		req->chipType = rc_otaInfo.chipType;//rc_otaInfo.imageType;
		req->manufaurerCode = rc_otaInfo.manufaurerCode;//rc_otaInfo.manufaurerCode;
		req->imageSize = rcDataCount;//max data block

		zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(ota_preamble_t), 1);

		ota_RetryCnt = DATAREQ_RETRYCNT;
		tryPollingCb = ev_on_timer(ota_dataTryReqCb, 0, DATATRY_TIME_MS*1000);

		ev_buf_free((u8 *)msg);
	}else{
		while(1);
	}
	return -1;
}


int ota_dataTryReqCb(void *arg)
{
	s8 ret=0;
	if(ota_RetryCnt--)
	{
		tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
		if(msg){
			tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
			pHdr->appId = TL_SPECIFIC_ID_OTA;
			pHdr->cmdId = TL_CMD_OTA_DATA_REQ;
			pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
			pHdr->len = sizeof(ota_preamble_t);

			ota_preamble_t *req = (ota_preamble_t *)msg->payload;
			req->fileVer = rc_otaInfo.fileVer;//rc_otaInfo.fileVer;
			req->chipType = rc_otaInfo.chipType;//rc_otaInfo.imageType;
			req->manufaurerCode = rc_otaInfo.manufaurerCode;//rc_otaInfo.manufaurerCode;
			req->imageSize = rcDataCount;//max data block

			zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(ota_preamble_t), 1);
			ev_buf_free((u8 *)msg);
		}else{
			while(1);
		}
	}
	else
	{
		ota_state = OTA_STA_FAILED;
		ota_RetryCnt = 0;
		ev_on_timer(ota_callBackRestore, 0, 500*1000);
//		otaCb(ota_state);
		ret = -1;
	}
	return ret;
}








void ota_DataProcess(u8 len, u8 *pData)
{
	u8 i=0;
	if(ota_state == OTA_STA_STARTING)
	{
		for(i = 0; i < len; i++)
		{
			if(ota_headercnt==OTA_HEAD_LENGTH)//head size
			{
				ota_headersize = (*(u8 *)(pData+i))|((*(u8 *)(pData+i+1))<<8);
			}
			else if(ota_headercnt==OTA_HEAD_MAUNCODE)
			{
				rc_otaInfo.manufaurerCode = (*(u8 *)(pData+i))|(*(u8 *)(pData+i+1)<<8);
			}
			else if(ota_headercnt==OTA_HEAD_IMAGETYPE)
			{
				rc_otaInfo.chipType = (*(u8 *)(pData+i))|(*(u8 *)(pData+i+1)<<8);
			}
			else if(ota_headercnt==OTA_HEAD_FILEVER)
			{
				rc_otaInfo.fileVer = (*(u8 *)(pData+i))|(*(u8 *)(pData+i+1)<<8)|(*(u8 *)(pData+i+2)<<16)|(*(u8 *)(pData+i+3)<<24);
			}
			else if((ota_headercnt==ota_headersize)&&(ota_headersize!=0))
			{
				ota_state = OTA_STA_DATAING;
				ota_headercnt += 6;
				i+=6;
				break;
			}
			ota_headercnt++;
		}
	}
	if(ota_state == OTA_STA_DATAING)
	{
		u8 datalen=0;

		if(len>0)
		{
			datalen = len - i;
			u32 ota_headerbuf = ota_headercnt;
			ota_headercnt += datalen;
			u32 baseAddr = (mcuBootAddr) ? 0 : FLASH_OTA_NEWIMAGE_ADDR;
			u8 flag=0;
			if((flashOffsetAddr<=8)&&((datalen+flashOffsetAddr)>=8)&&(datalen>0))
			{
					flag = 1;
					pData[i+8-flashOffsetAddr] = 0xff;
			}
			flash_write(baseAddr + flashOffsetAddr, datalen, &pData[i]);
			if(flag)  pData[i+8-flashOffsetAddr] = 0x4b;
			flashOffsetAddr += datalen;


			if(ota_headercnt>(rc_otaInfo.imageSize-4))
			{
				if(ota_headerbuf>(rc_otaInfo.imageSize-4))
				{
					datalen=0;
				}
				else
				{
					u16 offset = ota_headercnt-(rc_otaInfo.imageSize-4);
					if(datalen>offset)
						datalen -= offset;
					else
						datalen=0;
				}
			}
			if(datalen)
			{
				crcValue = xcrc32(&pData[i], datalen, crcValue);
			}

		}
	}
}



void ota_dataRspHandler(u8 *pd)
{
	ota_dataFrame_t *pHdr = (ota_dataFrame_t *)pd;


	if((pHdr->manufaurerCode==rc_otaInfo.manufaurerCode)&&
	   (pHdr->chipType==rc_otaInfo.chipType)&&
	   (pHdr->fileVer==rc_otaInfo.fileVer)&&
	   (pHdr->imageSize==rcDataCount))
	{
		if(autoPollingCb)	ev_unon_timer(&autoPollingCb);
		if(tryPollingCb)	ev_unon_timer(&tryPollingCb);
		if(pHdr->payloadSize==0)
		{
			ota_state = OTA_STA_FAILED;
			ev_on_timer(ota_callBackRestore, 0, 500*1000);
			return;
		}
		ota_DataProcess(pHdr->payloadSize,pHdr->payload);
		rcDataCount++;
		if(ota_headercnt>=rc_otaInfo.imageSize)
		{
			u32 baseAddr = (mcuBootAddr) ? 0 : FLASH_OTA_NEWIMAGE_ADDR;
			u32 crcBin=0;
			flash_read(baseAddr + rc_otaInfo.imageSize-ota_headersize-4-6, 4, (u8 *)&crcBin);
//			T_OTA_Debug[4] = crcBin;
//			T_OTA_Debug[5] = crcValue;
			if(crcBin==crcValue)
			{
				ota_state = OTA_STA_SUCCESS;
				ota_RetryCnt = DATAREQ_RETRYCNT;
				autoPollingCb = ev_on_timer(ota_stopRspCb, 0, STOPREQ_TIME_MS*1000);
			}
			else
			{
				ota_state = OTA_STA_FAILED;
				ev_on_timer(ota_callBackRestore, 0, 500*1000);
			}
		}
		else
		{
			ota_dataReqCb();
//			ota_RetryCnt = DATAREQ_RETRYCNT;
//			autoPollingCb = ev_on_timer(ota_dataReqCb, 0, DATAREQ_TIME_MS*1000);
		}
	}
}

int ota_stopRspCb(void *pd)
{
	s8 ret=0;
	if(ota_RetryCnt--)
	{
		tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
		if(msg){
			tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
			pHdr->appId = TL_SPECIFIC_ID_OTA;
			pHdr->cmdId = TL_CMD_OTA_STOP_REQ;
			pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
			pHdr->len = sizeof(ota_preamble_t)-4;

			ota_preamble_t *req = (ota_preamble_t *)msg->payload;
			req->fileVer = rc_otaInfo.fileVer;//rc_otaInfo.fileVer;
			req->chipType = rc_otaInfo.chipType;//rc_otaInfo.imageType;
			req->manufaurerCode = rc_otaInfo.manufaurerCode;//rc_otaInfo.manufaurerCode;
//			req->totalImageSize = OTA_FRAMESIZE;//max data block

			zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(ota_preamble_t)-4, 1);
			ev_buf_free((u8 *)msg);
		}else{
			while(1);
		}
	}
	else
	{
		ota_state = OTA_STA_FAILED;
//		ev_unon_timer(&autoPollingCb);
		otaCb(ota_state);
		ev_on_timer(ota_callBackRestore, 0, 500*1000);
		ret = -1;
	}
	return ret;
}


void ota_stopRspHandler(u8 *pd)
{
	ota_preamble_t *pHdr = (ota_preamble_t *)pd;
	if((pHdr->manufaurerCode==rc_otaInfo.manufaurerCode)&&
	   (pHdr->chipType==rc_otaInfo.chipType)&&
	   (pHdr->fileVer==rc_otaInfo.fileVer))
	{
//		ota_state = OTA_STA_SUCCESS;
		ev_unon_timer(&autoPollingCb);
		ev_on_timer(ota_callBackRestore, 0, 1*500*1000);
	}
}






void ota_startReqHandler(u8 *pd)
{
	ota_preamble_t *pHdr = (ota_preamble_t *)pd;
//	u16 headsize=0;
	ota_hdrFields_t headinfo;
	ota_signInfo_t imageinfo;
	u32 file_id=0;
	ota_initInfo();
	u32 flashAddr = (mcuBootAddr) ? 0 : FLASH_OTA_NEWIMAGE_ADDR;
	flash_read(flashAddr, 4, (u8 *)&file_id);

	if(file_id==OTA_UPGRADE_FILE_ID)
	{
		flash_read(flashAddr,sizeof(ota_hdrFields_t), (u8 *)&headinfo);
		rc_otaInfo.fileVer = headinfo.fileVer;
		rc_otaInfo.chipType = headinfo.chipType;
		rc_otaInfo.manufaurerCode = headinfo.manufaurerCode;
		rc_otaInfo.imageSize = headinfo.totalImageSize;


		ota_headersize = rc_otaInfo.imageSize;
		ota_headercnt = 0;

		if((pHdr->fileVer<rc_otaInfo.fileVer)&&
		   (pHdr->chipType==rc_otaInfo.chipType)&&
		   (pHdr->manufaurerCode==rc_otaInfo.manufaurerCode))
			ota_state = OTA_BIN_VALID;
		else
			ota_state = OTA_BIN_INVALID;

		if(ota_state == OTA_BIN_VALID)
		{
			if(headinfo.otaHdrLen<headinfo.totalImageSize)
				flash_read(flashAddr+headinfo.otaHdrLen,sizeof(ota_signInfo_t), (u8 *)&imageinfo);
			else
				ota_state = OTA_BIN_INVALID;

			if((imageinfo.BinType!=OTA_TYPE_BIN)||
			   (headinfo.totalImageSize!=(headinfo.otaHdrLen+imageinfo.BinSize+sizeof(ota_signInfo_t))))
				ota_state = OTA_BIN_INVALID;
		}

		if(headinfo.totalImageSize>=OTA_MAX_IMAGE_SIZE)
			ota_state = OTA_BIN_INVALID;
	}
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_OTA;
		pHdr->cmdId = TL_CMD_OTA_START_RSP;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(ota_preamble_t);

		ota_preamble_t *req = (ota_preamble_t *)msg->payload;
		req->fileVer = rc_otaInfo.fileVer;//rc_otaInfo.fileVer;
		req->chipType = rc_otaInfo.chipType;//rc_otaInfo.imageType;
		req->manufaurerCode = rc_otaInfo.manufaurerCode;//rc_otaInfo.manufaurerCode;
		req->imageSize = rc_otaInfo.imageSize;//max data block

		zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(ota_preamble_t), 1);
		ev_buf_free((u8 *)msg);
	}else{
		while(1);
	}
}



void ota_dataReqHandler(u8 *pd)
{
	ota_preamble_t *pdHdr = (ota_preamble_t *)pd;

	if((pdHdr->fileVer==rc_otaInfo.fileVer)&&
	   (pdHdr->chipType==rc_otaInfo.chipType)&&
	   (pdHdr->manufaurerCode==rc_otaInfo.manufaurerCode)&&
	   (ota_state == OTA_BIN_VALID))
	{
		tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
		if(msg){
			tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
			pHdr->appId = TL_SPECIFIC_ID_OTA;
			pHdr->cmdId = TL_CMD_OTA_DATA_RSP;
			pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
//			pHdr->len = sizeof(ota_preamble_t);

			u8 read_len = 0;
			u8  ota_data[OTA_FRAMESIZE] = {0};

			u32 flashAddr = (mcuBootAddr) ? 0 : FLASH_OTA_NEWIMAGE_ADDR;

			ota_headercnt = pdHdr->imageSize*OTA_FRAMESIZE;
			if((ota_headercnt+OTA_FRAMESIZE)<ota_headersize)
				read_len = OTA_FRAMESIZE;
			else
				read_len = ota_headersize%OTA_FRAMESIZE;
			flash_read(flashAddr+ota_headercnt, read_len, (u8 *)ota_data);

			ota_dataFrame_t *req = (ota_dataFrame_t *)msg->payload;
			req->fileVer = rc_otaInfo.fileVer;//rc_otaInfo.fileVer;
			req->chipType = rc_otaInfo.chipType;//rc_otaInfo.imageType;
			req->manufaurerCode = rc_otaInfo.manufaurerCode;//rc_otaInfo.manufaurerCode;
			req->imageSize = pdHdr->imageSize;
			req->payloadSize = read_len;//max data block
			memcpy((u8 *)req->payload,(u8 *)ota_data,read_len);
			pHdr->len = sizeof(tl_appFrameHdr_t) + sizeof(ota_dataFrame_t)+read_len;
			zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(ota_dataFrame_t)+read_len, 1);
			ev_buf_free((u8 *)msg);
		}else{
			while(1);
		}
	}
	else
	{
		tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
		if(msg){
			tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
			pHdr->appId = TL_SPECIFIC_ID_OTA;
			pHdr->cmdId = TL_CMD_OTA_DATA_RSP;
			pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
			pHdr->len = 0;
			zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t), 1);
			ev_buf_free((u8 *)msg);
		}else{
			while(1);
		}

	}



}

void ota_stopReqHandler(u8 *pd)
{
	ota_preamble_t *pHdr = (ota_preamble_t *)pd;

	if((pHdr->fileVer==rc_otaInfo.fileVer)&&
	   (pHdr->chipType==rc_otaInfo.chipType)&&
	   (pHdr->manufaurerCode==rc_otaInfo.manufaurerCode))
	{
		tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
		if(msg){
			tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
			pHdr->appId = TL_SPECIFIC_ID_OTA;
			pHdr->cmdId = TL_CMD_OTA_STOP_RSP;
			pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
			pHdr->len = sizeof(ota_preamble_t)-4;

			ota_preamble_t *req = (ota_preamble_t *)msg->payload;
			req->fileVer = rc_otaInfo.fileVer;//rc_otaInfo.fileVer;
			req->chipType = rc_otaInfo.chipType;//rc_otaInfo.imageType;
			req->manufaurerCode = rc_otaInfo.manufaurerCode;//rc_otaInfo.manufaurerCode;
//			req->imageSize = rc_otaInfo.imageSize;//max data block

			zrc2_vendorSpecficDataSend(ota_pairingRef, (u8 *)msg, sizeof(ota_preamble_t)+sizeof(tl_appFrameHdr_t)-4, 1);
			ev_buf_free((u8 *)msg);
		}else{
			while(1);
		}
	}

}





void tl_appOtaCmdHandler(u8 *pd)
{
	tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)pd;
	u8 *pld = ((tl_appFrameFmt_t *)pd)->payload;

	if(pHdr->cmdId == TL_CMD_OTA_START_REQ){//from RC
		ota_startReqHandler(pld);
	}else if(pHdr->cmdId == TL_CMD_OTA_STOP_REQ){
		ota_stopReqHandler(pld);
	}else if(pHdr->cmdId == TL_CMD_OTA_DATA_REQ){
		ota_dataReqHandler(pld);
	}

	else if(pHdr->cmdId == TL_CMD_OTA_START_RSP){//from target
//		T_OTA_Debug[10]++;
		ota_startRspHandler(pld);
	}else if(pHdr->cmdId == TL_CMD_OTA_STOP_RSP){
//		T_OTA_Debug[11]++;
		ota_stopRspHandler(pld);
	}else if(pHdr->cmdId == TL_CMD_OTA_DATA_RSP){
//		T_OTA_Debug[12]++;
		ota_dataRspHandler(pld);
	}
}

