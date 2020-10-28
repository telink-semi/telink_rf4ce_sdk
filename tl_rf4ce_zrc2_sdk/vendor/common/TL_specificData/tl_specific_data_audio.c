/********************************************************************************************************
 * @file     tl_specific_data_audio.c
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
#include "../../../proj/tl_common.h"
#include "../../../net/rf4ce/profile/zrc2/zrc.h"
#include "../../../net/rf4ce/rf4ce_includes.h"
#include "tl_specific_data.h"
#include "tl_specific_data_audio.h"
#include "../app_edScan.h"
#define AudioErrTimes			0x96
audioVars_t g_uapiVar;

#if (MODULE_AUDIO_ENABLE)
#include "../../../proj/drivers/audio/drv_audio.h"
#endif

#if (__PROJECT_ZRC_2_RC__)
static u8 g_tlAppAudioErr = 0;
static tl_audioRecInfo_t *g_pAudioRecInfo = NULL;
static u8 g_audioProfileId = 0;


#define AudioReqTryTime     		5

#include "../../zrc_rc_app/zrcApp_led.h"




u8 tl_audioStartReqSend(u8 channelNum, u8 duration,u8 profile){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_START_REQ;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(tl_audioStartReq_t);

		tl_audioStartReq_t *req = (tl_audioStartReq_t *)msg->payload;
		tl_audioRecInfo_t *info = g_pAudioRecInfo;
		req->sampleRate = info->sampleRate;
		req->resoutionBits = info->resoutionBits;
		req->micChlNum = info->micChlNum;
		req->codecType = info->codecType;
		req->pktSize = info->pktSize;
		req->interval = info->interval;
		req->channelNum = channelNum;
		req->duration = duration;
		if(PROFILE_ZRC2 == profile){
		zrc2_vendorSpecficDataSend(g_uapiVar.pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(tl_audioStartReq_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}

u8 tl_audioStopReqSend(u8 profile){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_STOP_REQ;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = 0;

		if(PROFILE_ZRC2 == profile){
			zrc2_vendorSpecficDataSend(g_uapiVar.pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}

u8 tl_audioDataSend(u8 *data, u8 len){
	u8 ret = SUCCESS;

	if(g_tlAppAudioErr>=AudioErrTimes)
	{
		g_tlAppAudioErr = 0;
		audio_recTaskStop();
		if(g_uapiVar.audioCb){
			g_uapiVar.audioCb(AUDIO_IDLE,TL_AUDIO_STA_STOP);
		}
		ret = FAILURE;
	}
	else
	{
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)data;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_DATA_NOTIFY;
		pHdr->seqNo = g_tlAppDataSeqNo;
		pHdr->len = len - 4;


		ret = nwk_profileAudioDataSend(data, len, g_audioProfileId);
		if(ret == SUCCESS){
			TL_SPECIFC_SEQNO_ADD;
		}
	}
	return ret;
}

void tl_audioRecInit(u8 profileId, tl_audioRecInfo_t *info){
	g_pAudioRecInfo = info;
	g_audioProfileId = profileId;
	audio_recInit(g_pAudioRecInfo->sampleRate, tl_audioDataSend);
	audio_recTaskStop();
}



static s32 audioStartReqTimeout(void *arg){
	if(g_uapiVar.dataReqRetries-- <= 0){
		if(g_uapiVar.audioCb){
			g_uapiVar.audioCb(AUDIO_IDLE,TL_AUDIO_STA_TIMEOUT);
		}
		g_uapiVar.dataReqTimeoutCb = NULL;
		u8 value = 0;
		mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
		return -1;
	}else{
		tl_audioStartReqSend(g_uapiVar.channelNum, g_uapiVar.scanDuration,PROFILE_ZRC2);
	}
	return 0;
}



u8 app_audioStart(u8 channelNum, u8 duration, audioUserCb cb){
	if(g_uapiVar.dataReqTimeoutCb){
		return -1;
	}
	u8 value = 1;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	g_uapiVar.pairingRef = 0;
	g_uapiVar.audioCb = cb;
	g_uapiVar.channelNum = channelNum;
	g_uapiVar.scanDuration = duration;
	tl_audioStartReqSend(channelNum, duration,PROFILE_ZRC2);

	g_uapiVar.dataReqRetries = 5;//UAPI_AUDIO_RETRY_NUM;
	g_uapiVar.dataReqTimeoutCb = ev_on_timer(audioStartReqTimeout, NULL, 200 * 1000);

	return 0;
}




void tl_audio_stop(u8 profile){
	audio_recTaskStop();
	u8 value = 1;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	tl_audioStopReqSend(profile);
	if(g_uapiVar.audioCb){
		g_uapiVar.audioCb(AUDIO_IDLE,TL_AUDIO_STA_STOP);
	}
}
#endif


void tl_audioDataNotify(u8 *pld, u8 len){
#if (MODULE_AUDIO_ENABLE && MODULE_USB_ENABLE && USB_MIC_ENABLE)
		u32 mic_buf[128/4];
		memcpy(mic_buf, pld+sizeof(tl_appFrameHdr_t), len - sizeof(tl_appFrameHdr_t));
		abuf_mic_add ((u32 *)mic_buf); // 128 bytes 62 samples 1 frame
#elif(MODULE_AUDIO_ENABLE && MODULE_USB_ENABLE && USB_CDC_AUDIO)
		u8 *uartBuf = ev_buf_allocate(LARGE_BUFFER);
		if (!uartBuf) {
			while(1);
		}
		memcpy(uartBuf, pld, len);
		sendCmdToTH(uartBuf, len);
#endif
}

u8 tl_audioStartRspSend(u8 bestChannel,u8 profile){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_START_RSP;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(tl_audioStartRsp_t);

		tl_audioStartRsp_t *rsp = (tl_audioStartRsp_t *)(pHdr + 1);
		rsp->bestChannel = bestChannel;

		if(PROFILE_ZRC2 == profile){
			zrc2_vendorSpecficDataSend(g_uapiVar.pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t)+sizeof(tl_audioStartRsp_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}




u8 tl_audioStopRspSend(u8 profile){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_STOP_RSP;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(tl_audioStopRsp_t);

		tl_audioStopRsp_t *rsp = (tl_audioStopRsp_t *)(pHdr + 1);
		rsp->state = SUCCESS;
		if(PROFILE_ZRC2 == profile){
		zrc2_vendorSpecficDataSend(g_uapiVar.pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t)+sizeof(tl_audioStartRsp_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}
#if (__PROJECT_ZRC_2_RC__)
_attribute_ram_code_ u8 tl_audioDataSendCnfHandler(u8 sta){
	u8 status = sta;
	if(status==SUCCESS)
	{
		g_tlAppAudioErr = 0;
	}
	else
	{
		g_tlAppAudioErr++;
	}
	SetAudioTxState(0);
	return 0;
}
#endif


s32 app_edScanLifetimeCb(void *arg){
	g_uapiVar.scanSta = AUDIO_ED_SCAN_STA_IDLE;
	g_uapiVar.dataReqTimeoutCb = NULL;
	return -1;
}



void app_edScanCompleteCb(u8 bestChannel){
	mac_mlmeSetReq(MAC_LOGICAL_CHANNEL,  &bestChannel);
	pairTable_t *pEntry = getPairingEntryByIndex(g_uapiVar.pairingRef);
	pEntry->destChannel = bestChannel;
	g_uapiVar.scanSta = AUDIO_ED_SCAN_STA_DONE;

	g_uapiVar.dataReqTimeoutCb = ev_on_timer(app_edScanLifetimeCb, NULL, 2 * 1000 * 1000);
}

#if (__PROJECT_ZRC_2_DONGLE__)
volatile u8 audio_seq_no=0;
#endif

void tl_appAudioCmdHandler(u8 *pd, u8 len){
	tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)pd;
	u8 *pld = ((tl_appFrameFmt_t *)pd)->payload;
	if(pHdr->cmdId == TL_CMD_AUDIO_START_REQ){
#if (__PROJECT_ZRC_2_DONGLE__)
		/* first strat ED scan */
		tl_audioStartReq_t *pReq = (tl_audioStartReq_t *)pld;
		if(g_uapiVar.scanSta == AUDIO_ED_SCAN_STA_DONE){
			pairTable_t *pEntry = getPairingEntryByIndex(g_uapiVar.pairingRef);
			tl_audioStartRspSend(pEntry->destChannel,PROFILE_ZRC2);
		}else if(g_uapiVar.scanSta == AUDIO_ED_SCAN_STA_IDLE){
			app_edScanStart(pReq->channelNum, pReq->duration, app_edScanCompleteCb);
			g_uapiVar.scanSta = AUDIO_ED_SCAN_STA_DOING;
		}
		audio_seq_no = pHdr->seqNo;
