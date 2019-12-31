/*
 * zrcIR_Tx.c
 *
 *  Created on: 2019-4-15
 *      Author: telink
 */
#include "qsIR_Tx.h"

#if 1
// u32 TstState=0;
//const u8 volumnUp[]={
//		0x00,0x47,0x03,0x01,0x06,0x22,0x4,
//		0x00,0x00,0x00,0xD2,0x00,  0x90,0x00,0xEB,
//		0x5D,0x90,0x00,0x6C,0x26,0xCA,0x08,0x65,
//		0x04,0xCA,0x08,0x32,0x02,0x90,0x00,0xA6,
//		0x01,0x90,0x00,0x8D,0x00,0x25,0x55,0x54,
//		0x54,0x54,0x44,0x45,0x45,0x45,0x45,0x54,
//		0x55,0x54,0x54,0x45,0x44,0x41,0x30,0x30,
//		0x25,0x54,0x54,0x54,0x44,0x45,0x45,
//		0x45,0x45,0x54,0x55,0x54,0x54,0x45,0x44,
//		0x41,0x25,0x25,
//};
//
//const u8 volumnDown[]={
//		0x0,0x20,0x1,0x1,0x3,0x0,0xc,0x0
//		,0x2,0x3,0x0,0x0,0x5,0x0,0x6d,0x3d,0x5,0x0
//		,0x60,0x7,0x5,0x0,0xea,0x4,0x22,0x22,0x22
//		,0x22,0x12,0x20,0x00,0x21,0x12,0x11};
const u8 volumnDown[]={
	0x00,0x23,0x03,0x02,0x04,0x00,0x11,0x00,
	0x00,0x00,0xC8,0x00,0x7D,0x00,0xA1,0x77,
	0x7D,0x00,0xFB,0x04,0x7D,0x00,0x01,0x04,
	0x7D,0x00,0x7D,0x00,0x32,0x13,0x23,0x21,
	0x13,0x23,0x21,0x13,0x00,
};
const u8 volumnUp[]={
		0x00,0x23,0x03,0x02,0x04,0x00,0x11,0x00,
		0x00,0x00,0xC8,0x00,0x7D,0x00,0xA1,0x77,
		0x7D,0x00,0xFB,0x04,0x7D,0x00,0x01,0x04,
		0x7D,0x00,0x7D,0x00,0x32,0x13,0x23,0x21,
		0x13,0x23,0x21,0x13,0x00,
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


 _attribute_ram_code_ u8 zrcIrGetState(void)
 {
	 return zrcIrState;
 }

 _attribute_ram_code_ void zrcIrPutState(u8 state)
 {
	 zrcIrState = state;
 }

 u8 zrcIrGetMode(void)
 {
	 return zrcMode;
 }

 //mode=0:   NORMAL_XMIT
 //mode=1:   MACRO_XMIT
 void zrcIrPutMode(u8 mode)
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
//			TstState = 1;
		if(zrcIRptr->conFlag&IRCarrier)//
		{
//			TstState = 2;
			m = IR_8MHZ/zrcIRptr->carrierPeriod;
			ir_set(m,3);//30% duty
			SPACE(1);
		}
		else
		{
			ir_set(38000,1);
			SPACE(1);
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
		zrcIrPutState(1);
		if(zrcMode==NORMAL_XMIT)
		{
			zrc_startSendRepeateDataTimer(108000);//30s
		}
		else
		{
			zrcRepeat = zrcMode;
			zrc_startSendRepeateDataTimer(108000);//30s
		}

		zrcMode = 0;
	}
	else
		zrcIrPutState(0);

	if(ws&IRSTART)
	{
		ws &=~IRSTART;
		qs_getIrData(IRSTART);
		qsInfo.curCnt = 0;
		qsInfo.totalCnt = zrcIRData.Size;
		qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
		hwTmr_set(TIMER_IDX_1, 1000 * tickPerUs, qsTimer1IrqCb, NULL);
	}
	else if(ws&IRREPEAT)
	{
		qs_getIrData(IRREPEAT);
		qsInfo.curCnt = 0;
		qsInfo.totalCnt = zrcIRData.Size;
		qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
		hwTmr_set(TIMER_IDX_1, 1000 * tickPerUs, qsTimer1IrqCb, NULL);
		if(zrcRepeat>0)zrcRepeat--;
	}
	else if(ws&IRRELEASE)
	{
		ws &= ~IRRELEASE;
		qs_getIrData(IRRELEASE);
		qsInfo.curCnt = 0;
		qsInfo.totalCnt = zrcIRData.Size;
		qsInfo.zrcIRdata = (u16 *)&zrcIRData.Symbol[0];
		hwTmr_set(TIMER_IDX_1, 1000 * tickPerUs, qsTimer1IrqCb, NULL);
	}
	return k;
}

s8 qs_getIrData(u8 frametype)
{
	s8 k = SUCCESS;
	u8 *frameptr,*toggleptr;
	u16 Timing[16][2]={{0},{0}};//max size
	u16 irdatasize=0,irdata=0,loopidx=0;
	u8 togflag=0,togstart=0,tognum=0,togidx=0,togdata=0;
	u8 data=0;
	toggleptr = NULL;
	frameptr = NULL;
	if(quicksetIrCodeData!=NULL)
	{
//		memset((u8 *)&zrcIRData,0,sizeof(zrcIRData));
		zrcIRDataHead *qsIRptr = (zrcIRDataHead *)quicksetIrCodeData;
		if(frametype&(IRSTART|IRREPEAT|IRRELEASE))
		{
			frameptr = &qsIRptr->irData;
			if(qsIRptr->numTiming)
			{
				for(u8 i=0;i<qsIRptr->numTiming;i++)
				{
					Timing[i][0] = (*(u16 *)(frameptr+ i*4));//mark
					Timing[i][1] = (*(u16 *)(frameptr + i*4+2));//space
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
						irdata = Timing[(data&0xf0)>>4][0];
						while(irdata>0x7fff)
						{
							zrcIRData.Symbol[irdatasize++] = 0x7fff|ZRCIR_HIGH;
							irdata -= 0x7fff;
						}
							zrcIRData.Symbol[irdatasize++] = irdata|ZRCIR_HIGH;
					}
					if(Timing[(data&0xf0)>>4][1])
					{

						irdata = Timing[(data&0xf0)>>4][1];
						while(irdata>0x7fff)
						{
							zrcIRData.Symbol[irdatasize++] = 0x7fff;
							irdata -= 0x7fff;
						}
							zrcIRData.Symbol[irdatasize++] = irdata;
					}
						data<<=4;
				}
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
	zrcIrPutState(0);
}


void zrctxcallback_end(zrcir_callback_t qs_cb)
{
	 quickset_handler = qs_cb;
}






_attribute_ram_code_ int qsTimer1IrqCb(void *arg){
	unsigned int t_flag=0,t_us=0;
	 SPACE(0);
    if(qsInfo.curCnt == qsInfo.totalCnt)
	{
//    	    SPACE(0);
		if((ws&(IRSTART|IRREPEAT|IRRELEASE))==0)
		{
			hwTmr_cancel(TIMER_IDX_1);
			zrcIrPutState(0);
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
    t_us<<=5;//clock run on 32M
    t_us-=160;
	hwTmr_setInterval(TIMER_IDX_1, t_us);
	qsInfo.curCnt += 1;
	(t_flag)?(MARK(t_us)):(SPACE(t_us));
	return 0;
}

