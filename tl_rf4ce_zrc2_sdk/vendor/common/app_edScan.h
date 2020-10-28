/*
 * emi.c
 */

#define ED_SCAN_LQI_THRESHOLD					60

typedef void (*app_edScanFinishCb_t)(u8 channel);

typedef struct{
	ev_time_event_t *scanTimer;
	app_edScanFinishCb_t doneCb;
	u32 duration;   //n * 10ms
	u8 ChannelTotal;
	u8 bestChannel;
	u8 bestChannelEnergy;
	u8 curChannelIdx;
	u8 curSeqNum;
}app_edScan_t;


u8 app_edScanStart(u8 channelNum, u8 duration, app_edScanFinishCb_t cb);


extern u8 RF4CE_Idx2Channel(u8 ch);
