/*
 * zrcIR_Tx.h
 *
 *  Created on: 2019-4-15
 *      Author: telink
 */

#ifndef QSIR_TX_H_
#define QSIR_TX_H_

#include "../../proj/tl_common.h"
#include "app_config.h"
#include "board_cfg.h"
#include "zrcApp.h"
#include "../../proj/os/timer.h"


typedef struct {
	u8 		irconfig;
	u8 		codeLength;
	u16 	conFlag;
	u8		numTiming;
	u8		numPress;
	u8		numRepeat;
	u8		numRelease;
	u8		numToggle;
	u8		numToggleSequ;
	u16		carrierPeriod;
	u8		irData;
}zrcIRDataHead;

//#define MaxSymbol   16
//#define MaxFrame    16

#define maxframe      180//380




typedef struct {
	u32      Size;
	u16		 Symbol[maxframe];
}zrcIRTimeData;



//typedef struct {
//	zrcIRTimeData	pressTime;
//	zrcIRTimeData	repeatTime;
//	zrcIRTimeData	releaseTime;
//}zrcIRFrame;

typedef struct {
	u32	carrierFreq;
	u16	curCnt;
	u16	totalCnt;
	u16 *zrcIRdata;
}zrcIRinfo;


#define IRVenSpec  BIT(0)

#define IRCarrier  BIT(1)


#define MarkSpace  BIT(2)

#define IR_8MHZ  8000000

#define IR_DUTY  30

#define IRSTART    1
#define IRREPEAT   2
#define IRRELEASE  4



//symbolunit:4us
#define 			symbolunit  				4

#define 			ZRCIR_HIGH 					0x8000  // Bit15 = 1: bit 15 set to one to
														// indicate logic high in data
														// duration
#define 			ZRCIR_LOW 					0x0000  // Bit15 = 0: bit 15 set to zero to

#define             Toggle_Reg					0x35
#define 			MAX_QS_DURATION_INTERVAL 	600000  // 10 minutes
#define 			MAX_IRTX_SIZE 				400

#define 			InByPassTimeUs				1500  // 1500us

#define 			NORMAL_XMIT 				0x00
#define 			MACRO_XMIT 					0x01

typedef void (*zrcir_callback_t)(void);


extern void ir_set_chn(int hz, int low_duty);
extern void zrc_startSendRepeateDataTimer(u32 rptTime);



s8 zrcIrInit(u8 *database);
void zrcIrServiceLoop(u8 *database);
u16 zrcMappingState(u16 *buf, u8 frameType);
u16 zrcDataSignal(u16 *bufout,  u32 *inbuf, u16 idxin);
u32 zrcTimeToPluseNum(u32 timeus);
void zrcIrPutState(u8 state);
u8 zrcIrGetState(void);
void zrctxcallback_end( zrcir_callback_t qs_cb);
s8 qs_getIrData(u8 frametype);

u8 zrcIrGetMode(void);
//mode=0:   NORMAL_XMIT
//mode=1:   MACRO_XMIT
void zrcIrPutMode(u8 mode);
void zrcStopIR(void);
int qsTimer1IrqCb(void *arg);
u8 zrcIrGetState(void);
void zrcStop(void);
void qsIrqCallBack(void);

//zipir interface
extern u8 ir_pwm;






extern u16* getdata1addr(void);
extern u16* getdata2addr(void);
extern u16* getptraddr(void);
extern void sendirdata(u16 ir_size);
extern void zrcStopTimer(void);
extern void zipir_send_release(void);
extern void zrcIRServiceLoop(void);

#endif /* ZRCIR_TX_H_ */
