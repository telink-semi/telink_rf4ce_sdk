/********************************************************************************************************
 * @file     adc.h
 *
 * @brief    This is the ADC driver header file for TLSR8258
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

#pragma once

#include "bsp.h"
#include "analog.h"
#include "register.h"
#include "gpio_8258.h"

#define ADC_1000MV_CAL_ADDR		0x770c2   //1000mV volatge measured saved address.
extern volatile unsigned short adc_cal_value;
volatile unsigned short	adc_code;



/**
 *  ADC reference voltage
 */
typedef enum{
	ADC_VREF_0P6V,
	ADC_VREF_0P9V,
	ADC_VREF_1P2V,
	ADC_VREF_VBAT_N,
}ADC_RefVolTypeDef;

/**
 *  ADC Vbat divider
 */
typedef enum{
	ADC_VBAT_DIVIDER_OFF = 0,
	ADC_VBAT_DIVIDER_1F4,
	ADC_VBAT_DIVIDER_1F3,
	ADC_VBAT_DIVIDER_1F2
}ADC_VbatDivTypeDef;

/**
 *	ADC analog input negative channel
 */

typedef enum {
	NOINPUTN,
	B0N,
	B1N,
	B2N,
	B3N,
	B4N,
	B5N,
	B6N,
	B7N,
	C4N,
	C5N,
	PGA0N,
	PGA1N,
	TEMSENSORN,
	RSVD_N,
	GND,
}ADC_InputNchTypeDef;

/**
 *	ADC analog input positive channel
 */

typedef enum {
	NOINPUTP,
	B0P,
	B1P,
	B2P,
	B3P,
	B4P,
	B5P,
	B6P,
	B7P,
	C4P,
	C5P,
	PGA0P,
	PGA1P,
	TEMSENSORP,
	RSVD_P,
	VBAT,
}ADC_InputPchTypeDef;

/**
 *	ADC resolution
 */
typedef enum{
	RES8,
	RES10,
	RES12,
	RES14
}ADC_ResTypeDef;

/**
 *	ADC channel input mode
 */
typedef enum{
	SINGLE_ENDED_MODE = 0,  //single-ended mode
	DIFFERENTIAL_MODE = 1,  //differential mode
}ADC_InputModeTypeDef;

/**
 *  ADC Sampling cycles
 */
typedef enum{
	SAMPLING_CYCLES_3,
	SAMPLING_CYCLES_6,
	SAMPLING_CYCLES_9,
	SAMPLING_CYCLES_12,
	SAMPLING_CYCLES_15,
	SAMPLING_CYCLES_18,
	SAMPLING_CYCLES_21,
	SAMPLING_CYCLES_24,
	SAMPLING_CYCLES_27,
	SAMPLING_CYCLES_30,
	SAMPLING_CYCLES_33,
	SAMPLING_CYCLES_36,
	SAMPLING_CYCLES_39,
	SAMPLING_CYCLES_42,
	SAMPLING_CYCLES_45,
	SAMPLING_CYCLES_48,
}ADC_SampCycTypeDef;

/**
 * ADC input channel: Left/Right/MISC/RNS
 */
typedef enum{
	ADC_LEFT_CHN 	= BIT(0),
	ADC_RIGHT_CHN	= BIT(1),
	ADC_MISC_CHN	= BIT(2),
	ADC_RNS_CHN 	= BIT(3),
}ADC_ChTypeDef;

/**
 *	ADC Prescaler
 */
typedef enum{
	ADC_PRESCALER_1,
	ADC_PRESCALER_1F2,
	ADC_PRESCALER_1F4,
	ADC_PRESCALER_1F8
}ADC_PreScalingTypeDef;

/**
 *	ADC current trim
 */
typedef enum{
	 ADC_CUR_TRIM_PER75,
	 ADC_CUR_TRIM_PER100,
	 ADC_CUR_TRIM_PER125,
	 ADC_CUR_TRIM_PER150
}ADC_Cur_TrimTypeDef;

