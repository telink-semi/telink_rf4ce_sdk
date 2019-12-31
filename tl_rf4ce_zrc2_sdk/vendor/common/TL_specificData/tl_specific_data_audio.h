/********************************************************************************************************
 * @file     tl_specific_data_audio.h
 *
 * @brief	 some command frame format, structure/enum for Telink specific audio data
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


typedef struct{
	u16 sampleRate;
	u8 	resoutionBits;
	u8 	channelNum;
	u8 	codecType;
	u8 	pktSize;
	u8 	interval;
}tl_audioRecInfo_t;

typedef struct{
	u8 sampleRate;
	u8 resoutionBits;
	u8 channelNum;
	u8 codecType;
	u8 pktSize;
	u8 interval;
}tl_audioStartReq_t;

typedef struct{
	u8 status;
}tl_audioStartRsp_t;

typedef tl_audioStartRsp_t	tl_audioStopRsp_t ;

typedef void (*tl_audioUserCb_t)(u8 state, u8 status);

void tl_appAudioCmdHandler(u8 *pd, u8 len);

void tl_audioRecInit(u8 profileId, tl_audioRecInfo_t *info, tl_audioUserCb_t userCb);

void tl_audio_start(u8 profile, u8 pairingRef);

void tl_audio_stop(u8 profile, u8 pairingRef);

//u8 tl_audioDataSend(u8 profile, u8 *data, u8 len);

//u8 tl_audioDataSendCnfHandler(u8 handler, u8 *cnf);

#endif /* TL_AUDIO_H_ */
