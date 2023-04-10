/********************************************************************************************************
 * @file    tl_specific_data_audio.h
 *
 * @brief   This is the header file for tl_specific_data_audio.h
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

#ifndef TL_SPECIFIC_DATA_AUDIO_H_
#define TL_SPECIFIC_DATA_AUDIO_H_

/*
 * audio command
 * */
typedef enum{
	TL_CMD_AUDIO_START_REQ 		= 0x01,
	TL_CMD_AUDIO_STOP_REQ 		= 0x02,
	TL_CMD_AUDIO_DATA_NOTIFY 	= 0x03,
	TL_CMD_AUDIO_START_RSP 		= 0x81,
	TL_CMD_AUDIO_STOP_RSP 		= 0x82,
	TL_CMD_AUDIO_DATA_RSP 		= 0x83,
}tl_audioCmdId_e;

enum{
	TL_AUDIO_SAMPLE_RATE_8K = 0x01,
	TL_AUDIO_SAMPLE_RATE_16K = 0x02,
};

enum{
	TL_AUDIO_CODEC_TYPE_ADPCM = 0x01,
};

enum{
	AUDIO_ED_SCAN_STA_IDLE,
	AUDIO_ED_SCAN_STA_DOING,
	AUDIO_ED_SCAN_STA_DONE
};
enum{
	TL_AUDIO_STA_START = 0x90, 		//!< audio start
	TL_AUDIO_STA_TIMEOUT = 0x91, 		//!< audio start timeout
	TL_AUDIO_STA_STOP = 0x92, 		//!< audio stop
};





typedef struct{
	u16 sampleRate;
	u8 	resoutionBits;
	u8 	micChlNum;
	u8 	codecType;
	u8 	pktSize;
	u8 	interval;
}tl_audioRecInfo_t;

typedef struct{
	u16 sampleRate;
	u8 resoutionBits;
	u8 micChlNum;
	u8 codecType;
	u8 pktSize;
	u8 interval;
	u8 channelNum;
	u8 duration;
}tl_audioStartReq_t;

typedef struct{
	u8 bestChannel;
}tl_audioStartRsp_t;

typedef struct{
	u8 state;
}tl_audioStopRsp_t;

typedef void (*audioUserCb)(u8 state, u8 status);
/**
 *  @brief  Define the structure for audio various
 */
typedef struct{
	ev_time_event_t *dataReqTimeoutCb;  //!< time out callback for data request
	audioUserCb audioCb; //!< audio call back for application
	u8  channelNum;
	u8  scanDuration;
	u8  scanSta;
	u8	pairingRef;
	u8	dataReqRetries;
}audioVars_t;



typedef void (*tl_audioUserCb_t)(u8 state, u8 status);

void tl_appAudioCmdHandler(u8 *pd, u8 len);

void tl_audioRecInit(u8 profileId, tl_audioRecInfo_t *info);

void tl_audio_start(u8 profile, u8 pairingRef);

void tl_audio_stop(u8 profile);

int audio_startsendtry(void *arg);

u8 app_audioStart(u8 channelNum, u8 duration, audioUserCb cb);
//u8 tl_audioDataSend(u8 profile, u8 *data, u8 len);

//u8 tl_audioDataSendCnfHandler(u8 handler, u8 *cnf);

#endif /* TL_AUDIO_H_ */