typedef enum{
	ADC_GAIN_STAGE_BIAS_PER75 = 0,
	ADC_GAIN_STAGE_BIAS_PER100,
	ADC_GAIN_STAGE_BIAS_PER125,
	ADC_GAIN_STAGE_BIAS_PER150,
}ADC_Gain_BiasTypeDef;

/**
 *	ADC mode: Normal mode/RNS mode
 */
typedef enum{
	ADC_NORMAL_MODE      = 0,
	ADC_RNS_MODE         = BIT(4),
}ADC_ModeTypeDef;

/**
 * ADC RNS channel source
 */
typedef enum {
	ADC_RNS_SAR_MODE   = 0,
	ADC_RNS_R_MODE_0   = 2,
	ADC_RNS_R_MODE_1   = 3,
	ADC_RNS_DAT12_MODE = 4,
	ADC_RNS_DAT5_MODE  = 6,
}ADC_RNS_SrcTypeDef;


typedef enum {
	ADC_RNS_READ_UPDATE        = BIT(3),
	ADC_RNS_CLOCK_UPDATE      = BIT(4),
}ADC_RNS_UpdateTypeDef;

/**
 * @brief      This function sets sar_adc power.
 * @param[in]  on - 1 : power on; 0 : power off.
 * @return     none
 */
static inline void adc_power_on(unsigned char on)
{
	analog_write (anareg_fc, (analog_read(anareg_fc)&(~FLD_SAR_ADC_POWER_DOWN)) | (!on)<<5  );
}

/**
 * @brief      This function reset adc module
 * @param[in]  none.
 * @return     none.
 */
static inline void	adc_reset(void)
{
	reg_rst1 = FLD_RST1_ADC;
	reg_rst1 = 0;
}


/**
 * @brief      This function enable adc source clock: external 24M
 * @param[in]  variable of source clock state 1: enable;  0: disable.
 * @return     none.
 */
static inline void adc_clk_en(unsigned int en)
{
	if(en)
	{
		analog_write(anareg_82, analog_read(anareg_82) | FLD_CLK_24M_TO_SAR_EN);
	}
	else
	{
		analog_write(anareg_82, analog_read(anareg_82) & ~FLD_CLK_24M_TO_SAR_EN);
	}
}

/**
 * @brief      This function sets adc sample clk. adc sample clk = 24M/(1+div)  div: 0~7.
 * @param[in]  div - the divider of adc sample clock.
 * @return     none
 */
static inline void adc_set_clk_div(unsigned char div)
{
	analog_write(anareg_f4,  (analog_read(0xf4) & (~FLD_ADC_SAMPLE_CLK_DIV)) | (div & 0x07) );
}

/**
 * @brief      This function sets ADC reference voltage for the MISC channel
 * @param[in]  v_ref - enum variable of adc reference voltage.
 * @return     none
 */
static inline void adc_set_misc_vref(ADC_RefVolTypeDef v_ref)
{
	analog_write(anareg_e7, ((analog_read(anareg_e7)&(~FLD_ADC_VREF_CHN_M)) | (v_ref<<4)) );
}

/**
 * @brief      This function sets ADC reference voltage for the L channel
 * @param[in]  v_ref - enum variable of adc reference voltage.
 * @return     none
 */
static inline void adc_set_left_vref(ADC_RefVolTypeDef v_ref)
{
	analog_write(anareg_e7, ((analog_read(anareg_e7)&(~FLD_ADC_VREF_CHN_L)) | (v_ref)) );
}

/**
 * @brief      This function sets ADC reference voltage for the R channel
 * @param[in]  v_ref - enum variable of adc reference voltage.
 * @return     none
 */
static inline void adc_set_right_vref(ADC_RefVolTypeDef v_ref)
{
	analog_write(anareg_e7, ((analog_read(anareg_e7)&(~FLD_ADC_VREF_CHN_R)) | (v_ref<<2) ));
}

