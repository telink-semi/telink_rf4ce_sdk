
#pragma once

int zrc_doPair(void *arg);
s32 zrc_doUnpair(void *arg);
int zrc_waitForNextKeyTimerCb(void *arg); 
void zrc_startWaitForNextKeyTimer(void); 
void zrc_stopWaitForNextKeyTimer(void);
int zrc_verifyTVBrand(u16 TVcode); 
void zrc_handleKeyPressed(void *arg);
void zrc_handleKeyReleased(void *arg); 

void zrc_restoreAppState(void);
void zrc_lightOnProgLed(void);
int zrc_ledShowVerInfo(void *arg);
void zrc_startSendCarrier(void *arg);
void zrc_sendCarrierOnNextCh(void);
void zrc_ckValidResp(u8 pairingRef, u8 status);
u8 ota_cmdCallback(u8 id, u8 status);
void zrc_numberRecall(void *arg);
int zrc_delayAllowDeep(void *arg);
