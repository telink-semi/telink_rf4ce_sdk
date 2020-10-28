/********************************************************************************************************
 * @file     audio_rec_proc.c
 *
 * @brief	 audio input configuration(AMIC/DMIC are supported), and then encode to ADPCM format,
 *
 * 			 after that sent out using RF
 *
 * @author
 * @date     July. 20, 2019
 *
 * @par      Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/


#include "drv_audio.h"
#include "audio_codec_adpcm.h"
#include "../drv_adc.h"
#include "../../os/timer.h"
#include "../../../net/rf4ce/mac/mac_phy.h"
#include "../../../net/rf4ce/rf4ce_includes.h"
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
		audio_amic_init(AUDIO_8K);
	}
	else if(sampleRate==AUDIO_SAMPLE_RATE_16K)
	{
		/* set fifo0 as input */
		audio_config_mic_buf((unsigned short*)buffer_mic,TL_MIC_BUFFER_SIZE);
		audio_amic_init(AUDIO_16K);
	}
#if MCU_CORE_8278
	audio_codec_and_pga_disable();
#endif

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
		set_pga_input_vol();
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
#elif MCU_CORE_8258
	BIT_CLR(reg_dfifo_mode,0); 	 //FLD_AUD_DFIFO0_INenable difofo
#elif  MCU_CORE_8278
	BIT_CLR(reg_dfifo_mode,0); 	 //FLD_AUD_DFIFO0_INenable difofo
	audio_codec_and_pga_disable();
#endif
	drv_adc_battery_detect_init();
}


void audio_recTaskRun(void){

#if MCU_CORE_826x
    proc_mic_encoder_826x();
#elif MCU_CORE_8258 || MCU_CORE_8278
    proc_mic_encoder_82x8();
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

extern u32 T_Debug_Rx_Time[];
_attribute_ram_code_ int AudioTimeOutTxCb(void* arg)
{
	clock_enable_clock(TIMER_FOR_USER, 0);
	ZB_RADIO_RX_DONE_CLR;
	ZB_RADIO_TX_DONE_CLR;
	rf_setTrxState(RF_STATE_TX);
	ZB_RADIO_TX_START(rf_tx_buf);//Manual mode

	u8 cnt = GetAudioTxCnt();
	SetAudioTxCnt(cnt+1);
	return -1;
}




#endif
