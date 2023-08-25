/********************************************************************************************************
 * @file    audio_rec_proc.c
 *
 * @brief   This is the source file for audio_rec_proc.c
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

#include "drv_audio.h"
#include "audio_codec_adpcm.h"
#include "../drv_adc.h"
#include "../../../net/rf4ce/mac/mac_phy.h"
#include "../../../net/rf4ce/rf4ce_includes.h"
#include "../drv_timer.h"
#if TL_MIC_BUFFER_SIZE
#define		APP_AUDIO_HDR_LEN		4
static s16 	buffer_mic[TL_MIC_BUFFER_SIZE>>1];
static int	buffer_mic_enc[BUFFER_PACKET_SIZE];  //adpcm buffer
static u8	buff_adpcm_temp[128+2] _attribute_aligned_(4);
u8			buffer_mic_pkt_wptr = 0;
u8			buffer_mic_pkt_rptr = 0;

u8 audio_status = AUDIO_IDLE;
int audio_delay_times = 0;
u8 TIMER_FOR_USER   = TIMER_IDX_0;
audio_rec_ntf  g_audioRecNtf = NULL;
int AudioTimeOutTxCb(void* arg);
bool GetAudioTxState(void);

#if MCU_CORE_826x
/*
 * collection unit: mic data to adpcm
 *
 * */
static void proc_mic_encoder_826x (void){
	static u16	buffer_mic_rptr;
	u16 mic_wptr = reg_audio_wr_ptr;
	u16 l = ((mic_wptr<<1) >= buffer_mic_rptr) ? ((mic_wptr<<1) - buffer_mic_rptr) : 0xffff;

	if (l >=(TL_MIC_BUFFER_SIZE>>2)) {
		s16 *ps = buffer_mic + buffer_mic_rptr;
		mic_to_adpcm_split (	ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>2);
		buffer_mic_pkt_wptr++;
		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
		}
		if(audio_delay_times>0)
			audio_delay_times--;
	}
}


