
#include "../../proj/tl_common.h"
#include "../../proj/common/types.h"
#include "../../proj/drivers/nv.h"
#include "nwk/nwk_const.h"
#include "nwk/nwk_api.h"

#if (RF4CE_ZID_ENABLE)
#include "profile/profile_attr.h"
#include "profile/profile_api.h"
#include "profile/zid/zid_const.h"
#include "profile/zid/zid_api.h"
#endif

#if (RF4CE_ZRC_ENABLE)
#include "profile/profile_attr.h"
#include "profile/profile_api.h"
#include "profile/zrc/zrc_api.h"
#include "profile/zrc_profileCommands.h"
#endif

#if (RF4CE_ZRC2_ENABLE)
#include "profile/profile_attr.h"
#include "profile/profile_api.h"
#include "profile/zrc2/zrc_api.h"
#include "profile/zrc2/zrc.h"
#include "profile/zrc_profileCommands.h"
#endif
#if (RF4CE_MSO_ENABLE)
#include "profile/profile_attr.h"
#include "profile/profile_api.h"
#include "profile/mso/mso_api.h"
#endif

/* Pairing Table related variables */
const u8 nwk_maxPairTableSize = RF4CE_MAX_PAIR_ENTRY_NUM;
pairTable_t nwkPairingTable[RF4CE_MAX_PAIR_ENTRY_NUM];

/* zrc command supported table */
#ifndef ZRCCmdSupportedBitMap
#define ZRCCmdSupportedBitMap   {0}
#endif
const u8 devSupportedCmd[] = ZRCCmdSupportedBitMap;

/* this variable is used for controller to store the cmd supported by remote device */
#ifndef ZRC_CmdSupportedFiledLen
#define ZRC_CmdSupportedFiledLen   32
#endif
//u8 supportedCmdTable[RF4CE_MAX_PAIR_ENTRY_NUM * ZRC_CmdSupportedFiledLen];

const u8 rf4ce_security = SECURITY_ENABLE;

#if (ENABLE_FREQ_AGILITY)
const u8 freqAgilitySampleTime = FREQUENCY_AGILITY_SAMPLE_INTERVAL;
const u8 freqAgilityTimesPerCh = FREQUENCY_AGILITY_TIMES_PER_CH;
#if (CERTIFICATION_TEST)
s8 freqAgilityEDThreshod   = FREQUENCY_AGILITY_ED_THRESHOD;
#else
s8 freqAgilityEDThreshod   = FREQUENCY_AGILITY_ED_THRESHOD;
#endif
s8 freqAgilityNoisyThreshod = FREQUENCY_AGILITY_NOISY_THRESHOD;
#endif

#if (ZID_HID_ADAPTER)
zid_proxy_entry_t zid_proxyTable[RF4CE_MAX_PAIR_ENTRY_NUM];
#endif

#define RF4CE_OP_CHAN_NUM	3

u8 rf4ce_opChanNum  = RF4CE_OP_CHAN_NUM;
u8 rf4ce_opChanList[RF4CE_OP_CHAN_NUM] = {15, 20, 25};

u8 RF4CE_Channel2Idx(u8 ch){
	for(u32 i = 0; i < RF4CE_OP_CHAN_NUM; i++){
		if(ch == rf4ce_opChanList[i]){
			return i;
		}
	}
	return 1;
}

u8 RF4CE_Idx2Channel(u8 ch){
	return rf4ce_opChanList[ch];
}

u8 RF4CENextChannel(u8 ch) {
	for(u32 i = 0; i < RF4CE_OP_CHAN_NUM; i++){
		if(ch == rf4ce_opChanList[i]){
			if(i == RF4CE_OP_CHAN_NUM-1){
				return rf4ce_opChanList[0];
			}
			return rf4ce_opChanList[i+1];
		}
	}
	return rf4ce_opChanList[0];
}

