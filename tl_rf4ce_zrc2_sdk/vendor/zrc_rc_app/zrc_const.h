
#pragma  once
#include "../../net/rf4ce/rf4ce_includes.h"
//#include "../../proj/drivers/usbkeycode.h"
#include "../../proj/tl_common.h"
#include "../../platform/platform_includes.h"



#define ZRC_APP_MAX_ERR_CNT              10
#define ZRC_APP_NV_ITEM                  1

static const u16 batteryVoltage[] = {
		3000,
		2650,
		2450,
		2100,
		2000,
};

enum{
	BAT_LEVEL_FULL = 0,
	BAT_LEVEL_2P6_UP,
	BAT_LEVEL_2P4_6,
	BAT_LEVEL_2P3_4,
	BAT_LEVEL_2P0_3,
	BAT_LEVEL_CUTOFF  // <2.0v
};

#define 	BAT_THRESHOLD_OTA		BAT_LEVEL_2P6_UP
#define 	BAT_THRESHOLD_PAIR		BAT_LEVEL_2P3_4
#define 	BAT_THRESHOLD_FLASH		BAT_LEVEL_2P0_3
#define 	BAT_THRESHOLD_WARNING	BAT_LEVEL_2P0_3