static void audio_recInit_826x(u32 sampleRate, audio_rec_ntf audioUserhandler){
	g_audioRecNtf = audioUserhandler;
	/*
	 * audio buffer configure
	 * */
	audio_config_mic_buf((unsigned short*)buffer_mic, TL_MIC_BUFFER_SIZE);

#if DMIC_ENABLE
    config_dmic (sampleRate);

#elif AMIC_ENABLE
    /*
     * sample rate configure
     * */
    if(sampleRate == AUDIO_SAMPLE_RATE_8K){
#if TL_MIC_32K_FIR_16K
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		audio_amic_init( DIFF_MODE, 26,  9, R4, CLOCK_SYS_TYPE);//R4
		audio_finetune_sample_rate(2);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		audio_amic_init( DIFF_MODE, 33, 15, R4, CLOCK_SYS_TYPE);
		audio_finetune_sample_rate(3);
	#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
		audio_amic_init( DIFF_MODE, 45, 20, R4, CLOCK_SYS_TYPE);
	#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
		audio_amic_init( DIFF_MODE, 65, 15, R6, CLOCK_SYS_TYPE);
	#endif
#else
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
			audio_amic_init( DIFF_MODE, 18,  11, R8, CLOCK_SYS_TYPE);//R2
			audio_finetune_sample_rate(2);
	#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
			audio_amic_init( DIFF_MODE, 18,  11, R8, CLOCK_SYS_TYPE);//R2
			audio_finetune_sample_rate(2);
	#endif
#endif
    }else if(sampleRate == AUDIO_SAMPLE_RATE_16K){
#if TL_MIC_32K_FIR_16K
		#if (CLOCK_SYS_CLOCK_HZ == 16000000)
			audio_amic_init( DIFF_MODE, 26,  9, R2, CLOCK_SYS_TYPE);//R2
			audio_finetune_sample_rate(2);  //reg0x30[1:0] 2 bits for fine tuning, divider for slow down sample rate
		#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
			audio_amic_init( DIFF_MODE, 33, 15, R2, CLOCK_SYS_TYPE);
			audio_finetune_sample_rate(3);
		#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
			audio_amic_init( DIFF_MODE, 45, 20, R2, CLOCK_SYS_TYPE); // 16 , 15
		#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
			audio_amic_init( DIFF_MODE, 65, 15, R3, CLOCK_SYS_TYPE);
		#endif
#else
		#if (CLOCK_SYS_CLOCK_HZ == 16000000)
			audio_amic_init( DIFF_MODE, 26,  9, R4, CLOCK_SYS_TYPE);//R4
			audio_finetune_sample_rate(2);
		#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
			audio_amic_init( DIFF_MODE, 33, 15, R4, CLOCK_SYS_TYPE);
			audio_finetune_sample_rate(3);
		#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
			audio_amic_init( DIFF_MODE, 45, 20, R4, CLOCK_SYS_TYPE);
		#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
			audio_amic_init( DIFF_MODE, 65, 15, R6, CLOCK_SYS_TYPE);
		#endif
#endif
    }
	/*
	 * audio input set, ignore the input parameter
	 *
	 */
    audio_amic_input_set(PGA_CH);
#endif  //AMIC/DMIC
}
#elif MCU_CORE_8258 || MCU_CORE_8278
static void proc_mic_encoder_82x8 (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = reg_audio_wptr;
	u16 l = ((mic_wptr) >= buffer_mic_rptr) ? ((mic_wptr) - buffer_mic_rptr) : 0xffff;
	if (l >=(TL_MIC_BUFFER_SIZE>>1)) {
		s16 *ps = buffer_mic + (buffer_mic_rptr>>1);//unit size:s16,so right shift 1 bit
		mic_to_adpcm_split (ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>1);
		buffer_mic_pkt_wptr++;
		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
		}
		if(audio_delay_times>0)
			audio_delay_times--;

	}

}

static void audio_recInit_82x8(u32 sampleRate, audio_rec_ntf audioUserhandler)
{
	g_audioRecNtf = audioUserhandler;
	if(sampleRate==AUDIO_SAMPLE_RATE_8K)
	{
		/* set fifo0 as input */
		audio_config_mic_buf((unsigned short*)buffer_mic,TL_MIC_BUFFER_SIZE);
#if (MCU_CORE_8258)
		audio_amic_init(AUDIO_8K);
#elif  (MCU_CORE_8278)
		audio_set_mute_pga(0);
		audio_amic_init(AUDIO_8K);
		sleep_ms(17);
		audio_set_mute_pga(1);
#endif
	}
	else if(sampleRate==AUDIO_SAMPLE_RATE_16K)
	{
		/* set fifo0 as input */
		audio_config_mic_buf((unsigned short*)buffer_mic,TL_MIC_BUFFER_SIZE);
#if (MCU_CORE_8258)
		audio_amic_init(AUDIO_16K);
#elif  (MCU_CORE_8278)
		audio_set_mute_pga(0);
		audio_amic_init(AUDIO_16K);
		sleep_ms(17);
		audio_set_mute_pga(1);
#endif
	}
}
#elif MCU_CORE_B92
#define AMIC_RX_DMA_CHN         DMA4
#define FIFO_NUM        		FIFO0
audio_codec_stream0_input_t audio_codec_amic_input =
{
	.input_src = AMIC_STREAM0_MONO_L,
	.sample_rate = AUDIO_16K,
	.fifo_num = FIFO_NUM,
	.data_width = CODEC_BIT_16_DATA,
	.dma_num = AMIC_RX_DMA_CHN,
	.data_buf = buffer_mic,
	.data_buf_size = sizeof(buffer_mic),
};