u8 RF4CEPreviousChannel(u8 ch){
	for(u32 i = 0; i < RF4CE_OP_CHAN_NUM; i++){
		if(ch == rf4ce_opChanList[i]){
			if(i == 0){
				return rf4ce_opChanList[RF4CE_OP_CHAN_NUM-1];
			}
			return rf4ce_opChanList[i-1];
		}
	}
	return rf4ce_opChanList[0];
}


#if (RF4CE_ZRC2_ENABLE)
//    const u16 APP_VENDOR_ID_LIST[] = {0x1141};//{0x1141}; //{0x10EF};  //{0x1141};//{0x10EF};
    zrc_tgtRcTbl_t zrc_tgtRcTable[RF4CE_MAX_PAIR_ENTRY_NUM];

    #if RF4CE_CONTROLLER
        const u8 ZRC2_CLASS_FILTER = 0xe0;
        const u8 ZRC2_MIN_LQI_FILTER = 0x00;
		const u16 ZRC2_FILTER_VENDOR_ID = 0xffff;//0x1234;
        zrc2_tgtPib_t  zrc2_tgtRibAttrTbl[RF4CE_MAX_PAIR_ENTRY_NUM];
		gdp_tgtAttr_t gdp_tgtAttrValTbl[RF4CE_MAX_PAIR_ENTRY_NUM];
	#else
        const u8 ZRC2_DEFALUT_CLASS_DESP = 0x22;
        const u8 ZRC2_DEFALUT_LQI_THRESHOLD = 0x00;
        //const u16 ZRC_VENDOR_ID = 0x1234;
	    zrc2_rcPib_t zrc2_rcRibAttrTbl[RF4CE_MAX_PAIR_ENTRY_NUM];
		gdp_rcAttr_t gdp_rcAttrValTbl[RF4CE_MAX_PAIR_ENTRY_NUM];
    #endif
    const u8 ZRC2_APP_VENDOR_STRING[] = {'T', 'L', 0x20, 0x20, 0x20, 0x20}; //{'T', 'L', 0x20, 0x20, 0x20};  //{'U', 'E', 'I', 0x20, 0x20}; //{'U', 'E', 'I', 0x20, 0x20};//{'T', 'E', 'L', 'I', 'N', 'K', 0x20};
    const u8 ZRC2_APP_USER_STRING[] = {'S', 'R', '-', '0', '0', '1', '-', 'U', 0, 0,0,0,0,0,0}; // {'H', 'D', 'u', 'D', 'T', 'A', 0, 0, 0, 0, 0}; //"SR-002-U";//{'T', 'E', 'L', 'I', 'N', 'K', 0, 0, 0};//{'G', 'P', 'R', 'E', 'F', 0, 0, '9', 0};//{'T', 'e', 'l', 'i', 'n', 'k', 0, 0, 0};

#endif
#if (RF4CE_MSO_ENABLE )
mso_tgtRcTbl_t mso_tgtRcTable[RF4CE_MAX_PAIR_ENTRY_NUM];
const u8 mso_maxKeyCnt = MSO_MAX_KEY_CNT_NUM;
mso_ribAttr_t mso_ribAttrTbl[RF4CE_MAX_PAIR_ENTRY_NUM];
const u8 mso_maxGeneralPurposeCnt = MSO_MAX_GENERAL_PURPOSE_CNT;


#define APP_VENDOR_ID_SUPPPORT_MAX	3
//const u16 APP_VENDOR_ID_LIST[] = {0x1141};   //{0x10D0, 0x10bc,0x10cb};
//u16 *MSO_APP_VENDOR_ID_LIST = &APP_VENDOR_ID_LIST[0];
//const u8 MSO_APP_VENDOR_STRING[] = {'T', 'L', 0, 0, 0, 0, 0};
//const u8 MSO_APP_USER_STRING[] = {'T', 'e', 'l', 'i', 'n', 'k', 0, 0, 0};

#ifndef WIN32
#if ( NV_HEADER_TABLE_SIZE < MSO_MAX_KEY_CNT_NUM )
//#error "MSO Key Cnt EXCEEDS the maximum number. You can change NV_HEADER_TABLE_SIZE"
#endif
#endif

#endif