/**
 * @brief This function serves to set the channel reference voltage.
 * @param[in]   enum variable of ADC input channel.
 * @param[in]   enum variable of ADC reference voltage.
 * @return none
 */
void adc_set_all_vref(ADC_ChTypeDef ch_n, ADC_RefVolTypeDef v_ref);

/**
 * @brief      This function select Vbat voltage divider
 * @param[in]  vbat_div - enum variable of Vbat voltage divider.
 * @return     none
 */
static inline void adc_set_vref_vbat_div(ADC_VbatDivTypeDef vbat_div)
{
	analog_write (anareg_f9, (analog_read(anareg_f9)&(~FLD_ADC_VREF_VBAT_DIV)) | (vbat_div<<2) );
}

/**
 * @brief      This function sets ADC analog negative input channel for the MISC channel
 * @param[in]  v_ain - enum variable of ADC analog negative input.
 * @return     none
 */
static inline void adc_set_misc_n_ain(ADC_InputNchTypeDef v_ain)
{
	analog_write (anareg_e8, (analog_read(anareg_e8)&(~FLD_ADC_AIN_NEGATIVE)) | (v_ain) );
}

/**
 * @brief      This function sets ADC analog positive input channel for the MISC channel
 * @param[in]  v_ain - enum variable of ADC analog positive input.
 * @return     none
 */
static inline void adc_set_misc_p_ain(ADC_InputPchTypeDef v_ain)
{
	analog_write (anareg_e8, (analog_read(anareg_e8)&(~FLD_ADC_AIN_POSITIVE)) | (v_ain<<4) );
}

/**
 * @brief      This function sets ADC analog negative input channel for the L channel
 * @param[in]  v_ain - enum variable of ADC analog negative input.
 * @return     none
 */
static inline void adc_set_left_n_ain(ADC_InputNchTypeDef v_ain)
{
	analog_write (anareg_e9, (analog_read(anareg_e9)&(~FLD_ADC_AIN_NEGATIVE)) | (v_ain) );
}

/**
 * @brief      This function sets ADC analog positive input channel for the L channel
 * @param[in]  v_ain - enum variable of ADC analog positive input.
 * @return     none
 */
static inline void adc_set_left_p_ain(ADC_InputPchTypeDef v_ain)
{
	analog_write (anareg_e9, (analog_read(anareg_e9)&(~FLD_ADC_AIN_POSITIVE)) | (v_ain<<4) );
}

/**
 * @brief      This function sets ADC analog negative input channel for the R channel
 * @param[in]  v_ain - enum variable of ADC analog negative input.
 * @return     none
 */
static inline void adc_set_right_n_ain(ADC_InputNchTypeDef v_ain)
{
	analog_write (anareg_ea, (analog_read(anareg_ea)&(~FLD_ADC_AIN_NEGATIVE)) | (v_ain) );
}

/**
 * @brief      This function sets ADC analog positive input channel for the R channel
 * @param[in]  v_ain - enum variable of ADC analog positive input.
 * @return     none
 */
static inline void adc_set_right_p_ain(ADC_InputPchTypeDef v_ain)
{
	analog_write (anareg_ea, (analog_read(anareg_ea)&(~FLD_ADC_AIN_POSITIVE)) | (v_ain<<4) );
}

/**
 * @brief This function serves to set input channel in single_ended_input_mode.
 * @param[in]   enum variable of ADC input channel.
 * @param[in]   enum variable of ADC analog positive input channel.
 * @return none
 */
void adc_set_all_single_end_ain(ADC_ChTypeDef ch_n, ADC_InputPchTypeDef InPCH);

/**
 * @brief This function serves to set input channel in differential_mode.
 * @param[in]   enum variable of ADC input channel.
 * @param[in]   enum variable of ADC analog positive input channel.
 * @param[in]   enum variable of ADC analog negative input channel.
 * @return none
 */
