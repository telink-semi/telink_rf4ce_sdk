#pragma  once

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


/**
 *  @brief  Define the frame format for Application data
 */
typedef struct{
	zrc_appDataFrmHdr_t hdr;	//!< App data frame header
	u8	payload[0];			 	//!< App data payload
}zrc_appDataFrame_t;


void zrc_appDataIndCb(u8 pairingRef, u8 *pd, u8 len);
