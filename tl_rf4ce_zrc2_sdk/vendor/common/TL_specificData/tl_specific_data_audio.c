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

#define AudioErrTimes			0x10

static u8 g_tlAppAudioBusy = 0;

#if (__PROJECT_ZRC_2_RC__)
static u8 g_tlAppAudioErr = 0;
static tl_audioRecInfo_t *g_pAudioRecInfo = NULL;
static tl_audioUserCb_t gAudioUserCb = NULL;
static u8 g_audioProfileId = 0;


#include "../../zrc_rc_app/zrcApp_led.h"
#if (MODULE_AUDIO_ENABLE)
#include "../../../proj/drivers/audio/drv_audio.h"
#endif

u8 tl_audioStartReqSend(u8 profile, u8 pairingRef){
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
		req->channelNum = info->channelNum;
		req->codecType = info->codecType;
		req->pktSize = info->pktSize;
		req->interval = info->interval;

		if(PROFILE_ZRC2 == profile){
			zrc2_vendorSpecficDataSend(pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t) + sizeof(tl_audioStartReq_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}

u8 tl_audioStopReqSend(u8 profile, u8 pairingRef){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_STOP_REQ;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = 0;

		if(PROFILE_ZRC2 == profile){
			zrc2_vendorSpecficDataSend(pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}

u8 tl_audioDataSend(u8 *data, u8 len){
	u8 ret = SUCCESS;
	tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)data;
	pHdr->appId = TL_SPECIFIC_ID_AUDIO;
	pHdr->cmdId = TL_CMD_AUDIO_DATA_NOTIFY;
	pHdr->seqNo = g_tlAppDataSeqNo;
	pHdr->len = len - 4;

	if(g_tlAppAudioBusy){
		return FAILURE;
	}
	ret = nwk_profileAudioDataSend(data, len, g_audioProfileId);
	if(ret == SUCCESS){
		g_tlAppAudioBusy = 1;
		TL_SPECIFC_SEQNO_ADD;
	}
	return ret;
}

void tl_audioRecInit(u8 profileId, tl_audioRecInfo_t *info, tl_audioUserCb_t userCb){
	g_pAudioRecInfo = info;
	gAudioUserCb = userCb;
	g_audioProfileId = profileId;
	audio_recInit(g_pAudioRecInfo->sampleRate, tl_audioDataSend);
}


ev_time_event_t *audioStartFailedCb;
void tl_audio_start(u8 profile, u8 pairingRef){
	tl_audioStartReqSend(profile,  pairingRef);
	if(audioStartFailedCb)
		ev_unon_timer(&audioStartFailedCb);
	audioStartFailedCb = ev_on_timer(audio_startfailed, 0, 300*1000);
}

int audio_startfailed(void *arg)
{
    u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	return -1;
}

void tl_audio_stop(u8 profile, u8 pairingRef){
	audio_recTaskStop();
	tl_audioStopReqSend(profile, pairingRef);
	if(gAudioUserCb){
		gAudioUserCb(AUDIO_IDLE, SUCCESS);
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

u8 tl_audioStartRspSend(u8 profile, u8 pairingRef){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_START_RSP;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(tl_audioStartRsp_t);

		tl_audioStartRsp_t *rsp = (tl_audioStartRsp_t *)(pHdr + 1);
		rsp->status = SUCCESS;

		if(PROFILE_ZRC2 == profile){
			zrc2_vendorSpecficDataSend(pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t)+sizeof(tl_audioStartRsp_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}

u8 tl_audioStopRspSend(u8 profile, u8 pairingRef){
	u8 ret = SUCCESS;
	tl_appFrameFmt_t *msg = (tl_appFrameFmt_t*)ev_buf_allocate();
	if(msg){
		tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)msg;
		pHdr->appId = TL_SPECIFIC_ID_AUDIO;
		pHdr->cmdId = TL_CMD_AUDIO_STOP_RSP;
		pHdr->seqNo = TL_SPECIFC_SEQNO_ADD;
		pHdr->len = sizeof(tl_audioStartRsp_t);

		tl_audioStopRsp_t *rsp = (tl_audioStopRsp_t *)(pHdr + 1);
		rsp->status = SUCCESS;

		if(PROFILE_ZRC2 == profile){
			zrc2_vendorSpecficDataSend(pairingRef, (u8 *)msg, sizeof(tl_appFrameHdr_t)+sizeof(tl_audioStartRsp_t), 1);
		}
		ev_buf_free((u8 *)msg);
	}else{
		return FAILURE;
	}

	return ret;
}

u8 tl_audioDataSendCnfHandler(u8 handler, u8 *cnf){
	g_tlAppAudioBusy = 0;
#if (__PROJECT_ZRC_2_RC__)
	u8 status = ((mac_dataCnf_t *)cnf)->status;
	if(status==SUCCESS)
		g_tlAppAudioErr = 0;
	else
		g_tlAppAudioErr++;

	if(g_tlAppAudioErr>=AudioErrTimes)
	{
		audio_recTaskStop();
	}
#endif
	return 0;
}

s32 tl_audioStatusRespSend(void *arg){
	u32 rsp = (u32)arg;
	if(rsp == TL_CMD_AUDIO_START_RSP){
		tl_audioStartRspSend(PROFILE_ZRC2, 0);
	}else{
		tl_audioStopRspSend(PROFILE_ZRC2, 0);
	}
	return -1;
}

void tl_appAudioCmdHandler(u8 *pd, u8 len){
	tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)pd;


	if(pHdr->cmdId == TL_CMD_AUDIO_START_REQ){
		ev_on_timer(tl_audioStatusRespSend, (void *)TL_CMD_AUDIO_START_RSP, 2*1000);
	}else if(pHdr->cmdId == TL_CMD_AUDIO_STOP_REQ){
		ev_on_timer(tl_audioStatusRespSend, (void *)TL_CMD_AUDIO_STOP_RSP, 2*1000);
	}else if(pHdr->cmdId == TL_CMD_AUDIO_DATA_NOTIFY){
		tl_audioDataNotify(pd, len);
	}else if(pHdr->cmdId == TL_CMD_AUDIO_START_RSP){
#if (__PROJECT_ZRC_2_RC__)
		if(audioStartFailedCb)
			ev_unon_timer(&audioStartFailedCb);
		u8 *pld = ((tl_appFrameFmt_t *)pd)->payload;
		tl_audioStartRsp_t *rsp = (tl_audioStartRsp_t *)pld;
		if(rsp->status == SUCCESS){
			audio_recTaskStart();
			if(gAudioUserCb){
				gAudioUserCb(AUDIO_OPENED, rsp->status);
			}
		}
#endif
	}
}
