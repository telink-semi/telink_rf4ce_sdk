/********************************************************************************************************
 * @file    dongle_info.h
 *
 * @brief   This is the header file for dongle_info.h
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

#pragma  once

//enum version{
//    MAJOR_VER = 1,
//    MINOR_VER = 0,
//    PATCH_VER = 1,
//    BUILD_VER = 4
//};
#define firmwareVersion 	0x01000105
#define APP_VENDOR_ID_LIST  0x1141
#if (MCU_CORE_8258)
#define CHIP_ID  0x8258
#elif (MCU_CORE_826x)
#define CHIP_ID  0x8269
#elif (MCU_CORE_8278)
#define CHIP_ID  0x8278
#else
#define CHIP_ID  0x0000
#endif
