/********************************************************************************************************
 * @file    drv_adc.h
 *
 * @brief   This is the header file for drv_adc.h
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

#include "../common/types.h"
#include "../common/compiler.h"


/****
* brief: ADC initiate function, set the ADC clock details (4MHz) and start the ADC clock.
* param[in] null
*
* @return	  1: set success ;
*             0: set error
*/
unsigned char drv_adc_init();

/****
* brief: get the sample data
* param[in] null
* @return,the result
*/
unsigned short drv_get_adc_data();

void drv_adc_battery_detect_init(void);

unsigned short drv_get_adc_data(void);


#if defined (MCU_CORE_8258) || defined (MCU_CORE_8278)

typedef enum{
	Drv_ADC_BASE_MODE,
	Drv_ADC_VBAT_MODE,
}Drv_ADC_Mode;

/****
* brief: set ADC test mode and pin
* param[in]
* @return
*/
void drv_adc_mode_pin_set(Drv_ADC_Mode mode, GPIO_PinTypeDef pin);

/****
* brief: set ADC calibrated value
* param[in]
* @return
*/
void drv_adc_set_calValue(unsigned short value);

/****
* brief: start/stop ADC
* param[in] TRUE/FALSE
* @return
*/
void drv_adc_enable(bool enable);
#else

typedef enum{
	Drv_ADC_LEFT_CHN 	= BIT(0),
	Drv_ADC_RIGHT_CHN	= BIT(1),
	Drv_ADC_MISC_CHN	= BIT(2),
	Drv_ADC_RNS_CHN		= BIT(3),
}Drv_ADC_ChTypeDef;

typedef enum{
	Drv_SINGLE_ENDED_MODE = 0,  //single-ended mode
	Drv_DIFFERENTIAL_MODE = 1,  //differential mode
}DRV_ADC_InputModeTypeDef;

/****
* brief: set the ADC setting parameter
* param[in] adc_chan, enum the channel
* param[in] mode,single or differential mode
* param[in] pcha_p,the positive input pin
* param[in] pcha_n,the negative input,if the mode is the single mode,it is GND
* param[in] sample_time,the sample time cycle,reference to the API.
* param[in] ref_vol,the reference voltage,should be the enum mode,reference to the API.
* param[in] res,the sample resolution,should be the enum mode,reference to the API.
* @return
*/
void drv_ADC_ParamSetting(Drv_ADC_ChTypeDef ad_ch,DRV_ADC_InputModeTypeDef mode,u8 pcha_p, u8 pcha_n,u8 sample_time,u8 ref_vol,u8 res);
#endif