void adc_set_all_differential_p_n_ain(ADC_ChTypeDef ch_n, ADC_InputPchTypeDef InPCH,ADC_InputNchTypeDef InNCH);

/**
 * @brief This function serves to set pre_scaling.
 * @param[in]   enum variable of ADC pre_scaling factor.
 * @return none
 */
void adc_set_all_ain_pre_scaler(ADC_PreScalingTypeDef v_scl);
/**
 * @brief      This function sets ADC resolution for the L channel
 * @param[in]  v_res - enum variable of ADC resolution.
 * @return     none
 */
static inline void adc_set_left_resolution(ADC_ResTypeDef v_res)
{
	analog_write(anareg_eb, (analog_read(anareg_eb)&(~FLD_ADC_RES_L)) | (v_res) );
}

/**
 * @brief      This function sets ADC resolution for the R channel
 * @param[in]  v_res - enum variable of ADC resolution.
 * @return     none
 */
static inline void adc_set_right_resolution(ADC_ResTypeDef v_res)
{
	analog_write(anareg_eb, (analog_read(anareg_eb)&(~FLD_ADC_RES_R)) | (v_res<<4) );
}

/**
 * @brief      This function sets ADC resolution for the MISC channel
 * @param[in]  v_res - enum variable of ADC resolution.
 * @return     none
 */
static inline void adc_set_misc_resolution(ADC_ResTypeDef v_res)
{
	analog_write(anareg_ec, (analog_read(anareg_ec)&(~FLD_ADC_RES_M)) | (v_res) );
}

/**
 * @brief This function serves to set resolution.
 * @param[in]   enum variable of ADC input channel.
 * @param[in]   enum variable of ADC resolution.
 * @return none
 */
void adc_set_all_resolution(ADC_ChTypeDef ch_n, ADC_ResTypeDef v_res);

/**
 * @brief      This function sets ADC input mode for the MISC channel
 * @param[in]  m_input - enum variable of ADC channel input mode.
 * @return     none
 */
static inline void adc_set_misc_input_mode(ADC_InputModeTypeDef m_input)
{
	if(m_input){  //differential mode
		analog_write(anareg_ec, analog_read(anareg_ec) | FLD_ADC_EN_DIFF_CHN_M );
	}
	else{  //single-ended mode
		analog_write(anareg_ec, analog_read(anareg_ec) & (~FLD_ADC_EN_DIFF_CHN_M) );
	}
}

/**
 * @brief      This function sets ADC input mode for the L channel
 * @param[in]  m_input - enum variable of ADC channel input mode.
 * @return     none
 */
static inline void adc_set_left_input_mode(ADC_InputModeTypeDef m_input)
{
	if(m_input){  //differential mode
		analog_write(anareg_ec, (analog_read(anareg_ec) | FLD_ADC_EN_DIFF_CHN_L ));
	}
	else{  //single-ended mode
		analog_write(anareg_ec, (analog_read(anareg_ec) & (~FLD_ADC_EN_DIFF_CHN_L)));
	}
}

/**
 * @brief      This function sets ADC input mode for the R channel
 * @param[in]  m_input - enum variable of ADC channel input mode.
 * @return     none
 */
static inline void adc_set_right_input_mode(ADC_InputModeTypeDef m_input)
{
	if(m_input){  //differential mode
		analog_write(anareg_ec, analog_read(anareg_ec) | FLD_ADC_EN_DIFF_CHN_R );
	}
	else{  //single-ended mode
		analog_write(anareg_ec, analog_read(anareg_ec) & (~FLD_ADC_EN_DIFF_CHN_R) );
	}
}

/**
 * @brief This function serves to set input_mode.
 * @param[in]   enum variable of ADC input channel.
 * @param[in]   enum variable of ADC channel input mode.
 * @return none
 */
void adc_set_all_input_mode(ADC_ChTypeDef ch_n,  ADC_InputModeTypeDef m_input);

