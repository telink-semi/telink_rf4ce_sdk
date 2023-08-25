/********************************************************************************************************
 * @file    tl_specific_data.c
 *
 * @brief   This is the source file for tl_specific_data.c
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
#include "tl_specific_data.h"
#include "tl_specific_data_audio.h"

u8 	g_tlAppDataSeqNo = 0;

void tl_appDataIndicate(u8 *pd, u8 len){
	tl_appFrameHdr_t *pHdr = (tl_appFrameHdr_t *)pd;
	if(pHdr->appId == TL_SPECIFIC_ID_OTA){
		tl_appOtaCmdHandler(pd);
	}else if(pHdr->appId == TL_SPECIFIC_ID_AUDIO){
		tl_appAudioCmdHandler(pd, len);
	}
}



