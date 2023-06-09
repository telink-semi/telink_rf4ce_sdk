/********************************************************************************************************
 * @file    app_edScan.c
 *
 * @brief   This is the source file for app_edScan.c
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

/*
 * emi.c
 */
#include "../../proj/tl_common.h"
#include "../../net/rf4ce/rf4ce_includes.h"
#include "app_edScan.h"

app_edScan_t g_appEdScanInfo = {0};

s32 app_energyScanCb(void *arg){
	u32 idx = g_appEdScanInfo.curChannelIdx;
	u8 curEnergy = rf_stopED();

	if(curEnergy <= ED_SCAN_LQI_THRESHOLD){
		g_appEdScanInfo.bestChannel = RF4CE_Idx2Channel(idx);
		g_appEdScanInfo.bestChannelEnergy = curEnergy;
		if(g_appEdScanInfo.doneCb){
			g_appEdScanInfo.doneCb(g_appEdScanInfo.bestChannel);
		}

		ZB_RADIO_RX_ENABLE;
		g_appEdScanInfo.scanTimer = NULL;
		return -1;
	}

	if(curEnergy < g_appEdScanInfo.bestChannelEnergy){
		g_appEdScanInfo.bestChannelEnergy = curEnergy;
		g_appEdScanInfo.bestChannel = RF4CE_Idx2Channel(idx);
	}

	/* get next channel */
	if(++g_appEdScanInfo.curChannelIdx >= g_appEdScanInfo.ChannelTotal){
		if(g_appEdScanInfo.doneCb){
			g_appEdScanInfo.doneCb(g_appEdScanInfo.bestChannel);
		}

		ZB_RADIO_RX_ENABLE;
		g_appEdScanInfo.scanTimer = NULL;
		return -1;
	}else{
		u8 channel = RF4CE_Idx2Channel(g_appEdScanInfo.curChannelIdx);
		mac_mlmeSetReq(MAC_LOGICAL_CHANNEL,  &channel);
		rf_startED();
	}

	return 0;
}

u8 app_edScanStart(u8 channelNum, u8 duration, app_edScanFinishCb_t cb){
	if(g_appEdScanInfo.scanTimer){
		return FAILURE;
	}

	g_appEdScanInfo.ChannelTotal = channelNum;
	g_appEdScanInfo.duration = duration * 10 * 1000;
	g_appEdScanInfo.curChannelIdx = 0;
	g_appEdScanInfo.bestChannelEnergy = 0xff;
	g_appEdScanInfo.doneCb = cb;

	ZB_RADIO_RX_DISABLE;
	ZB_RADIO_TX_DONE_CLR;
	ZB_RADIO_RX_DONE_CLR;
	u8 channel = RF4CE_Idx2Channel(g_appEdScanInfo.curChannelIdx);
	mac_mlmeSetReq(MAC_LOGICAL_CHANNEL,  &channel);
	rf_startED();
	g_appEdScanInfo.scanTimer = ev_on_timer(app_energyScanCb, 0, g_appEdScanInfo.duration);

	return SUCCESS;
}