/**
 * @brief      This function sets ADC sample time(the number of adc clocks for sample cycles) for the MISC channel.
 * @param[in]  adcST - enum variable of adc sample cycles.
 * @return     none
 */
static inline void adc_set_misc_tsample_cycle(ADC_SampCycTypeDef adcST)
{
	analog_write(anareg_ee, (analog_read(anareg_ee)&(~FLD_ADC_TSAMPLE_CYCLE_CHN_M)) | (adcST) );
}
/**
 * @brief      This function sets ADC sample time(the number of adc clocks for sample cycles) for the L channel.
 * @param[in]  adcST - enum variable of adc sample cycles.
 * @return     none
 */
static inline void adc_set_left_tsample_cycle(ADC_SampCycTypeDef adcST)
{
	analog_write(anareg_ed, (analog_read(anareg_ed)&(~FLD_ADC_TSAMPLE_CYCLE_CHN_L)) | (adcST) );
}

/**
 * @brief      This function sets ADC sample time(the number of adc clocks for sample cycles) for the R channel.
 * @param[in]  adcST - enum variable of adc sample cycles.
 * @return     none
 */
static inline void adc_set_right_tsample_cycle(ADC_SampCycTypeDef adcST)
{
	analog_write(anareg_ed, (analog_read(anareg_ed)&(~FLD_ADC_TSAMPLE_CYCLE_CHN_R)) | (adcST<<4) );
}

/**
 * @brief This function serves to set sample_cycle.
 * @param[in]   enum variable of ADC input channel.
 * @param[in]   enum variable of ADC Sampling cycles.
 * @return none
 */
void adc_set_all_tsample_cycle(ADC_ChTypeDef ch_n, ADC_SampCycTypeDef adcST);

/**
 * @brief      This function sets length of each ¡°set¡± state for L channel.
 * @param[in]  r_max_s - variable of length of "set" state for L channel.
 * @return     none
 */
static inline void adc_set_all_set_state_length(unsigned char r_max_s)
{
	analog_write(anareg_f1, (analog_read(anareg_f1)&(~FLD_R_MAX_S)) | (r_max_s) );
}

/**
 * @brief      This function sets length of each ¡°set¡± state for MISC channel.
 * @param[in]  r_max_mc - variable of length of "set" state for MISC channel.
 * @return     none
 */
static inline void adc_set_misc_rns_capture_state_length(unsigned short r_max_mc)
{
	analog_write(anareg_ef,  (r_max_mc & 0x0ff));
	analog_write(anareg_f1,  ((analog_read(anareg_f1)&(~FLD_R_MAX_MC1)) | (r_max_mc>>8)<<6 ));
}

/**
 * @brief      This function sets length of each ¡°set¡± state for R channel.
 * @param[in]  r_max_c - variable of length of "set" state for R channel.
 * @return     none
 */
static inline void adc_set_left_right_capture_state_length(unsigned short r_max_c)
{
	analog_write(anareg_f0,  r_max_c & 0xff);
	analog_write(anareg_f1,  (analog_read(anareg_f1)&(~FLD_R_MAX_C1)) | (r_max_c>>8)<<4 );
}

/**
 * @brief This function serves to set state length.
 * @param[in]   r_max_mc - Value of length of "capture" state for RNS & MISC channel.
 * @param[in]   r_max_c - Value of length of "capture" state for L & R channel.
 * @param[in]   r_max_s - Value of length of "set" state for L & R & MISC channel.
 * @return none
 */
void adc_set_all_set_and_capture_state_length(unsigned short r_max_mc, unsigned short r_max_c,unsigned char r_max_s);

/**
 * @brief      This function sets ADC input channel.
 * @param[in]  ad_ch - enum variable of ADC input channel.
 * @return     none
 */
static inline void adc_set_chn_en(ADC_ChTypeDef ad_ch)
{
	analog_write(anareg_f2, (analog_read(anareg_f2)&0xf0) | ad_ch );
}

/**
 * @brief      This function sets total state index for sampling state.
 * @param[in]  s_cnt - sum of state index start with 0x0.
 * @return     none
 */
