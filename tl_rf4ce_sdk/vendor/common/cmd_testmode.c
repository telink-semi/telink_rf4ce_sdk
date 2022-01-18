/********************************************************************************************************
 * @file    cmd_testmode.c
 *
 * @brief   This is the source file for cmd_testmode.c
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

/*
 * emi.c
 */
#include "../../proj/tl_common.h"

#if MODULE_UART_ENABLE && RC_TEST_MODE_ENABLE
/**********************************************************************
 * INCLUDES
 */
#include "../../platform/platform_includes.h"
#include "../../net/rf4ce/rf4ce_includes.h"
#include "./cmd_testmode.h"
#include "../../vendor/mso_rc_app/msoApp.h"

#define CMD_HEADER_FLAG		0x00FE
#define RESP_HEADER_FLAG	0x00FE
#define CMD_LENGTH			1
#define OVERHEAD_LEN		4

static void testmode_set(void);
static void usermode_set(void);
static void rootmode_set(void);
static void irmode_set(void);
static void rfmode_set(void);
static void sleepmode_set(void);
static void mac_get(void);
static void adc_get(void);

u8 uart_rx_buf[32] = {0};
u8 uart_tx_buf[32] = {RESP_HEADER_FLAG & 0xff,
					  (RESP_HEADER_FLAG >> 8) & 0xff};

#define TEST_CMD_RESP_SUCC		0xF0
#define TEST_CMD_RESP_FAIL		0xF1
#define MSO_UART_SEND_TIMEOUT	50*1000  //10ms

static testmode_handler_t command_callback[8] = {
	{TESTMODE_CMD_SET_TEST_MODE, 	testmode_set},
	{TESTMODE_CMD_SET_USER_MODE, 	usermode_set},
	{TESTMODE_CMD_SET_IR_MODE,   	irmode_set},
	{TESTMODE_CMD_SET_RF_MODE,   	rfmode_set},
	{TESTMODE_CMD_SET_SLEEP_MODE,  	sleepmode_set},
	{TESTMODE_CMD_GET_MAC ,   		mac_get},
	{TESTMODE_CMD_GET_ADC ,   		adc_get},
	{TESTMODE_CMD_SET_ROOT_MODE, 	rootmode_set},
};

unsigned char *uartRecvBuf = NULL;

volatile u8 T_recvCmdId = 0;
volatile u8 T_testModeCmd = 0;

static u8 checksum_crc8(u8 *ptr, s32 len) {
	u32 cs = 0;
	for(s32 i = 0; i < len; i++){
		cs += *ptr++;
	}
	return(cs & 0xff);
}


static void uart_resp_send(u8 cmd, u8 *data, u8 len){
	testmode_resp_pkt_t *pResp = (testmode_resp_pkt_t *)uart_tx_buf;
	u8 *payload = pResp->payload;

	pResp->hdr = RESP_HEADER_FLAG;
	pResp->len = len+1;
	pResp->cmd_id = cmd;

	if(len){
		memcpy(payload, data, len);
	}
	*(payload+len) = checksum_crc8(&pResp->len, len+2);

	u32 uartSendTime = clock_time();
	uart_pktSend((u8 *)pResp, pResp->len + OVERHEAD_LEN);
	while(!uart_tx_done() && !clock_time_exceed(uartSendTime, MSO_UART_SEND_TIMEOUT));
}

static void testmode_set(void){
	uart_resp_send(TEST_CMD_RESP_SUCC, NULL, 0);
	mso_setAppState(MSO_TESTING_STATE);
}

static void usermode_set(void){
	uart_resp_send(TEST_CMD_RESP_SUCC, NULL, 0);

	mso_restoreAppState();
	mso_appVars.flags.bf.allowedDeep = 1;
}

static void rootmode_set(void){
	testmode_set();
}

static void irmode_set(void){
	uart_resp_send(TEST_CMD_RESP_SUCC, NULL, 0);

	/* TODO: */


	mso_setAppState(MSO_TESTING_TX_STATE);
}

