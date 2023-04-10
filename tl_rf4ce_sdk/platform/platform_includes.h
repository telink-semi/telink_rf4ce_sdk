/********************************************************************************************************
 * @file    platform_includes.h
 *
 * @brief   This is the header file for CHIP_MODEL
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


#if MCU_CORE_826x
#include "./tlsr_826x/register_826x.h"
#include "./tlsr_826x/clock_826x.h"
#include "./tlsr_826x/pm_826x.h"
#include "./tlsr_826x/rf_drv_826x.h"
#include "./tlsr_826x/adc_826x.h"
#include "./tlsr_826x/gpio_826x.h"
#include "./tlsr_826x/gpio_default_826x.h"
#include "./tlsr_826x/pwm_826x.h"
#include "./tlsr_826x/aes_ccm_826x.h"
#include "./tlsr_826x/uart_826x.h"
#include "./tlsr_826x/i2c_826x.h"
#include "./tlsr_826x/bsp_826x.h"
#include "./tlsr_826x/flash_826x.h"
#include "./tlsr_826x/audio_826x.h"
#include "./tlsr_826x/spi_826x.h"
#elif MCU_CORE_8258
#include "./tlsr_8258/driver_8258.h"

#elif MCU_CORE_8278
#include "./tlsr_8278/driver_8278.h"


#endif