static inline void adc_set_max_state_cnt(unsigned char s_cnt)
{
	analog_write(anareg_f2, (analog_read(anareg_f2)&(~FLD_ADC_MAX_SCNT)) | ((s_cnt&0x07)<<4) );
}

/**
 * @brief      This function sets total state index for the channel.
 * @param[in]  ad_ch - enum variable of ADC input channel.
 * @param[in]  s_cnt - sum of the channel state index start with 0x0.
 * @return     none
 */
static inline void adc_set_chn_en_and_max_state_cnt(ADC_ChTypeDef ad_ch, unsigned char s_cnt)
{
	analog_write(anareg_f2, ad_ch | ((s_cnt&0x07)<<4) );
}
/**
 * @brief      This function sets pre-scaling for comparator preamp bias current trimming.
 * @param[in]  bias - enum variable of current trimming.
 * @return     none
 */
static inline void adc_set_itrim_preamp(ADC_Cur_TrimTypeDef bias)
{
	analog_write(anareg_fa, (analog_read(anareg_fa)&(~FLD_ADC_ITRIM_PREAMP)) | (bias<<0) );
}

/**
 * @brief      This function sets pre-scaling for Vref buffer bias current trimming.
 * @param[in]  bias - enum variable of current trimming.
 * @return     none
 */
static inline void adc_set_itrim_vrefbuf(ADC_Cur_TrimTypeDef bias)
{
	analog_write(anareg_fa, (analog_read(anareg_fa)&(~FLD_ADC_ITRIM_VREFBUF)) | (bias<<2) );
}

/**
 * @brief      This function sets pre-scaling for Vref(Vcmbuf) buffer bias current trimming.
 * @param[in]  bias - enum variable of current trimming.
 * @return     none
 */
static inline void adc_set_itrim_vcmbuf(ADC_Cur_TrimTypeDef bias)
{
	analog_write(anareg_fa, (analog_read(anareg_fa)&(~FLD_ADC_ITRIM_VCMBUF)) | (bias<<4) );
}

/**
 * @brief      This function serves to set mode for ADC.
 * @param[in]  adc_m - 0: normal mode; 1: RNS mode.
 * @return     none
 */
static inline void adc_set_mode(ADC_ModeTypeDef adc_m)
{
	analog_write (anareg_fc, (analog_read(anareg_fc)&(~FLD_ADC_MODE)) | adc_m);
}

/**
 * @brief      This function sets PGA-Left-channel power.
 * @param[in]  on -  1 : power on; 0 : power off.
 * @return     none
 */
static inline void adc_set_pga_left_power_on(unsigned char on)
{
	analog_write (anareg_fc, (analog_read(anareg_fc)&(~FLD_POWER_DOWN_PGA_CHN_L)) | (!on)<<6 );
}

/**
 * @brief      This function sets PGA Right channel power.
 * @param[in]  on -  1 : power on; 0 : power off.
 * @return     none
 */
static inline void adc_set_pga_right_power_on(unsigned char on)
{
	analog_write (anareg_fc, (analog_read(anareg_fc)&(~FLD_POWER_DOWN_PGA_CHN_R)) | (!on)<<7 );
}

/**
 * @brief      This function sets left boost bias with PGA enable.
 * @param[in]  bias - Value of gain_stage.
 * @return     none
 */
static inline void adc_set_left_boost_bias(ADC_Gain_BiasTypeDef bias)
{
	analog_write(anareg_fb, (analog_read(anareg_fb)&(~FLD_PGA_ITRIM_BOOST_L)) | (bias<<4) | FLD_PGA_CAP_TRIM_EN_L );
}

/**
 * @brief      This function sets right boost bias with PGA enable.
 * @param[in]  bias - Value of gain_stage.
 * @return     none
 */
