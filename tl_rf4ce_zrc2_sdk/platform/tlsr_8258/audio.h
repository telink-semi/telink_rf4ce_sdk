/********************************************************************************************************
 * @file     audio.h
 *
 * @brief    This is the Audio driver header file for TLSR8258
 *
 * @author   junyuan.zhang@telink-semi.com;junwei.lu@telink-semi.com
 * @date     May 8, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 * @par      History:
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/

#ifndef audio_H
#define audio_H


#include "register.h"
#include "i2c.h"
#include "dfifo.h"

#define AMIC_INPUT_CHN_MOD		1			//1: Left channel  2: stereo channel


/**
 * define audio rate value.
 */
typedef enum{
	AUDIO_8K,
	AUDIO_16K,
	AUDIO_22K,
	AUDIO_32K,
	AUDIO_44K,
	AUDIO_48K,
	AUDIO_96K,
	RATE_SIZE
}AudioRate_Typedef;

/**
 * define audio input type.
 */
typedef enum{
	AMIC,
	DMIC,
	I2S_IN,
	USB_IN,
}AudioInput_Typedef;

/**
 * define codec mode.
 */
typedef enum{
	CODEC_MODE_MICPHONE_TO_HEADPHONE_LINEOUT_I2S,
	CODEC_MODE_LINE_TO_HEADPHONE_LINEOUT_I2S,
	CODEC_MODE_I2S_TO_HEADPHONE_LINEOUT,
}CodecMode_Typedef;

/**
 *  define the volume in manual mode or auto mode
 */
enum{
	AUD_VOLUME_MANUAL,
	AUD_VOLUME_AUTO,
};

/**
 * 	@brief      This function serves to set the clock of dmic
 * 	@param[in]  step -
 * 	@param[in]  mode -
 * 	@return     none
 */
static inline void audio_set_dmic_clk(unsigned char step,unsigned char mod)
{
	reg_dmic_step = step|FLD_DMIC_CLK_EN;
	reg_dmic_mod = mod;
}

/**
 * 	@brief      This function serves to set the clock of i2s
 * 	@param[in]  adr - the first address of SRAM buffer to store MIC data.
 * 	@param[in]  size - the size of adr.
 * 	@return     none
 */
static inline void audio_set_i2s_clk(unsigned char step,unsigned char mod)
{
	reg_i2s_step = step|FLD_I2S_CLK_EN;
	reg_i2s_mod = mod;
}

/**
 * 	@brief     audio amic initial function. configure ADC corresponding parameters. set hpf,lpf and decimation ratio.
 * 	@param[in] Audio_Rate - audio rate value
 * 	@return    none
 */
void audio_amic_init(AudioRate_Typedef Audio_Rate);
/**
 * 	@brief     audio DMIC init function, config the speed of DMIC and downsample audio data to required speed.         actually audio data is dmic_speed/d_samp.
 * 	@param[in] dmic_speed - set the DMIC speed. such as 1 indicate 1M and 2 indicate 2M.
 * 	@return    none.
 */
void audio_dmic_init(AudioRate_Typedef Audio_Rate);
/**
 * 	@brief     audio USB init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * 	@param[in] dmic_speed - set the DMIC speed. such as 1 indicate 1M and 2 indicate 2M.
 *
 * 	@return    none.
 */
void audio_usb_init(AudioRate_Typedef Audio_Rate);

/**
 * 	@brief     audio I2S init in function, config the speed of i2s and MCLK to required speed.
 * 	@param[in] none.
 * 	@return    none.
 */
void audio_i2s_init(void);


/**
*	@brief     sdm setting function, enable or disable the sdm output, configure SDM output paramaters
*	@param[in] audio_out_en - audio output enable or disable set, '1' enable audio output; '0' disable output
*	@param[in] sample_rate - audio sampling rate, such as 16K,32k etc.
*	@param[in] sdm_clk - SDM clock, default to be 8Mhz
*	@param[in] fhs_source - the parameter is CLOCK_SYS_TYPE. avoid CLOCK_SYS_TYPE to be modified to other word.such as SYS_TYPE etc.
*	@return	 none
*/
void audio_set_sdm_output(AudioInput_Typedef InType,AudioRate_Typedef Audio_Rate,unsigned char audio_out_en);

/**
 * @brief     This function servers to set USB input/output.
 * @param[in] none.
 * @return    none.
 */