#endif
	}else if(pHdr->cmdId == TL_CMD_AUDIO_STOP_REQ){
#if (__PROJECT_ZRC_2_DONGLE__)
		tl_audioStopRspSend(PROFILE_ZRC2);
#endif
	}else if(pHdr->cmdId == TL_CMD_AUDIO_DATA_NOTIFY){
#if (__PROJECT_ZRC_2_DONGLE__)
		g_uapiVar.scanSta = AUDIO_ED_SCAN_STA_IDLE;
		if(g_uapiVar.dataReqTimeoutCb){
			ev_unon_timer(&g_uapiVar.dataReqTimeoutCb);
		}
		if(audio_seq_no != pHdr->seqNo)
		{
		tl_audioDataNotify(pd, len);
		audio_seq_no = pHdr->seqNo;
		}
#endif
	}else if(pHdr->cmdId == TL_CMD_AUDIO_START_RSP){
#if (__PROJECT_ZRC_2_RC__)
//			tl_audioStartRsp_t *rsp = (tl_audioStartRsp_t *)pld;
			if(g_uapiVar.dataReqTimeoutCb){
				ev_unon_timer(&g_uapiVar.dataReqTimeoutCb);
			}
			tl_audioStartRsp_t *pRsp = (tl_audioStartRsp_t *)pld;
			mac_mlmeSetReq(MAC_LOGICAL_CHANNEL,  &pRsp->bestChannel);
			pairTable_t *pEntry = getPairingEntryByIndex(g_uapiVar.pairingRef);
			pEntry->destChannel = pRsp->bestChannel;

			audio_recTaskStart();
			if(g_uapiVar.audioCb){
				g_uapiVar.audioCb(AUDIO_OPENED,TL_AUDIO_STA_START);
			}
#endif
	}
}
