/********************************************************************************************************
 * @file    qsIR_Tx.c
 *
 * @brief   This is the source file for qsIR_Tx.c
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

#include "../../proj/tl_common.h"
#if (RF4CE_ZRC2_ENABLE)
#include "qsIR_Tx.h"
#include "../../proj/drivers/ir/ir.h"

#if 1
// u32 TstState=0;
const u8 volumnUp[]={
		0x00,0x47,0x03,0x01,0x06,0x22,0x4,
		0x00,0x00,0x00,0xD2,0x00,0x90,0x00,0xEB,
		0x5D,0x90,0x00,0x6C,0x26,0xCA,0x08,0x65,
		0x04,0xCA,0x08,0x32,0x02,0x90,0x00,0xA6,
		0x01,0x90,0x00,0x8D,0x00,0x25,0x55,0x54,
		0x54,0x54,0x44,0x45,0x45,0x45,0x45,0x54,
		0x55,0x54,0x54,0x45,0x44,0x41,0x30,0x30,
		0x25,0x54,0x54,0x54,0x44,0x45,0x45,
		0x45,0x45,0x54,0x55,0x54,0x54,0x45,0x44,
		0x41,0x25,0x25,
};

const u8 volumnDown[] = {
		0x0,0x20,0x1,0x1,0x3,0x0,0xc,0x0
		,0x2,0x3,0x0,0x0,0x5,0x0,0x6d,0x3d,0x5,0x0
		,0x60,0x7,0x5,0x0,0xea,0x4,0x22,0x22,0x22
		,0x22,0x12,0x20,0x00,0x21,0x12,0x11
};
#endif



 zrcir_callback_t quickset_handler = NULL;
u8 *quicksetIrCodeData=NULL;
 //zrc to zipir
 u8 ws,zrcMode=0;
 s8 zrcRepeat=0;
 u8 zrcIrState=0;
 volatile zrcIRTimeData zrcIRData;
 volatile zrcIRinfo qsInfo;


 _attribute_ram_code_ u8 zrc_getIrState(void)
 {
	 return zrcIrState;
 }

 _attribute_ram_code_ void zrc_putIrState(u8 state)
 {
	 zrcIrState = state;
 }

 u8 zrc_getIrMode(void)
 {
	 return zrcMode;
 }

 //mode=0:   NORMAL_XMIT
 //mode=1:   MACRO_XMIT
 void zrc_putIrMode(u8 mode)
 {
	 zrcMode = mode;
 }

//0:init ok
//-1:failed
//mode=0:   NORMAL_XMIT
//mode=1:   MACRO_XMIT
s8 zrcIrInit(u8 *database)
{
	zrcIRDataHead *zrcIRptr = (zrcIRDataHead *)database;
	s8 k = SUCCESS;
	u32 m = 0;
//	u8 singleflag = 0;
	u8 ToggleFlag=0;
//	if(zrcIrGetState()){
	if(app_isSendingIR()){
		return INVALID_TASK;
	}
	ws = 0;
	quicksetIrCodeData = NULL;
	memset((u8 *)&zrcIRData,0,sizeof(zrcIRData));
	if(!((zrcIRptr->irconfig)&IRVenSpec))//init carrier
	{
		if(zrcIRptr->conFlag&IRCarrier)//
		{
			m = IR_8MHZ/zrcIRptr->carrierPeriod;
			qsInfo.carrierFreq = m;
			ir_set(m,3);//30% duty
#if !IR_DMA_FIFO_EN
			SPACE(1);
#endif
		}
		else
		{
			qsInfo.carrierFreq = 100000;
			ir_set(100000,1);
#if !IR_DMA_FIFO_EN
			SPACE(1);
#endif
		}

	}

	u8 *frameptr;

    frameptr = &zrcIRptr->irData;

	 if(zrcIRptr->conFlag&0x1)
	{
		zrcRepeat = -1;//always repeat
	}
	else if((zrcIRptr->conFlag)&0xf00)
	{
		zrcRepeat = (((zrcIRptr->conFlag)&0xf00)>>8);
	}

	 quicksetIrCodeData = ev_buf_allocate(LARGE_BUFFER);
	 if((quicksetIrCodeData!=NULL)&&(zrcIRptr->codeLength!=0))
	 {
		 if((2+zrcIRptr->codeLength)<=LARGE_BUFFER)
			 memcpy((u8 *)quicksetIrCodeData,(u8 *)zrcIRptr, 2+zrcIRptr->codeLength);
		 else
			 return INVALID_TASK;
	 }
	 else
		 return INVALID_TASK;

	if(zrcIRptr->numPress)//start
		ws |= IRSTART;

	if(zrcIRptr->numRepeat)//repeat
	{
		ws |= IRREPEAT;
		if(zrcIRptr->numToggle)//toggle
		{
			ToggleFlag = ReadAnalogReg(Toggle_Reg);
			   ToggleFlag++;
			if(ToggleFlag>zrcIRptr->numToggleSequ)
			   ToggleFlag = 0;
			WriteAnalogReg(Toggle_Reg,ToggleFlag);
		}
	}

	if(zrcIRptr->numRelease)//release
		ws |= IRRELEASE;

	if(ws&(IRSTART|IRREPEAT|IRRELEASE))
	{
		zrc_putIrState(1);
		app_setState(APP_SENDING_IR_STATE);
		if(zrcMode==NORMAL_XMIT)
		{
			app_startSendRepeateDataTimer(108000);//30s
		}
		else
		{
			zrcRepeat = zrcMode;
			app_startSendRepeateDataTimer(108000);//30s
		}

		zrcMode = 0;
	}
	else
		zrc_putIrState(0);

	if(ws&IRSTART)
	{
		ws &=~IRSTART;
		qs_getIrData(IRSTART);
#if IR_DMA_FIFO_EN
		unsigned char* buff = (u8 *)&zrcIRData.Size;
		pwm_set_dma_address(buff);
		reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
		reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
		pwm_start_dma_ir_sending();
		zrcDMAIrcallback(qsIrqCallBack);
#else
		qsInfo.curCnt = 0;
		qsInfo.totalCnt = zrcIRData.Size;
		qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
		hwTmr_set(TIMER_IDX_1, 1000, qsTimer1IrqCb, NULL);
#endif
	}
	else if(ws&IRREPEAT)
	{
		qs_getIrData(IRREPEAT);
#if IR_DMA_FIFO_EN
		unsigned char* buff = (u8 *)&zrcIRData.Size;
		pwm_set_dma_address(buff);
		reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
		reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
		pwm_start_dma_ir_sending();
		zrcDMAIrcallback(qsIrqCallBack);
#else
		qsInfo.curCnt = 0;
		qsInfo.totalCnt = zrcIRData.Size;
		qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
		hwTmr_set(TIMER_IDX_1, 1000, qsTimer1IrqCb, NULL);
#endif
		if(zrcRepeat>0)zrcRepeat--;
	}
	else if(ws&IRRELEASE)
	{
		ws &= ~IRRELEASE;
		qs_getIrData(IRRELEASE);
#if IR_DMA_FIFO_EN
		unsigned char* buff = (u8 *)&zrcIRData.Size;
		pwm_set_dma_address(buff);
		reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
		reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
		pwm_start_dma_ir_sending();
		zrcDMAIrcallback(qsIrqCallBack);
#else
		qsInfo.curCnt = 0;
		qsInfo.totalCnt = zrcIRData.Size;
		qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
		hwTmr_set(TIMER_IDX_1, 1000, qsTimer1IrqCb, NULL);
#endif
	}
	return k;
}

s8 qs_getIrData(u8 frametype)
{
	s8 k = SUCCESS;
	u8 *frameptr,*toggleptr;
	u16 Timing[16][2]={{0},{0}};//max size
	u16 irdatasize=0,loopidx=0;
	u8 togflag=0,togstart=0,tognum=0,togidx=0,togdata=0;
	u8 data=0;
#if IR_DMA_FIFO_EN
	u32 carrier_cycle_tick = 0;
#else
	u16 irdata=0;
#endif
	toggleptr = NULL;
	frameptr = NULL;
	if(quicksetIrCodeData!=NULL)
	{
//		memset((u8 *)&zrcIRData,0,sizeof(zrcIRData));
		zrcIRDataHead *qsIRptr = (zrcIRDataHead *)quicksetIrCodeData;
		if(frametype&(IRSTART|IRREPEAT|IRRELEASE))
		{
			frameptr = &qsIRptr->irData;
#if IR_DMA_FIFO_EN
			if(qsInfo.carrierFreq)
			carrier_cycle_tick = CLOCK_SYS_CLOCK_HZ/qsInfo.carrierFreq;
#endif
			if(qsIRptr->numTiming)
			{
				for(u8 i=0;i<qsIRptr->numTiming;i++)
				{
					Timing[i][0] = (*(u16 *)(frameptr + i*4));//mark
					Timing[i][1] = (*(u16 *)(frameptr + i*4+2));//space
#if IR_DMA_FIFO_EN
					if(Timing[i][0])
					Timing[i][0] = pwm_config_dma_fifo_waveform(1, PWM0_PULSE_NORMAL, Timing[i][0] * H_TIMER_CLOCK_1US*4/carrier_cycle_tick);//mark
					if(Timing[i][1])
					Timing[i][1] = pwm_config_dma_fifo_waveform(0, PWM0_PULSE_NORMAL, Timing[i][1] * H_TIMER_CLOCK_1US*4/carrier_cycle_tick);//space
#endif
				}
			}
					frameptr += qsIRptr->numTiming*4;
			if((qsIRptr->numPress)&&(frametype==IRSTART))
			{
				loopidx = qsIRptr->numPress;
			}
			else if((qsIRptr->numRepeat)&&(frametype==IRREPEAT))
			{
				loopidx = qsIRptr->numRepeat;
				frameptr += (qsIRptr->numPress>>1)+(qsIRptr->numPress&0x1);
				if(qsIRptr->numToggle)
				{
					togflag = ReadAnalogReg(Toggle_Reg);
					if(togflag)
					{
					toggleptr = frameptr +(qsIRptr->numRepeat>>1)+(qsIRptr->numRepeat&0x1);
					toggleptr += (qsIRptr->numRelease>>1)+(qsIRptr->numRelease&0x1);
					togstart = *(u8 *)(toggleptr++);
					toggleptr += (togflag-1)*((qsIRptr->numToggle>>1)+(qsIRptr->numToggle&1));
					tognum = qsIRptr->numToggle;
					}
				}
			}
			else if((qsIRptr->numRelease)&&(frametype==IRRELEASE))
			{
				loopidx = qsIRptr->numRelease;
				frameptr += (qsIRptr->numPress>>1)+(qsIRptr->numPress&0x1);
				frameptr += (qsIRptr->numRepeat>>1)+(qsIRptr->numRepeat&0x1);
			}
			else
				k = INVALID_TASK;


			if(k == SUCCESS)
			{
				for(u8 i=0;i<loopidx;i++)
				{
					if((i%2)==0)
					{
						data = *(u8 *)frameptr;
						frameptr += 1;
					}

					if(togflag)
					{
						if((i>=togstart)&&(i<(togstart+tognum)))
						{
							if((togidx%2)==0)
							{
								togdata = *(u8 *)toggleptr;
								toggleptr += 1;
							}
							data &=0x0f;
							data |= togdata&0xf0;
							togdata<<=4;
							togidx++;
						}
					}


					if(Timing[(data&0xf0)>>4][0])
					{
#if IR_DMA_FIFO_EN
						zrcIRData.Symbol[irdatasize++] = Timing[(data&0xf0)>>4][0];
#else
						irdata = Timing[(data&0xf0)>>4][0];
						while(irdata>0x7fff)
						{
							zrcIRData.Symbol[irdatasize++] = 0x7fff|ZRCIR_HIGH;
							irdata -= 0x7fff;
						}
							zrcIRData.Symbol[irdatasize++] = irdata|ZRCIR_HIGH;
#endif
					}
					if(Timing[(data&0xf0)>>4][1])
					{
#if IR_DMA_FIFO_EN
						zrcIRData.Symbol[irdatasize++] = Timing[(data&0xf0)>>4][1];
#else
						irdata = Timing[(data&0xf0)>>4][1];
						while(irdata>0x7fff)
						{
							zrcIRData.Symbol[irdatasize++] = 0x7fff;
							irdata -= 0x7fff;
						}
							zrcIRData.Symbol[irdatasize++] = irdata;
#endif
					}

						data<<=4;
				}
#if IR_DMA_FIFO_EN
				irdatasize <<= 1;
#endif
				zrcIRData.Size = irdatasize;
			}
		}
		else
			k = INVALID_TASK;
	}
	else
		k = INVALID_TASK;
		return k;
}


s32 app_zrcIrDoneCb(void *arg){
	if(quickset_handler){
		quickset_handler();
	}
	return -1;
}




void zrcStop(void)
{
	if(zrcRepeat>0) return;
	ws&=~IRREPEAT;
}

void zrcStopIR(void)
{
	ws&=~IRREPEAT;
	zrc_putIrState(0);
}


void zrctxcallback_end(zrcir_callback_t qs_cb)
{
	 quickset_handler = qs_cb;
}




#if IR_DMA_FIFO_EN
_attribute_ram_code_ void qsIrqCallBack(void)
{
	if((ws&(IRSTART|IRREPEAT|IRRELEASE))==0)
	{
		pwm_stop_dma_ir_sending();
		reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear irq status
		reg_pwm_irq_mask &= ~FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //disable irq mask
		zrc_putIrState(0);
		ev_on_timer(app_zrcIrDoneCb, NULL, 20* 1000);
		ev_buf_free((u8 *)quicksetIrCodeData);
		return;
	}

	if(ws&IRREPEAT)
	{
		if(zrcRepeat!=0)
		{
			qs_getIrData(IRREPEAT);
			pwm_start_dma_ir_sending();
			if(zrcRepeat>0)
			{
				zrcRepeat--;
				if(zrcRepeat==0)
					ws &=~IRREPEAT;
			}
		}
		else
			ws &=~IRREPEAT;
	}
	else
	{
		if(ws&IRRELEASE)
		{
			qs_getIrData(IRRELEASE);
			pwm_start_dma_ir_sending();
			ws&=~IRRELEASE;
		}
	}
	return;
}
#else
_attribute_ram_code_ int qsTimer1IrqCb(void *arg){
	unsigned int t_flag=0,t_us=0;
	 SPACE(0);
    if(qsInfo.curCnt == qsInfo.totalCnt)
	{
//    	    SPACE(0);
		if((ws&(IRSTART|IRREPEAT|IRRELEASE))==0)
		{
			hwTmr_cancel(TIMER_IDX_1);
			zrc_putIrState(0);
			ev_on_timer(app_zrcIrDoneCb, NULL, 20* 1000);
			ev_buf_free((u8 *)quicksetIrCodeData);
			return -1;
		}
		//code
		if(ws&IRREPEAT)
		{
			if(zrcRepeat!=0)
			{
				qs_getIrData(IRREPEAT);
				qsInfo.curCnt = 0;
				qsInfo.totalCnt = zrcIRData.Size;
				qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];

				if(zrcRepeat>0)
				{
					zrcRepeat--;
					if(zrcRepeat==0)
						ws &=~IRREPEAT;
				}
			}
			else
				ws &=~IRREPEAT;
		}
		else
		{
			if(ws&IRRELEASE)
			{
				qs_getIrData(IRRELEASE);
				ws&=~IRRELEASE;
				qsInfo.curCnt = 0;
				qsInfo.totalCnt = zrcIRData.Size;
				qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
			}
		}
	}
    t_flag = qsInfo.zrcIRdata[qsInfo.curCnt]&ZRCIR_HIGH;
    t_us = qsInfo.zrcIRdata[qsInfo.curCnt]&(~ZRCIR_HIGH);
    t_us<<=2;//symbol*4
    if(t_us>4)
    t_us-=4;
	hwTmr_setInterval(TIMER_IDX_1, t_us);
	qsInfo.curCnt += 1;
	(t_flag)?(MARK(t_us)):(SPACE(t_us));
	return 0;
}
#endif
#endif