void audio_set_usb_output(void);

/**
 *
 * 	@brief	   	i2s setting function, enable or disable the i2s output, configure i2s output paramaters
 * 	@param[in] 	InType		- select audio input type including amic ,dmic ,i2s and usb
 * 	@param[in] 	Audio_Rate 	- select audio rate, which will decide on which adc sampling rate and relative decimation configuration will be chosen .
 *
 * 	@return	  	none
 */
void audio_set_i2s_output(AudioInput_Typedef InType,AudioRate_Typedef Audio_Rate);

/**
 * @brief     This function serves to send data to USB Host.
 * @param[in] Input_type - audio input type.
 * @param[in] Audio_Rate - audio rate.
 * @return    none.
 */
void audio_tx_data_to_usb(AudioInput_Typedef Input_Type,AudioRate_Typedef Audio_Rate);

/**
 * @brief     This function serves to receive data from USB Host.
 * @param[in] none
 * @return    none.
 */
void audio_rx_data_from_usb(void);

/**
 * 	@brief     This function serves to set I2S input.
 * 	@param[in] i2c_pin_group - select the pin for I2S.
 * 	@param[in] CodecMode - select I2S mode.
 * 	@param[in] sysclk - system clock.
 * 	@return    none.
 */
void audio_set_codec(I2C_GPIO_GroupTypeDef i2c_pin_group, CodecMode_Typedef CodecMode,unsigned sysclk);

/**
 *	@brief		reg0x30[1:0] 2 bits for fine tuning, divider for slow down sample rate
 *	@param[in]	fine_tune - unsigned char fine_tune,range from 0 to 3
 *	@return	    none
 */
void audio_finetune_sample_rate(unsigned char fine_tune);

/**
 *	@brief      tune decimation shift .i.e register 0xb04 in datasheet.
 *	@param[in]  deci_shift - range from 0 to 5.
 *	@return     none
 */
unsigned char audio_tune_deci_shift(unsigned char deci_shift);

/**
 *	@brief       tune the HPF shift .i.e register 0xb05 in datasheet.
 *	@param[in]   hpf_shift - range from 0 to 0x0f
 *	@return      none
 */
unsigned char audio_tune_hpf_shift(unsigned char hpf_shift);

/**
 *	@brief     set audio volume level
 *	@param[in] input_out_sel - select the tune channel, '1' tune ALC volume; '0' tune SDM output volume
 *	@param[in] volume_level - volume level
 *	@return	 none
 */
void audio_set_volume(unsigned char input_out_sel, unsigned char volume_level);

/**
 *	@brief     automatically gradual change volume
 *	@param[in] vol_step - volume change step, the high part is decrease step while the low part is increase step
 *	@param[in] gradual_interval - volume increase interval
 *	@return    none
 */
void audio_set_volume_step(unsigned char vol_step,unsigned short gradual_interval);

#endif

/** \defgroup GP3  Audio Examples
 *
 * 	@{
 */

