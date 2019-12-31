/*
 * emi.h
 *
 *  Created on: Feb 14, 2014
 *      Author: xuzhen
 */

#ifndef CMD_TESTMODE_H_
#define CMD_TESTMODE_H_

enum{
	TESTMODE_CMD_SET_TEST_MODE = 0x10,
	TESTMODE_CMD_SET_USER_MODE,
	TESTMODE_CMD_SET_ROOT_MODE,
	TESTMODE_CMD_SET_IR_MODE,
	TESTMODE_CMD_SET_RF_MODE,
	TESTMODE_CMD_SET_SLEEP_MODE = 0x15,
	TESTMODE_CMD_GET_MAC = 0x51,
	TESTMODE_CMD_GET_ADC
}TESTMODE_CMD_ID;

typedef struct{
	u16 hdr;
	u8 len;
	u8 cmd_id;
	u8 crc;
}testmode_cmd_pkt_t;

typedef struct{
	u16 hdr;
	u8 len;
	u8 cmd_id;
	u8 payload[1];
}testmode_resp_pkt_t;

typedef struct{
	u8 cmd_id;
	void (*cmd_cb)(void);
}testmode_handler_t;

void test_mode_init(void);

void test_mode_task(void);

#endif /* EMI_H_ */
