/********************************************************************************************************
 * @file    user_config.h
 *
 * @brief   This is the header file for user_config.h
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

#pragma once

#if (__PROJECT_ZRC_2_RC__) || (__PROJECT_MSO_RC__)
	#include "../rc_app/app_config.h"
#elif (__PROJECT_ZRC_2_DONGLE__) ||(__PROJECT_MSO_DONGLE__)
	#include "../dongle_app/app_config.h"
#else
	#include "user_config_common.h"
#endif