/*! \page audio Table of Contents
	- [API-AUDIO-CASE1:AMIC To USB](#AMIC_To_USB)
	- [API-AUDIO-CASE2:AMIC To SDM](#AMIC_To_SDM)
	- [API-AUDIO-CASE3:AMIC To I2S](#AMIC_To_I2S)
	- [API-AUDIO-CASE4:DMIC To USB](#DMIC_To_USB)
	- [API-AUDIO-CASE5:DMIC To SDM](#DMIC_To_SDM)
	- [API-AUDIO-CASE6:DMIC To I2S](#DMIC_To_I2S)
	- [API-AUDIO-CASE7:I2S To USB](#I2S_To_USB)
	- [API-AUDIO-CASE8:I2S To SDM](#I2S_To_SDM)
	- [API-AUDIO-CASE9:USB To SDM](#USB_To_SDM)
	- [API-AUDIO-CASE10:USB To I2S](#USB_To_I2S)

\n
Variables used in the following cases are defined as below
~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define	MIC_BUFFER_SIZE			4096
volatile signed short MicBuf[MIC_BUFFER_SIZE>>1];
~~~~~~~~~~~~~~~~~~~~~~~~~~~

<h1 id=AMIC_To_USB> API-AUDIO-CASE1:AMIC To USB </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | usb_irq_init() ||  enable manual interrupt | ^ |
| ^ | ^ | irq_enable() || enable global interrupt | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | gpio_set_func() <br> gpio_set_output_en() <br> gpio_set_input_en() <br> gpio_write() | gpio_set_func(GPIO_PC4, AS_GPIO) <br> gpio_set_output_en(GPIO_PC4, 1) <br> gpio_set_input_en(GPIO_PC4 ,0) <br> gpio_write(GPIO_PC4, 1) | initiate the gpio pin for BIAS of AMIC | ^ |
| ^ | ^ | audio_amic_init() | audio_amic_init(AUDIO_16K) | initiate AMIC module as audio input path | ^ |
| ^ | ^ | audio_set_usb_output() | audio_set_usb_output(AMIC,AUDIO_16K) | set USB as audio output path | ^ |
| ^ | main_loop() | usb_handle_irq_process() || handle with USB interrupt  | ^ |
| ^ | ^ | if(usb_audio_mic_cnt) || determine whether the specified interrupt flag is generated  | ^ |
| ^ | ^ | >audio_tx_data_to_usb(AMIC, AUDIO_16K) | audio_tx_data_to_usb() | send data to USB Host if the specified interrupt flag is generated  | ^ |
| ^ | ^ | >usb_audio_mic_cnt=0 || clear flag and wait for that next interrupt flag | ^ |


<h1 id=AMIC_To_SDM> API-AUDIO-CASE2:AMIC To SDM </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | gpio_set_func() <br> gpio_set_output_en() <br> gpio_set_input_en() <br> gpio_write() | gpio_set_func(GPIO_PC4, AS_GPIO) <br> gpio_set_output_en(GPIO_PC4, 1) <br> gpio_set_input_en(GPIO_PC4 ,0) <br> gpio_write(GPIO_PC4, 1) | initiate the gpio pin for BIAS of AMIC | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_amic_init() | audio_amic_init(AUDIO_16K) | initiate AMIC module as audio input path | ^ |
| ^ | ^ | audio_set_sdm_output() | audio_set_sdm_output(AMIC,AUDIO_16K,1) | set SDM as audio output path | ^ |
| ^ | main_loop() | None || Main program loop | ^ |

<h1 id=AMIC_To_I2S> API-AUDIO-CASE3:AMIC To I2S </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | gpio_set_func() <br> gpio_set_output_en() <br> gpio_set_input_en() <br> gpio_write() | gpio_set_func(GPIO_PC4, AS_GPIO) <br> gpio_set_output_en(GPIO_PC4, 1) <br> gpio_set_input_en(GPIO_PC4 ,0) <br> gpio_write(GPIO_PC4, 1) | initiate the gpio pin for BIAS of AMIC | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_amic_init() | audio_amic_init(AUDIO_16K) | initiate AMIC module as audio input path | ^ |
| ^ | ^ | audio_set_i2s_output() | audio_set_i2s_output(AMIC,AUDIO_32K) | set I2S as audio output path | ^ |
| ^ | main_loop() | None || Main program loop | ^ |

<h1 id=DMIC_To_USB> API-AUDIO-CASE4:DMIC To USB </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | usb_irq_init() ||  enable manual interrupt | ^ |
| ^ | ^ | irq_enable() || enable global interrupt | ^ |
| ^ | ^ | gpio_set_func() <br> <br> gpio_set_input_en() | gpio_set_func(GPIO_PA0, AS_DMIC) <br> gpio_set_func(GPIO_PA1, AS_DMIC) <br> gpio_set_input_en(GPIO_PA0, 1) | initiate the gpio pin for DATA and CLK of DMIC | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_dmic_init() | audio_dmic_init(AUDIO_16K) | initiate DMIC module as audio input path | ^ |
| ^ | ^ | audio_set_usb_output() | audio_set_usb_output(DMIC,AUDIO_16K) | set USB as audio output path | ^ |
| ^ | main_loop() | usb_handle_irq_process() || handle with USB interrupt  | ^ |
| ^ | ^ | if(usb_audio_mic_cnt) || determine whether the specified interrupt flag is generated  | ^ |
| ^ | ^ | >audio_tx_data_to_usb(AMIC, AUDIO_16K) | audio_tx_data_to_usb() | send data to USB Host if the specified interrupt flag is generated  | ^ |
| ^ | ^ | >usb_audio_mic_cnt=0 || clear flag and wait for that next interrupt flag | ^ |

<h1 id=DMIC_To_SDM> API-AUDIO-CASE5:DMIC To SDM </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | gpio_set_func() <br> <br> gpio_set_input_en() | gpio_set_func(GPIO_PA0, AS_DMIC) <br> gpio_set_func(GPIO_PA1, AS_DMIC) <br> gpio_set_input_en(GPIO_PA0, 1) | initiate the gpio pin for DATA and CLK of DMIC | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_dmic_init() | audio_dmic_init(AUDIO_16K) | initiate DMIC module as audio input path | ^ |
| ^ | ^ | audio_set_sdm_output() | audio_set_sdm_output(DMIC,AUDIO_16K,1) | set SDM as audio output path | ^ |
| ^ | main_loop() | None || Main program loop | ^ |

<h1 id=DMIC_To_I2S> API-AUDIO-CASE6:DMIC To I2S </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | gpio_set_func() <br> <br> gpio_set_input_en() | gpio_set_func(GPIO_PA0, AS_DMIC) <br> gpio_set_func(GPIO_PA1, AS_DMIC) <br> gpio_set_input_en(GPIO_PA0, 1) | initiate the gpio pin for DATA and CLK of DMIC | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_dmic_init() | audio_dmic_init(AUDIO_32K) | initiate DMIC module as audio input path | ^ |
| ^ | ^ | audio_set_i2s_output() | audio_set_i2s_output(AMIC,AUDIO_32K); | set I2S as audio output path | ^ |
| ^ | main_loop() | None || Main program loop | ^ |

<h1 id=I2S_To_USB> API-AUDIO-CASE7:I2S To USB </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | usb_irq_init() ||  enable manual interrupt | ^ |
| ^ | ^ | irq_enable() || enable global interrupt | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_i2s_init() || initiate I2S module as audio input path | ^ |
| ^ | ^ | audio_set_usb_output() | audio_set_usb_output(DMIC,AUDIO_16K) | set USB as audio output path | ^ |
| ^ | main_loop() | usb_handle_irq_process() || handle with USB interrupt  | ^ |
| ^ | ^ | if(usb_audio_mic_cnt) || determine whether the specified interrupt flag is generated  | ^ |
| ^ | ^ | >audio_tx_data_to_usb(AMIC, AUDIO_16K) | audio_tx_data_to_usb() | send data to USB Host if the specified interrupt flag is generated  | ^ |
| ^ | ^ | >usb_audio_mic_cnt=0 || clear flag and wait for that next interrupt flag | ^ |

<h1 id=I2S_To_SDM> API-AUDIO-CASE8:I2S To SDM </h1>

<h1 id=USB_To_SDM> API-AUDIO-CASE9:USB To SDM </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | usb_set_pin_en() || open the DP and DM of USB and enable 1.5k internal pull-up resistor | ^ |
| ^ | ^ | usb_irq_init() ||  enable manual interrupt | ^ |
| ^ | ^ | irq_enable() || enable global interrupt | ^ |
| ^ | ^ | dfifo_audio_set_mic_buf() | dfifo_audio_set_mic_buf((unsigned short*)MicBuf,MIC_BUFFER_SIZE) | set the address and size of buffer for AMIC | ^ |
| ^ | ^ | audio_set_sdm_output() | audio_set_sdm_output(DMIC,AUDIO_16K,1) | set SDM as audio output path | ^ |
| ^ | ^ | audio_set_usb_output() | audio_set_usb_output(USB_IN,AUDIO_16K) | set USB as audio output path | ^ |
| ^ | main_loop() | usb_handle_irq_process() || handle with USB interrupt  | ^ |
| ^ | ^ | if(usb_audio_speaker_cnt) || determine whether the specified interrupt flag is generated  | ^ |
| ^ | ^ | >audio_rx_data_from_usb() | audio_rx_data_from_usb() | receive data from USB Host if the specified interrupt flag is generated  | ^ |
| ^ | ^ | >usb_audio_speaker_cnt=0 || clear flag and wait for that next interrupt flag  | ^ |

<h1 id=USB_To_I2S> API-AUDIO-CASE10:USB To I2S </h1>

<h1> History Record </h1>

| Date | Description | Author |
| :--- | :---------- | :----- |
| 2019-1-10 | initial release | ZJY/LJW |


*/

 /** @}*/ //end of GP3
