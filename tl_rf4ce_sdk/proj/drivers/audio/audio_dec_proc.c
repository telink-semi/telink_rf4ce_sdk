/********************************************************************************************************
 * @file    audio_dec_proc.c
 *
 * @brief   This is the source file for audio_dec_proc.c
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

#define	MIC_SHORT_FRAME_SIZE    10
#define PACK_POINTER    (abuf_dec_rptr/248) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16)

#define USB_ISO_IN_SIZE (MIC_SAMPLE_RATE / 1000)
#define DEC_BUFFER_SIZE	(MIC_SHORT_DEC_SIZE<<2) // app_config.h 248

/////////////////////////////////////////////////////////
//	abuf_mic:
//		abuf_mic_wptr: step by 2
//		abuf_dec_wptr: step by 8	(abuf_mic_rptr)
/////////////////////////////////////////////////////////
static u8  abuf_mic_wptr=0;
static u8  abuf_dec_wptr=0;
static u16 abuf_dec_rptr;
static int abuf_reset = 0;


u8  abuf_mic[MIC_ADPCM_FRAME_SIZE * 4];     // adpcm buffer
s16 abuf_dec[MIC_SHORT_DEC_SIZE << 2]; 		// pcm buffer after adpcm decoded

/*
 * audio buffer initialization
 *
 * */
void abuf_init(void){
	abuf_mic_wptr = abuf_dec_wptr = 0;
	abuf_reset = 16;
}

/*
 * add adpcm data to abuf_mic
 *
 * */
void abuf_mic_add (u32 *p){
	u32 *pd = (u32 *) (abuf_mic + (abuf_mic_wptr & 3) * MIC_ADPCM_FRAME_SIZE);
	for (int i=0; i<(MIC_ADPCM_FRAME_SIZE>>2); i++){
		*pd ++ = *p++;
	}
	abuf_mic_wptr ++;
}

/*
 * convert the data from adpcm to pcm
 *
 * */
void abuf_mic_dec (void){
	static int start = 1;
	static int abuf_reset_no;

    if (abuf_reset){
		abuf_dec_wptr = abuf_mic_wptr;
	}else{
		u8 num_mic = abuf_mic_wptr - abuf_dec_wptr;
		u8 num_dec = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);

		if (num_mic > 4){ 			// in case of overflow
			//log_data (TR_24_abuf_dec_rptr, PACK_POINTER);
			abuf_dec_wptr ++;
		}

		if (num_dec > 4){
			//log_data (TR_24_abuf_dec_wptr, (abuf_dec_rptr>>8) | (abuf_dec_wptr<<8) | (abuf_mic_wptr<<16));
			abuf_reset = 16;
			start = 1;
			abuf_reset_no++;
		}else if ( ((!start && num_mic>=1) || (start && num_mic>=2)) && (num_dec <= 3) ){
			adpcm_to_pcm (
					(s16 *) (abuf_mic + (abuf_dec_wptr & 3) * MIC_ADPCM_FRAME_SIZE),
					abuf_dec + (abuf_dec_wptr & 3) * MIC_SHORT_DEC_SIZE,
					MIC_SHORT_DEC_SIZE );
			abuf_dec_wptr ++;			// 256-byte = 128-s16
			start = 0;
		}
	}
}

void abuf_dec_usb (void){
	static u32 tick_usb_iso_in;
	static u8  buffer_empty = 1;
	static u8  n_usb_iso = 0;

	n_usb_iso++;

	if (clock_time_exceed (tick_usb_iso_in, 4000))
	{
		abuf_reset = 16;
	}

	tick_usb_iso_in = clock_time ();
	if (abuf_reset)
	{
		abuf_dec_rptr = abuf_dec_wptr*MIC_SHORT_DEC_SIZE;
		abuf_reset--;
	}
	/////////////////// copy data to usb iso in buffer ///////////////
	reg_usb_ep7_ptr = 0;
	u8 num = abuf_dec_wptr - (abuf_dec_rptr/MIC_SHORT_DEC_SIZE);
	if (num)
	{
		if ((buffer_empty && num >= 3) || (!buffer_empty && (num >= 1 || (n_usb_iso & 3))))
		{
			buffer_empty = 0;

			u16 offset = abuf_dec_rptr%DEC_BUFFER_SIZE;
			s16 *ps = abuf_dec + offset;

            if(offset == DEC_BUFFER_SIZE - (USB_ISO_IN_SIZE/2)){
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
				ps = abuf_dec;
				for (int i=0; i<(USB_ISO_IN_SIZE/2); i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8;
				}
			}
			else{
				for (int i=0; i<USB_ISO_IN_SIZE; i++)
				{
					reg_usb_ep7_dat = *ps;
					reg_usb_ep7_dat = *ps++ >> 8; // why so much noises..
				}
			}
			abuf_dec_rptr += USB_ISO_IN_SIZE;
			if(abuf_dec_rptr >= (MIC_SHORT_DEC_SIZE<<8) ){
				abuf_dec_rptr = 0;
			}
		}
		else {}
	}
	else
	{

#if 0
		//Fill 0
		for (int i=0; i<USB_ISO_IN_SIZE * 2; i++)
		{
			reg_usb_ep7_dat = 0;
		}
#endif
		buffer_empty = 1;
	}
	reg_usb_ep7_ctrl = BIT(0);			//ACK iso in
}


void audio_decInit(void){
#if MCU_CORE_826x
	//////////////// config USB ISO IN/OUT interrupt /////////////////
	reg_usb_mask = BITS(6, 7);			//audio in/out interrupt enable
	reg_irq_mask |= FLD_IRQ_EP_DATA_EN;
	reg_usb_ep6_buf_addr = 0x80;
	reg_usb_ep7_buf_addr = 0xe0;
	reg_usb_ep_max_size = (256 >> 3);
#elif MCU_CORE_8258 || MCU_CORE_8278
	reg_usb_mask = BITS(6, 7);			//audio in/out interrupt enable
	reg_irq_mask |= FLD_IRQ_EP_DATA_EN;
	reg_usb_ep6_buf_addr = 0x80;
	reg_usb_ep7_buf_addr  = 0xa0;//amic 0xa0 dmic 0xc0
	reg_usb_ep_max_size = 256>>2;	//amic 24 dmic 256>>2
	#if MCU_CORE_8258
		rng_init();
	#elif MCU_CORE_8278
		random_generator_init();
	#endif
#endif
}

#if AUDIO_LOCAL_DEBUG
void audio_debug_usb_direct_out(u32 tick_usb_enum){
    int enc_data_ready =0;

    usb_handle_irq();
    if (!clock_time_exceed (tick_usb_enum, 7*1000*1000)) {return;}

    enc_data_ready = mic_encoder_data_ready ((int *)buff_adpcm);   //encode buffer_mic_enc data to buff_adpcm
    if (enc_data_ready) {
        abuf_mic_add ((u32 *)buff_adpcm);                   //move data from buff_adpcm to abuf_mic
    }
    proc_mic_encoder();                                     //buffer_mic --> buffer_mic_enc
    abuf_mic_dec ();
}
#endif