static inline void adc_set_right_boost_bias(ADC_Gain_BiasTypeDef bias)
{
	analog_write(anareg_fb, (analog_read(anareg_fb)&(~FLD_PGA_ITRIM_BOOST_R)) | (bias<<6) |  FLD_PGA_CAP_TRIM_EN_R);
}

/**
 * @brief      This function gets left gain bias with PGA enable.
 * @param[in]  bias - Value of gain_stage.
 * @return     none
 */
static inline void adc_set_left_gain_bias(ADC_Gain_BiasTypeDef bias)
{
	analog_write(anareg_fc, (analog_read(anareg_fc)&(~FLD_PGA_ITRIM_GAIN_L)) | (bias) );
}

/**
 * @brief      This function gets right_gain_bias with PGA enable.
 * @param[in]  bias - Value of gain_stage.
 * @return     none
 */
static inline void adc_set_right_gain_bias(ADC_Gain_BiasTypeDef bias)
{
	analog_write(anareg_fc, (analog_read(anareg_fc)&(~FLD_PGA_ITRIM_GAIN_R)) | (bias<<2) );
}


/**
 * @brief This function serves to ADC init.
 * @param[in]   none
 * @return none
 */
void adc_init(void );

/**
 * @brief This function is used for IO port configuration of ADC IO port voltage sampling.
 * @param[in]   GPIO_PinTypeDef pin
 * @return none
 */
void adc_base_pin_init(GPIO_PinTypeDef pin);

/**
 * @brief This function is used for IO port configuration of ADC supply voltage sampling.
 * @param[in]   GPIO_PinTypeDef pin
 * @return none
 */
void adc_vbat_pin_init(GPIO_PinTypeDef pin);

/**
 * @brief This function is used for ADC configuration of ADC IO voltage sampling.
 * @param[in]   GPIO_PinTypeDef pin
 * @return none
 */

void adc_base_init(GPIO_PinTypeDef pin);

/**
 * @brief This function is used for ADC configuration of ADC supply voltage sampling.
 * @param[in]   GPIO_PinTypeDef pin
 * @return none
 */

void adc_vbat_init(GPIO_PinTypeDef pin);

/**
 * @brief This function serves to set adc sampling and get results.
 * @param[in]  none.
 * @return the result of sampling.
 */
unsigned int adc_set_sample_and_get_result(void);

#if(BLE_SDK_EN)
#define adc_power_on_sar_adc							adc_power_on
#define adc_reset_adc_module							adc_reset
#define adc_enable_clk_24m_to_sar_adc					adc_clk_en
#define adc_set_vref_chn_misc							adc_set_misc_vref
#define adc_set_vref_chn_left							adc_set_left_vref
#define adc_set_vref_chn_right							adc_set_right_vref
#define adc_set_ref_voltage								adc_set_all_vref
#define adc_set_ain_negative_chn_misc					adc_set_misc_n_ain
#define adc_set_ain_positive_chn_misc					adc_set_misc_p_ain
#define adc_set_ain_negative_chn_left					adc_set_left_n_ain
#define adc_set_ain_positive_chn_left					adc_set_left_p_ain
#define adc_set_ain_negative_chn_right					adc_set_right_n_ain
#define adc_set_ain_positive_chn_right					adc_set_right_p_ain
#define adc_set_ain_channel_single_ended_input_mode		adc_set_all_single_end_ain
#define adc_set_ain_channel_differential_mode			adc_set_all_differential_p_n_ain
#define adc_set_ain_pre_scaler							adc_set_all_ain_pre_scaler
#define adc_set_resolution_chn_misc						adc_set_misc_resolution
#define adc_set_resolution_chn_left						adc_set_left_resolution
#define adc_set_resolution_chn_right					adc_set_right_resolution
#define adc_set_resolution								adc_set_all_resolution
#define adc_set_input_mode_chn_misc						adc_set_misc_input_mode
#define adc_set_input_mode_chn_left						adc_set_left_input_mode
#define adc_set_input_mode_chn_right					adc_set_right_input_mode
#define adc_set_input_mode								adc_set_all_input_mode
#define adc_set_tsample_cycle_chn_misc					adc_set_misc_tsample_cycle
#define adc_set_tsample_cycle_chn_left					adc_set_left_tsample_cycle
#define adc_set_tsample_cycle_chn_right					adc_set_right_tsample_cycle
#define adc_set_tsample_cycle							adc_set_all_tsample_cycle
#define adc_set_length_set_state						adc_set_all_set_state_length
#define adc_set_length_capture_state_for_chn_misc_rns	adc_set_misc_rns_capture_state_length
#define adc_set_length_capture_state_for_chn_left_right	adc_set_left_right_capture_state_length
#define adc_set_state_length							adc_set_all_set_and_capture_state_length
#define adc_set_chn_enable								adc_set_chn_en