static void rfmode_set(void){
	uart_resp_send(TEST_CMD_RESP_SUCC, NULL, 0);

	/* send packet */
	u32 i = 0, j = 0;
	u8 *rfPktBuf = ev_buf_allocate(SMALL_BUFFER);

	for(i = 0; i < 32; i++){
		rfPktBuf[i] = i;
	}
	for(j = 0; j < 20; j++){
		for(i = 0; i < 3; i++){
			rf_setChannel(15 + i * 5);
			rf_tx(rfPktBuf, 32);
			WaitMs(2);
		}
	}
	ev_buf_free(rfPktBuf);

	mso_setAppState(MSO_TESTING_TX_STATE);
}

static void sleepmode_set(void){
	uart_resp_send(TEST_CMD_RESP_SUCC, NULL, 0);

	/* after packet is sent, system enter deep sleep mode */
	sys_enter_deepsleep_mode();
}

static void mac_get(void){
	uart_resp_send(TEST_CMD_RESP_SUCC, macPib.extendedAddress, 8);
}

static void adc_get(void){
	u16 battery_value1 = adc_BatteryValueGet();
	uart_resp_send(TEST_CMD_RESP_SUCC, (u8 *)&battery_value1, 2);
}


static void uart_cmd_parse(u8 *data){
	testmode_cmd_pkt_t *pCmd = (testmode_cmd_pkt_t *)data;

	if(pCmd->hdr != CMD_HEADER_FLAG || pCmd->len != CMD_LENGTH){
		return;
	}
	/*TODO: check sum ? */

	for(s32 i = 0; i < sizeof(command_callback)/sizeof(testmode_handler_t); i++){
		if(pCmd->cmd_id == command_callback[i].cmd_id){
			u8 curMode = mso_getAppState();
			if(curMode == MSO_PRE_TESTING_STATE){
				if(pCmd->cmd_id == TESTMODE_CMD_SET_TEST_MODE){
					if(command_callback[i].cmd_cb){
						command_callback[i].cmd_cb();
					}
				}else{
					/* send out error response */
					uart_resp_send(TEST_CMD_RESP_FAIL, NULL, 0);
				}
			}else if(curMode == MSO_TESTING_STATE){
				if(command_callback[i].cmd_cb){
					command_callback[i].cmd_cb();
				}
			}else if(curMode == MSO_TESTING_TX_STATE){
				if(pCmd->cmd_id == TESTMODE_CMD_SET_USER_MODE ||
				   pCmd->cmd_id == TESTMODE_CMD_SET_ROOT_MODE){
					if(command_callback[i].cmd_cb){
						command_callback[i].cmd_cb();
					}
				}else{
					/* send out error response */
					uart_resp_send(TEST_CMD_RESP_FAIL, NULL, 0);
				}
			}
			return;
		}
	}
}

static void uart_receive_handler(void){
	u32 *pLen = (u32 *)uart_rx_buf;
	if(uartRecvBuf || *pLen > 32){
		return;
	}

	uartRecvBuf = ev_buf_allocate(LARGE_BUFFER);
	memcpy(uartRecvBuf, &uart_rx_buf[4], *pLen);
}

void test_mode_init(void){
	/* uart init */
	uart_RecBuffInit(uart_rx_buf, sizeof(uart_rx_buf)/sizeof(u8));
	uart_irq_init(uart_receive_handler);
	CLK32M_UART115200;  //use DMA mode
	UART_GPIO_CFG_PC2_PC3();
}

void test_mode_task(void){
	/* for test */
#if 0
	if(T_testModeCmd){
		testmode_cmd_pkt_t *pCmd = (testmode_cmd_pkt_t *)uart_tx_buf;
		pCmd->hdr = CMD_HEADER_FLAG;
		pCmd->len = CMD_LENGTH;
		pCmd->cmd_id = T_testModeCmd;
		pCmd->crc = 0;
		uart_pktSend((u8 *)pCmd, pCmd->len + OVERHEAD_LEN);
		T_testModeCmd = 0;
	}
#endif

	if(uartRecvBuf){
		uart_cmd_parse(uartRecvBuf);
		ev_buf_free(uartRecvBuf);
		uartRecvBuf = NULL;
	}
}

#endif



