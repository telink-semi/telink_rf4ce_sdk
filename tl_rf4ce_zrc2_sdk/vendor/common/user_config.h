
#pragma once

#if (__PROJECT_ZRC_2_RC__)
	#include "../zrc_rc_app/app_config.h"
#elif (__PROJECT_ZRC_2_DONGLE__)
	#include "../zrc_dongle_app/app_config.h"
#else
	#include "user_config_common.h"
#endif



