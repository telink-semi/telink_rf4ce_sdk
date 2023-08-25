/********************************************************************************************************
 * @file    module_test.c
 *
 * @brief   This is the source file for module_test.c
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

#include "../../proj/config/user_config.h"
#include "../../proj/tl_common.h"
#include "../../platform/platform_includes.h"
#include "../../proj/drivers/drv_adc.h"
#include "../../proj/drivers/drv_timer.h"
/*
 * Note: test case based on the Ohsung-RF211 8267 board
 */

#define MODULE_TEST_ADC_ENABLE	0
#define MODULE_TEST_PM_ENABLE 	0
#define MODULE_TEST_PWM_ENABLE 	0
#define MODULE_TEST_RSSI_ENABLE 	0
#define MODULE_TEST_RF_ENABLE	0

#define MODULE_TEST_LED			0
#define MODULE_TEST_UART		0

#define MODULE_TEST_TIMER		0
#define MODULE_I2C_TEST			0

#if(MODULE_I2C_TEST)
#define I2C_MASTER_MODE_EN		1
#define I2C_SLAVE_MAPPING_MODE	1
#define I2C_SLAVE_DMA_MODE		0
#define i2c_master_mode

u32 mapping_data[4] = {0x11223344, 0x55667788, 0x99aabbcc, 0xeeff0011} ;
u32 read_buff[4] = {0};
u32 slave_read_buff[32] = {0};
volatile u8 T_i2cMasterOp = 0;
void moduleTest_forI2c(void){
	i2c_pin_initial(GPIO_PC0,GPIO_PC1);

#if I2C_MASTER_MODE_EN
	i2c_master_init(0x5c,0x50);
	int opNum = 0;

	while(1){
		if(T_i2cMasterOp){
			T_i2cMasterOp = 0;
			for(int i = 0; i < 4; i++){
				mapping_data[i] = opNum+1;
				read_buff[i] = 0;
			}
			i2c_write_buff_mapping(mapping_data,0x0b);///0x0b
			WaitMs(1000);
			i2c_read_buff_mapping(read_buff,0x0b);///0x0b
			WaitMs(1000);
			opNum++;
		}
	}
#else
	u8 *pBuf = slave_read_buff;
	for(int i = 0; i < 128; i++){
		pBuf[i] = i;
	}
	#if I2C_SLAVE_DMA_MODE
		I2C_SlaveInit(I2C_SLAVE_DMA,NULL,0x5c,I2C_IRQ_ENABLE);///slave mode/Don't care/slave id/
	#elif I2C_SLAVE_MAPPING_MODE
		I2C_SlaveInit(I2C_SLAVE_MAP,(unsigned char*)slave_read_buff,0x5c,I2C_IRQ_ENABLE);///slave mode/buff addr/slave id/interrupt en_dis
	#endif
	i2c_slave_rev_irq_en();
	while(1);
#endif
}
#endif

#if (MODULE_TEST_TIMER)
volatile int T_timerCnt = 0;
int timer1IrqCb(void* arg){
	T_timerCnt++;

//	if(T_timerCnt & 0x01){
//		hwTmr_setInterval(TIMER_IDX_1, 10*1000);
//	}else{
//		hwTmr_setInterval(TIMER_IDX_1, 5000*1000);
//	}
}
void moduleTest_forTimer(void){
	irq_enable();
	drv_hwTmr_init(TIMER_IDX_1, TIMER_MODE_SCLK);
	drv_hwTmr_set(TIMER_IDX_1, 1000*1000, timer1IrqCb, NULL);
	while(1){
		WaitMs(100);
		wd_clear();
	}
}
#endif


#if MODULE_TEST_UART
u8 uart_tx_data[4] = {0};
u8 uart_rx_data[32] = {0};
volatile u8 T_uartSendPkt = 0;
void moduleTest_forUart(void){
	uart_RecBuffInit(uart_rx_data, sizeof(uart_rx_data)/sizeof(u8));
	CLK32M_UART115200;  //use DMA mode
	UART_GPIO_CFG_PC2_PC3();
	irq_enable();

	for(int i = 0; i < sizeof(uart_tx_data)/sizeof(u8); i++){
		uart_tx_data[i] = i;
	}

	for(;;){
		T_uartSendPkt++;
		uart_tx_data[0] = T_uartSendPkt;
		uart_pktSend(uart_tx_data, sizeof(uart_tx_data)/sizeof(u8));
		while(uart_status == UART_STA_TX_DOING);
		WaitMs(1000);
	}
}
#endif

#if MODULE_TEST_RSSI_ENABLE
s8 T_rssiValue_15[32] = {0};
s8 T_rssiValue_20[32] = {0};
s8 T_rssiValue_25[32] = {0};
void moduleTest_forEdScan(void){
	while(1){
		for(int i = 0; i < 3; i++){
			rf_setChannel(15+5*i);
			rf_setRxMode();

			for(int j = 0; j < 32; j++){
				if(i == 0)T_rssiValue_15[j] = rf_getRssi();
				if(i == 1)T_rssiValue_20[j] = rf_getRssi();
				if(i == 2)T_rssiValue_25[j] = rf_getRssi();
				WaitUs(20);
			}
		}
	}
}
#endif