#define adc_set_chn_enable_and_max_state_cnt			adc_set_chn_en_and_max_state_cnt

#define pga_left_chn_power_on							adc_set_pga_left_power_on
#define pga_right_chn_power_on							adc_set_pga_right_power_on

#endif

/** \defgroup GP1  ADC Examples
 *
 * 	@{
 */

/*! \page adc Table of Contents
	- [API-ADC-CASE1:ADC BASE MODE](#ADC_BASE_MODE)
	- [API-ADC-CASE2:ADC VBAT MODE](#ADC_VBAT_MODE)
	- [API-ADC-CASE3:ADC RNG MODE](#ADC_RNG_MODE)
	- [API-ADC-CASE4:ADC TEMP MODE](#ADC_TEMP_MODE)

<h1 id=ADC_BASE_MODE> API-ADC-CASE1:ADC BASE MODE </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | adc_init()|| initiate ADC module  | ^ |
| ^ | ^ | adc_base_init()| adc_base_init(GPIO_PB0)| initiate ADC module in the BASE mode | ^ |
| ^ | ^ | adc_power_on() | adc_power_on(1) | Power on ADC module | ^ |
| ^ | main_loop() | base_val = adc_set_sample_and_get_result() || get the result in main program loop | ^ |

<h1 id=ADC_VBAT_MODE> API-ADC-CASE2:ADC VBAT MODE </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | adc_init()|| initiate ADC module  | ^ |
| ^ | ^ | adc_vbat_init()| adc_vbat_init(GPIO_PB0)| initiate ADC module in the BASE mode | ^ |
| ^ | ^ | adc_power_on() | adc_power_on(1) | Power on ADC module | ^ |
| ^ | main_loop() | vbat_val = adc_set_sample_and_get_result() || get the result in main program loop | ^ |

<h1 id=ADC_RNG_MODE> API-ADC-CASE3:ADC RNG MODE </h1>

| Function | Sub-Function | APIs || Description | Update Status |
| :------- | :----------- | :---------- | :---------- |:---------- | :------------ |
| irq_handler() | None ||| Interrupt handler function [**Mandatory**] | 2019-1-10 |
| main() | system_init() ||| CPU initialization function [**Mandatory**] | ^ |
| ^ | clock_init() | clock_init(SYS_CLK_24M_XTAL) || Clock initialization function, System Clock is 24M RC by default [**optional**] | ^ |
| ^ | rf_mode_init() | rf_mode_init(RF_MODE_BLE_1M) || RF mode initialization [**optional**] | ^ |
| ^ | gpio_init() ||| GPIO initialization: set the initialization status of all GPIOs [**optional**] | ^ |
| ^ | user_init() | rng_init()|| initiate RNG  | ^ |
| ^ | main_loop() | rns_val = rand() || get the result in main program loop | ^ |

<h1 id=ADC_TEMP_MODE> API-ADC-CASE4:ADC TEMP MODE </h1>

<h1> History Record </h1>

| Date | Description | Author |
| :--- | :---------- | :----- |
| 2019-1-10 | initial release | ZJY/LJW |


*/

 /** @}*/ //end of GP1


