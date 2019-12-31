#pragma once
  
typedef struct{
	u32 onInterval;
	u32 offIntervalList[5];
	u8  modeNum;
}app_ledFlushInfo_t;

int zrc_ledBlinkTimer(void *arg);
int zrc_pairDoneTimerCb(void *arg); 
void zrc_startLed(u8 blinkTime, u32 blinkInterval, u8 ledIndex);
void zrc_startLedSpecial(u8 blinkTime, app_ledFlushInfo_t *ledAction, u8 ledIndex);
void zrc_stopLed(u8 ledIndex); 
void zrc_startConfigDoneLed(void);
void zrc_ledOff(u8 ledIndex);
void zrc_ledOn(u8 ledIndex);