#if MODULE_TEST_ADC_ENABLE
u16 T_bat_det_buf[64] = {0};
/*********************************************************************
 * @fn      msoApp_adcModuleTest
 *
 * @brief   adc module verification after power on or recover from deep sleep.
 *
 * @param   None
 *
 * @return  None
 */
void  moduleTest_forAdc(void){
 /* init ADC */
	drv_adc_init();//
	drv_adc_battery_detect_init();
	WaitUs(20);	

	while(1){
		int det_num = 0;
		while(det_num++ < 64){
			T_bat_det_buf[det_num] =drv_get_adc_data();// adc_BatteryValueGet();
			WaitMs(10);
		}
	}


    if(MODULE_TEST_LED){
		gpio_set_func(MODULE_TEST_LED, AS_GPIO);
		gpio_set_output_en(MODULE_TEST_LED, 1);
		gpio_set_input_en(MODULE_TEST_LED, 0);
		u8 led_sta = 0;
		for(int i = 0 ; i < 10; i++){
			gpio_write(MODULE_TEST_LED, led_sta);
			WaitMs(200);
			led_sta ^= 1;
		}
    }

	PM_PadSet(GROUPA_PIN1|GROUPA_PIN3|GROUPA_PIN4|GROUPA_PIN7, 1, 1);     //active high
	PM_PadSet(GROUPB_PIN1|GROUPB_PIN4, 1, 1);     //active high
	PM_LowPwrEnter(LOWPWR_DEEPSLEEP, WAKEUP_SRC_PAD, 0);
}
#endif

#if MODULE_TEST_PM_ENABLE
#define SuspendTimeInMs		(5000 * CLOCK_SYS_CLOCK_1MS)

u8 T_pkt_send_buf[] = {
		0x61, 0x88, 0x23, 0x11, 0x36, 0x67, 0xa0, 0xD0, 0x2f, 0xb8, 0x0f, 0x0, 0x0,
		0xc0, 0xd0, 0x10, 0x03, 0x26, 0xbb, 0x5b, 0x88, 0xbc, 0xff, 0xff
};
void moduleTest_forPm(u8 mode){
	/*
	 * Todo: wake-up pin should be pulled down
	 * */

	//rf_setChannel(15);
	if(mode == LOWPWR_DEEPSLEEP){
		PM_PadSet(GROUPA_PIN1|GROUPA_PIN3|GROUPA_PIN4|GROUPA_PIN7, 1, 1);     //active high
		PM_PadSet(GROUPB_PIN1|GROUPB_PIN4, 1, 1);     //active high
		PM_LowPwrEnter(LOWPWR_DEEPSLEEP, WAKEUP_SRC_PAD, 0);
	}else{
		while(1){
			//rf_tx(T_pkt_send_buf, 25);
			//WaitMs(2);
			PM_LowPwrEnter(LOWPWR_SUSPEND, WAKEUP_SRC_TIMER, clock_time()+SuspendTimeInMs);
			u8 led_sta = 0;
			for(int i = 0 ; i < 10; i++){
				if(MODULE_TEST_LED){
					gpio_write(MODULE_TEST_LED, led_sta);
				}
				WaitMs(100);
				led_sta ^= 1;
			}
		}
	}
}
#endif

#if MODULE_TEST_PWM_ENABLE
#define PWM_FREQUENCY	56000		//56kHz
volatile int T_pwmChg = 0;
void moduleTest_forPwm(void){
	PWM0_CFG_GPIO_A0();
	pwm_Init(0);   //set PWM divider
	u16 max_tick = CLOCK_SYS_CLOCK_HZ/PWM_FREQUENCY;
	u16 cmp_tick = max_tick / 2;
	pwm_Open(PWM0, NORMAL, 0, cmp_tick, max_tick, 0x2fff);
	EN_PWM(PWM0);
	while(1);
	{
		T_pwmChg += 2;
		EN_PWM(PWM0);
		WaitMs(100);
		DIS_PWM(PWM0);
		WaitMs(100);
		EN_PWM(PWM0);
		WaitMs(150);
		DIS_PWM(PWM0);
		WaitMs(150);
	}
}
#endif

#if MODULE_TEST_RF_ENABLE
void module_rf_recv_test(void){
	rf_init();
	mac_rxInit();
	//rf_setChannel(15);
	//rf_setRxMode();
	irq_enable();
	while(1);
}
#endif

void moduleTest_start(void){
#if MODULE_TEST_RF_ENABLE
	module_rf_recv_test();
#endif

#if MODULE_TEST_RSSI_ENABLE
	moduleTest_forEdScan();
#endif

#if MODULE_TEST_ADC_ENABLE
	moduleTest_forAdc();
#endif
#if MODULE_TEST_PM_ENABLE
	moduleTest_forPm(LOWPWR_SUSPEND);
#endif
#if MODULE_TEST_PWM_ENABLE
	moduleTest_forPwm();
#endif

#if MODULE_TEST_UART
	moduleTest_forUart();
#endif

#if (MODULE_TEST_TIMER)
	moduleTest_forTimer();
#endif

#if (MODULE_I2C_TEST)
	moduleTest_forI2c();
#endif
}



