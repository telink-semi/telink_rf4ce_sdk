/********************************************************************************************************
 * @file     drv_audio.h
 *
 * @brief	 audio function API defined here
 *
 * @author
 * @date     July. 20, 2019
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
#include "../../../platform/platform_includes.h"
#include "audio_codec_adpcm.h"
#define DMIC_ENABLE		0
#define AMIC_ENABLE		1

enum{
	AUDIO_IDLE 		= 0,
	AUDIO_OPENING 	= (1 << 0),
	AUDIO_OPENED 	= (1 << 1),
	AUDIO_CLOSING 	= (1 << 2),
};

typedef enum{
	AUDIO_SAMPLE_RATE_8K	= 8000,
	AUDIO_SAMPLE_RATE_16K	= 16000,
}audio_sampleRate_e;

#ifndef		TL_MIC_PACKET_BUFFER_NUM
#define		TL_MIC_PACKET_BUFFER_NUM    32
#endif

/*
 * buffer for audio data from MIC
* */
#define	BUFFER_PACKET_SIZE		((ADPCM_PACKET_LEN >> 2) * TL_MIC_PACKET_BUFFER_NUM)

#define	AMIC_PIN_CFG_ON(inPin0, inPin1, biasPin)  do \
		{										\
			if(inPin0)							\
			{									\
			gpio_set_func(inPin0, AS_GPIO);		\
			gpio_set_output_en (inPin0, 0);		\
			gpio_set_input_en (inPin0, 0);		\
			}									\
			if(inPin1)							\
			{									\
			gpio_set_func(inPin1, AS_GPIO);		\
			gpio_set_output_en (inPin1, 0);		\
			gpio_set_input_en (inPin1, 0);		\
			}									\
			if(biasPin)							\
			{									\
			gpio_set_func(biasPin, AS_GPIO);    \
			gpio_set_output_en (biasPin, 1);	\
			gpio_set_input_en (biasPin, 0);		\
			gpio_set_data_strength (biasPin, 1);\
			gpio_write (biasPin, 1);			\
			}									\
		}while(0);

#define	AMIC_PIN_CFG_OFF(inPin0, inPin1, biasPin)  do \
		{										\
			if(inPin0)							\
			{									\
			gpio_set_func(inPin0, AS_GPIO);		\
			gpio_set_output_en (inPin0, 0);		\
			gpio_set_input_en (inPin0, 0);		\
			}									\
			if(inPin1)							\
			{									\
			gpio_set_func(inPin1, AS_GPIO);		\
			gpio_set_output_en (inPin1, 0);		\
			gpio_set_input_en (inPin1, 0);		\
			}									\
			if(biasPin)							\
			{									\
			gpio_set_func(biasPin, AS_GPIO);    \
			gpio_set_output_en (biasPin, 1);	\
			gpio_set_input_en (biasPin, 0);		\
			gpio_set_data_strength (biasPin, 0);\
			gpio_write (biasPin, 0);			\
			}									\
		}while(0);

typedef u8 (*audio_rec_ntf)(u8 *data, u8 len);

/*
 * audio recorder init
 *
 * */
void audio_recInit(u32 sampleRate, audio_rec_ntf audioUserhandler);


void audio_recTaskStart(void);

void audio_recTaskStop(void);

u8 audio_recTaskStatusGet(void);

void audio_decInit(void);

void abuf_mic_add (u32 *p);

void abuf_mic_dec (void);

void SetAudioTxState(bool flag);

void SetAudioTxCnt(u8 cnt);

bool GetAudioTxState(void);

u8 GetAudioTxCnt(void);

void abuf_dec_usb (void);