static void proc_mic_encoder_b92 (void)
{
	static u16	buffer_mic_rptr;
	u16 mic_wptr = audio_get_rx_wptr(FIFO_NUM);
	u16 l = ((mic_wptr) >= buffer_mic_rptr) ? ((mic_wptr) - buffer_mic_rptr) : 0xffff;
	if (l >=(TL_MIC_BUFFER_SIZE>>1)) {
		s16 *ps = buffer_mic + (buffer_mic_rptr>>1);//unit size:s16,so right shift 1 bit
		mic_to_adpcm_split (ps,	TL_MIC_ADPCM_UNIT_SIZE,
						(s16 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
						(buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1))), 1);


		u8 *ptr = (u8 *)(buffer_mic_enc + (ADPCM_PACKET_LEN>>2) * (buffer_mic_pkt_wptr & (TL_MIC_PACKET_BUFFER_NUM - 1)));

		buffer_mic_rptr = buffer_mic_rptr ? 0 : (TL_MIC_BUFFER_SIZE>>1);
		buffer_mic_pkt_wptr++;
		int pkts = (buffer_mic_pkt_wptr - buffer_mic_pkt_rptr) & (TL_MIC_PACKET_BUFFER_NUM*2-1);
		if (pkts > TL_MIC_PACKET_BUFFER_NUM) {
			buffer_mic_pkt_rptr++;
		}
		if(audio_delay_times>0)
			audio_delay_times--;

	}

}

static void audio_recInit_b92(u32 sampleRate, audio_rec_ntf audioUserhandler)
{
	g_audioRecNtf = audioUserhandler;
	if(sampleRate==AUDIO_SAMPLE_RATE_16K||sampleRate==AUDIO_SAMPLE_RATE_8K)
	{
	/****audio init ****/
	(sampleRate==AUDIO_SAMPLE_RATE_8K)?(audio_codec_amic_input.sample_rate=AUDIO_8K):(audio_codec_amic_input.sample_rate=AUDIO_16K);
	audio_codec_init();
	audio_codec_stream0_input_init(&audio_codec_amic_input);
	audio_rx_dma_chain_init(audio_codec_amic_input.fifo_num,audio_codec_amic_input.dma_num,(unsigned short*)audio_codec_amic_input.data_buf,audio_codec_amic_input.data_buf_size);
	audio_rx_dma_en(audio_codec_amic_input.dma_num);
	audio_set_adc_pga_l_gain(CODEC_IN_GAIN_24P0_DB);//set voltume
	}
}

#endif



static int	*mic_encoder_data_buffer (void){
	if (buffer_mic_pkt_rptr == buffer_mic_pkt_wptr) {
			return 0;
	}
	u8 value = 0;
	s8 buffer_mic_pkt_diff = (s8)(buffer_mic_pkt_wptr-buffer_mic_pkt_rptr);
	if(buffer_mic_pkt_diff>20)
		value = 1;
	else
		value = 3;

	mac_mlmeSetReq(MAC_MAX_FRAME_RETRIES, &value);
	value = 0;
	mac_mlmeSetReq( MAC_MAX_CSMA_BACKOFFS, &value );
	int *ps = buffer_mic_enc + (ADPCM_PACKET_LEN>>2) *
			(buffer_mic_pkt_rptr & (TL_MIC_PACKET_BUFFER_NUM - 1));

	buffer_mic_pkt_rptr++;
	return ps;
}

/*
 *
 * collection unit: send out the audio data
 *
 * */
static void proc_sending84(void){
	if(audio_delay_times > 0){
		return;
	}
	if(!GetAudioTxState()){
	int *buff_adpcm =  mic_encoder_data_buffer();
    if ( buff_adpcm ) {
        memcpy(&buff_adpcm_temp[APP_AUDIO_HDR_LEN], buff_adpcm, MIC_ADPCM_FRAME_SIZE);
    	if(g_audioRecNtf){
    		g_audioRecNtf(buff_adpcm_temp, MIC_ADPCM_FRAME_SIZE+APP_AUDIO_HDR_LEN);
    		drv_hwTmr_cancel(TIMER_FOR_USER);
    		drv_hwTmr_set(TIMER_FOR_USER, 1000, AudioTimeOutTxCb, NULL);
			}
		}
	}
}

