/********************************************************************************************************
 * @file    drv_adc.c
 *
 * @brief   This is the source file for drv_adc.c
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

#include "../tl_common.h"
#include "./drv_adc.h"

#if defined (MCU_CORE_8258) || defined (MCU_CORE_8278)
void drv_adc_mode_pin_set(Drv_ADC_Mode mode, GPIO_PinTypeDef pin)
{
	if(mode == Drv_ADC_BASE_MODE){
		adc_base_init(pin);
	}else if(mode == Drv_ADC_VBAT_MODE){
		adc_vbat_init(pin);
	}
}


void drv_adc_enable(bool enable)
{
#if defined (MCU_CORE_8258)||defined (MCU_CORE_8278)
	adc_power_on_sar_adc(enable);
#endif
}

#else
/****
* brief: Set ADC channel,8269 only support the left and misc channel
* param[in] ad_ch, enum the channel
*
* @return
*/
void drv_adc_channel_set(Drv_ADC_ChTypeDef ad_ch)
{
	if(ad_ch == Drv_ADC_LEFT_CHN)
	{
		ADC2AUDIO();
	}
	else if(ad_ch == Drv_ADC_MISC_CHN)
	{
		AUDIO2ADC();
	}
	else
	{
		//unsupported!
		return;
	}
}

/****
* brief: set the hardware channel of analog input
* param[in] adc_chan, enum the channel,the same as the drv_adc_channel_set
* param[in] mode,single or differential mode
* param[in] pcha_p,the positive input
* param[in] pcha_n,the negative input,if the single mode,it is GND
* @return
*/
void drv_set_mode_Pcha(Drv_ADC_ChTypeDef adc_chan, DRV_ADC_InputModeTypeDef mode, u8 pcha_p, u8 pcha_n)
{
	ADC_AnaModeSet(mode);
	ADC_AnaChSet(pcha_p);
}


/****
* brief: set the sample time cycle
* param[in] adc_chan, enum the channel,the same as the drv_adc_channel_set
* param[in] sample_time,the sample time cycle,reference to the API.
* @return
*/
void drv_set_sample_time(Drv_ADC_ChTypeDef adc_chan,u8 sample_time)
{
	ADC_SampleTimeSet(sample_time);
}


/****
* brief: set the sample reference voltage
* param[in] adc_chan, enum the channel,the same as the drv_adc_channel_set
* param[in] ref_vol,the reference voltage,should be the enum mode,reference to the API.
* @return
*/
void drv_set_ref_vol(Drv_ADC_ChTypeDef adc_chan,u8 ref_vol)
{
	ADC_RefVoltageSet(ref_vol);
}

/****
* brief: set the sample resolution
* param[in] adc_chan, enum the channel,the same as the drv_adc_channel_set
* param[in] res,the reference voltage,should be the enum mode,reference to the API.
* @return
*/
void drv_set_resolution(Drv_ADC_ChTypeDef adc_chan,u8 res)
{
	ADC_ResSet(res);
}

/****
* brief: set the ADC setting parameter
* param[in] adc_chan, enum the channel
* param[in] mode,single or differential mode
* param[in] pcha_p,the positive input
* param[in] pcha_n,the negative input,if the single mode,it is GND
* param[in] sample_time,the sample time cycle,reference to the API.
* param[in] ref_vol,the reference voltage,should be the enum mode,reference to the API.
* param[in] res,the reference voltage,should be the enum mode,reference to the API.
* @return
*/
void drv_ADC_ParamSetting(Drv_ADC_ChTypeDef ad_ch,DRV_ADC_InputModeTypeDef mode,u8 pcha_p, u8 pcha_n,u8 sample_time,u8 ref_vol,u8 res)
{
	drv_adc_channel_set(ad_ch);
	drv_set_mode_Pcha(ad_ch, mode, pcha_p, pcha_n);
	drv_set_sample_time(ad_ch, sample_time);
	drv_set_ref_vol(ad_ch, ref_vol);
	drv_set_resolution(ad_ch, res);
}
#endif



/****
* brief: ADC initiate function, set the ADC clock details (4MHz) and start the ADC clock.
* param[in] null
*
* @return	  1: set success ;
*             0: set error
*/
unsigned char drv_adc_init()
{
#if	defined (MCU_CORE_826x)
	//set the ADC clock details (4MHz) and start the ADC clock.
	ADC_Init();
	AUDIO2ADC();
	return 1;
#elif  defined(MCU_CORE_8258)
	random_generator_init();
	adc_init();
	return 1;
#elif  defined(MCU_CORE_8278)
	random_generator_init();
	adc_init();
	return 1;
#endif
}


/****
* brief: get the sample data
* param[in] null
* @return,the result
*/
unsigned short drv_get_adc_data(void)
{
#if defined (MCU_CORE_826x)
//	u16 vol = 3300*(ADC_SampleValueGet() - 128)/(16384 - 256);
	u32 vol = 3*(1428*(ADC_SampleValueGet() - 128)/(16384 - 256));
//		      3*(1428*(sum - 128)/(16384 - 256));
	return vol;
#elif defined (MCU_CORE_8258)|| defined (MCU_CORE_8278)
	return (unsigned short)adc_sample_and_get_result();
#endif
}


#define BATTERY_SAFETY_THRESHOLD	2200//2.2v

void drv_adc_battery_detect_init(void){
#if	defined (MCU_CORE_826x)
	ADC_BatteryCheckInit(Battery_Chn_VCC);//drv_ADC_ParamSetting(Drv_ADC_MISC_CHN,Drv_SINGLE_ENDED_MODE,B4,B4,S_3,RV_AVDD,RES14);
	WaitUs(20);
#elif  defined(MCU_CORE_8258)
	drv_adc_mode_pin_set(Drv_ADC_VBAT_MODE, GPIO_PB7);
	drv_adc_enable(1);
	flash_safe_voltage_set(BATTERY_SAFETY_THRESHOLD);
#elif  defined(MCU_CORE_8278)
	adc_vbat_init(GPIO_PB7);
	drv_adc_enable(1);
	flash_safe_voltage_set(BATTERY_SAFETY_THRESHOLD);
#endif
}
