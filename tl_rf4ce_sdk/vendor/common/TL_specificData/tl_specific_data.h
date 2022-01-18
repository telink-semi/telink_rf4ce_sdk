/********************************************************************************************************
 * @file    tl_specific_data.h
 *
 * @brief   This is the header file for tl_specific_data.h
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

#ifndef TL_SPECIFIC_DATA_H_
#define TL_SPECIFIC_DATA_H_
#include "../../../proj/tl_common.h"
#include "tl_specific_data_audio.h"
#include "tl_specific_data_ota.h"


typedef enum{
	TL_SPECIFIC_ID_OTA 		= 0x31,
	TL_SPECIFIC_ID_AUDIO 	= 0x32,
}tl_specificDataId_e;

typedef struct{
	tl_specificDataId_e	appId;
	u8					seqNo;
	u8					cmdId;
	u8					len;
}tl_appFrameHdr_t;

typedef struct{
	tl_appFrameHdr_t	hdr;
	u8					payload[0];
}tl_appFrameFmt_t;


extern u8 	g_tlAppDataSeqNo;

#define TL_SPECIFC_SEQNO_ADD		(g_tlAppDataSeqNo++)

void tl_appDataIndicate(u8 *pd, u8 len);

#endif /* TL_AUDIO_H_ */
