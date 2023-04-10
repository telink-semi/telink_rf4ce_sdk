/********************************************************************************************************
 * @file    drv_audio.h
 *
 * @brief   This is the header file for drv_audio.h
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
