#pragma once

enum{
	ZRC_APPID_OTA = 0x01,
	ZRC_APPID_AUDIO = 0x02,
}zrcAppDataId_e;

typedef struct{
	u8 	option;   //!< option byte, 08: means more data, others(must 0)
	u8 	sqeNo;    //!< sequence number
	u8	appId;	  //!< used to identify the specific features
	u8	opCode;	  //!< used to identify the specific operations within an appID
}zrc_appDataFrmHdr_t;

void zrcApp_cmdQInit(cmdQ_t *q);
u8 zrcApp_cmdQGetSize(cmdQ_t *q);
u8 zrc_sendIRData(u8 profileId, u8 keyCode, u8 single_key);
u8 zrc_sendRFData(u8 profileId, u8 keyCode);
void zrc_stopSendRepeateDataTimer(void);
zrc_sts_t user_ZrcPowerStatusPush(u8 status);