void audio_recInit(u32 sampleRate, audio_rec_ntf audioUserhandler)
{
#if MCU_CORE_826x
	audio_recInit_826x(sampleRate,audioUserhandler);
#elif MCU_CORE_8258 || MCU_CORE_8278
	audio_recInit_82x8(sampleRate,audioUserhandler);
#elif MCU_CORE_B92
	audio_recInit_b92(sampleRate,audioUserhandler);
#endif
}


void audio_recTaskStart(void){
	if(audio_status != AUDIO_OPENED){
		audio_status = AUDIO_OPENED;

		APP_AMIC_PIN_CFG_ON;
		audio_recInit(MIC_SAMPLE_RATE, g_audioRecNtf);

#if MCU_CORE_826x
		BIT_SET(reg_dfifo_ana_in,4); //enable difofo
		BIT_SET(reg_dfifo_ana_in,5); //enable difofo
#elif MCU_CORE_8258
		BIT_SET(reg_dfifo_mode,0); //FLD_AUD_DFIFO0_IN   enable difofo
#elif MCU_CORE_8278
		BIT_SET(reg_dfifo_mode,0); //FLD_AUD_DFIFO0_IN   enable difofo
#elif MCU_CORE_B92

#endif
		audio_delay_times = 20;

	}
}

void audio_recTaskStop(void){
    audio_status = AUDIO_IDLE;
    APP_AMIC_PIN_CFG_OFF;
#if MCU_CORE_826x
    BIT_CLR(reg_dfifo_ana_in,4); //enable difofo
	BIT_CLR(reg_dfifo_ana_in,5); //enable difofo
	AUDIO2ADC();
	drv_adc_battery_detect_init();
#elif MCU_CORE_8258
	BIT_CLR(reg_dfifo_mode,0); 	 //FLD_AUD_DFIFO0_INenable difofo
	drv_adc_battery_detect_init();
#elif  MCU_CORE_8278
	BIT_CLR(reg_dfifo_mode,0); 	 //FLD_AUD_DFIFO0_INenable difofo
	audio_codec_and_pga_disable();
#elif  MCU_CORE_B92
	audio_rx_dma_dis(AMIC_RX_DMA_CHN);
	audio_power_down();
#endif
}


void audio_recTaskRun(void){

#if MCU_CORE_826x
    proc_mic_encoder_826x();
#elif MCU_CORE_8258 || MCU_CORE_8278
    proc_mic_encoder_82x8();
#elif MCU_CORE_B92
    proc_mic_encoder_b92();
#endif

    proc_sending84();
}



u8 audio_recTaskStatusGet(void){
    return audio_status;
}

volatile bool AudioTxDataFlag;
volatile u8   AudioTxCurCnt;



_attribute_ram_code_ void SetAudioTxState(bool flag)
{
	AudioTxDataFlag = flag;
}

_attribute_ram_code_ void SetAudioTxCnt(u8 cnt)
{
	AudioTxCurCnt = cnt;
}


_attribute_ram_code_ bool GetAudioTxState(void)
{
	return AudioTxDataFlag;
}

_attribute_ram_code_ u8 GetAudioTxCnt(void)
{
	return AudioTxCurCnt;
}

extern u8 rf_tx_buf[];
_attribute_ram_code_ int AudioTimeOutTxCb(void* arg)
{
	ZB_RADIO_RX_DONE_CLR;
	ZB_RADIO_TX_DONE_CLR;
	rf_setTrxState(RF_STATE_TX);
	ZB_RADIO_TX_START(rf_tx_buf);//Manual mode

	u8 cnt = GetAudioTxCnt();
	SetAudioTxCnt(cnt+1);
	return -1;
}



#endif
