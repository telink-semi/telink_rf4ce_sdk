/********************************************************************************************************
 * @file    module_test.c
 *
 * @brief   This is the source file for module_test.c
 *
 * @author  Zigbee GROUP
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

#define MODULE_TEST_ADC_ENABLE     0
#define MODULE_TEST_PM_ENABLE      0
#define MODULE_TEST_PWM_ENABLE     0
#define MODULE_TEST_RSSI_ENABLE    0
#define MODULE_TEST_RF_ENABLE      0

#define MODULE_TEST_UART           0

#define MODULE_TEST_TIMER          0
#define MODULE_I2C_TEST            0

#if(MODULE_I2C_TEST)
#define I2C_MASTER_MODE_EN        1
#define I2C_SLAVE_MAPPING_MODE    1
#define I2C_SLAVE_DMA_MODE        0
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
#if defined(MCU_CORE_TL321X)
    #define TEST_GPIO_0         GPIO_PB1
#endif
#define TEST_TIME_IDX           TIMER_IDX_3

volatile int T_timerCnt = 0;
int timer1IrqCb(void* arg){
    T_timerCnt++;

    if (T_timerCnt%2) {
        gpio_set_level(TEST_GPIO_0, 0);
    } else {
        gpio_set_level(TEST_GPIO_0, 1);
    }

    return 0;
}

void moduleTest_forTimer(void){
    gpio_function_en(TEST_GPIO_0);
    gpio_set_output(TEST_GPIO_0, 1); //enable output
    gpio_set_input(TEST_GPIO_0, 0);  //disable input
    gpio_set_level(TEST_GPIO_0, 1);     //LED On

    irq_enable();
    drv_hwTmr_init(TEST_TIME_IDX, TIMER_MODE_SCLK);
    drv_hwTmr_set(TEST_TIME_IDX, 1000*1000, timer1IrqCb, NULL);
    while(1){
        WaitMs(100);
        wd_clear();
    }
}
#endif


#if MODULE_TEST_UART
#include "../../proj/drivers/drv_uart.h"
#if defined(MCU_CORE_TL321X)
    #define UART_TX_PIN    GPIO_PC4
    #define UART_RX_PIN    GPIO_PC5
#endif

__attribute__((aligned(4))) u8 moduleTest_uartTxBuf[4] = {0};
__attribute__((aligned(4))) u8 moduleTest_uartRxBuf[32] = {0};

volatile u8  T_uartPktSentSeqNo = 0;
volatile u32 T_uartPktRecvSeqNo = 0;
volatile u32 T_uartPktRecvLen = 0;
volatile u32 T_uartPktSentExcept = 0;

typedef struct {
    u32    dataLen;
     u8    dataPayload[1];
} uart_rxData_t;

void module_test_uartRcvHandler(void)
{
    /*
     * the format of the uart rx data: length(4 Bytes) + payload
     *
     */
    uart_rxData_t *rxData = (uart_rxData_t *)moduleTest_uartRxBuf;
    T_uartPktRecvSeqNo = rxData->dataPayload[0];

    if (T_uartPktRecvSeqNo == 0) {
        T_uartPktRecvLen = rxData->dataLen;
    }
}

volatile u8 T_uartSendPkt = 0;
void moduleTest_forUart(void)
{
    drv_uart_pin_set(UART_TX_PIN, UART_RX_PIN);

    drv_uart_init(115200, moduleTest_uartRxBuf, sizeof(moduleTest_uartRxBuf) / sizeof(u8), module_test_uartRcvHandler);

    irq_enable();

    for (int i = 0; i < sizeof(moduleTest_uartTxBuf) / sizeof(u8); i++) {
        moduleTest_uartTxBuf[i] = i;
    }

    while (1) {
        if (T_uartPktRecvSeqNo == 0) {
            if (T_uartPktRecvLen) {
                T_uartPktRecvLen = 0;
                uart_rxData_t *rxData = (uart_rxData_t *)moduleTest_uartRxBuf;
                drv_uart_tx_start(rxData->dataPayload, rxData->dataLen);
            }
        } else if (T_uartPktRecvSeqNo == 0xAA) {
            moduleTest_uartTxBuf[0] = T_uartPktSentSeqNo++;
            if (drv_uart_tx_start(moduleTest_uartTxBuf, sizeof(moduleTest_uartTxBuf) / sizeof(u8)) == 1) {
                WaitMs(1000);
            } else {
                T_uartPktSentExcept++;
                while(1);
            }
        } else if (T_uartPktRecvSeqNo == 0xCC) {
            u16 random = (u16)drv_u32Rand();

            moduleTest_uartTxBuf[0] = HI_UINT16(random);
            moduleTest_uartTxBuf[1] = LO_UINT16(random);

            if (drv_uart_tx_start(moduleTest_uartTxBuf, 2) == 1) {
                WaitMs(1000);
            } else {
                T_uartPktSentExcept++;
                while(1);
            }
        }
    }
}
#endif

#if MODULE_TEST_RSSI_ENABLE
#include "../../net/rf4ce/mac/mac_phy.h"
#include "../../net/rf4ce/mac/mac_api.h"
volatile s8 T_rssiValue[3][32] = {0};
void moduleTest_forEdScan(void){
    while(1){
        for(int i = 0; i < 3; i++){
            ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(15+5*i));

            for(int j = 0; j < 32; j++){
                T_rssiValue[i][j] = ZB_RADIO_RSSI_GET();
                WaitUs(20);
            }
        }

        WaitMs(5000);
    }
}
#endif

#if MODULE_TEST_ADC_ENABLE
#define MODULE_TEST_LED            GPIO_PB1
volatile u16 T_bat_det_buf[64] = {0};
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
    drv_adc_init();
    drv_adc_battery_detect_init();
    WaitUs(20);
    volatile int det_num = 0;

    det_num = 0;
    while(det_num < 64){
        T_bat_det_buf[det_num] = drv_get_adc_data();// adc_BatteryValueGet();
        det_num++;
        WaitMs(10);
    }


    if(MODULE_TEST_LED){
//        gpio_set_func(MODULE_TEST_LED, AS_GPIO);
        gpio_set_output_en(MODULE_TEST_LED, 1);
        gpio_set_input_en(MODULE_TEST_LED, 0);
        u8 led_sta = 0;
        for(int i = 0 ; i < 10; i++){
            gpio_write(MODULE_TEST_LED, led_sta);
            WaitMs(200);
            led_sta ^= 1;
        }
    }

    while(1);
}
#endif

#if MODULE_TEST_PM_ENABLE
#if defined(MCU_CORE_TL321X)
#define MODULE_TEST_LED        GPIO_PB1
#define TEST_WAKEUP_PAD        GPIO_PC5
#define TEST_WAKEUP_LEVEL      PLATFORM_WAKEUP_LEVEL_LOW

#define PC5_FUNC               AS_GPIO
#define PC5_OUTPUT_ENABLE      0
#define PC5_INPUT_ENABLE       1
#define PULL_WAKEUP_SRC_PC5    GPIO_PIN_PULLUP_10K
#endif

#define SuspendTimeInMs            10000

u8 T_pkt_send_buf[] = {
        0x61, 0x88, 0x23, 0x11, 0x36, 0x67, 0xa0, 0xD0, 0x2f, 0xb8, 0x0f, 0x0, 0x0,
        0xc0, 0xd0, 0x10, 0x03, 0x26, 0xbb, 0x5b, 0x88, 0xbc, 0xff, 0xff
};
void moduleTest_forPm(void){
    /*
     * Todo: wake-up pin should be pulled down
     * */
    gpio_set_gpio_en(TEST_WAKEUP_PAD);
    gpio_set_output_en(TEST_WAKEUP_PAD, 0);
    gpio_set_input_en(TEST_WAKEUP_PAD, 1);
    gpio_set_up_down_res(TEST_WAKEUP_PAD, GPIO_PIN_PULLUP_10K);

    WaitMs(2000);

    platform_wakeup_pad_cfg(TEST_WAKEUP_PAD, TEST_WAKEUP_LEVEL, 1);

    while(1){
#if 1
        platform_lowpower_enter(PLATFORM_MODE_SUSPEND, PLATFORM_WAKEUP_TIMER|PLATFORM_WAKEUP_PAD, SuspendTimeInMs);
#else
//        platform_lowpower_enter(PLATFORM_MODE_DEEPSLEEP, PLATFORM_WAKEUP_TIMER|PLATFORM_WAKEUP_PAD, SuspendTimeInMs);
        platform_lowpower_enter(PLATFORM_MODE_DEEPSLEEP, PLATFORM_WAKEUP_PAD, SuspendTimeInMs);
#endif
    }

}
#endif

#if MODULE_TEST_PWM_ENABLE
#define PWM_FREQUENCY    56000        //56kHz
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
#define TEST_LOGIC_CHANNEL        11
#include "../../net/rf4ce/mac/mac_phy.h"
#include "../../net/rf4ce/mac/mac_api.h"
unsigned char tx_packet[48] __attribute__ ((aligned (4))) = {
    0x09, 0x00, 0x00, 0x00, 0x0a, 0x03, 0x08, 0xd0, 0xff, 0xff, 0xff, 0xff, 0x07
};
void module_rf_recv_test(void){
    rf_init();
    mac_rxInit();

    ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(TEST_LOGIC_CHANNEL));

    irq_enable();
    while(1);
}

void module_rf_send_test(void){
    rf_init();
    mac_rxInit();

    ZB_RADIO_TRX_SWITCH(RF_MODE_TX, LOGICCHANNEL_TO_PHYSICAL(TEST_LOGIC_CHANNEL));
    irq_enable();

    ZB_RADIO_DMA_HDR_BUILD(tx_packet, 8);

    while (1) {
        WaitMs(500);
        ZB_RADIO_TX_START(tx_packet);
    }
}
#endif

void moduleTest_start(void){
#if MODULE_TEST_RF_ENABLE
    module_rf_recv_test();
//    module_rf_send_test();
#endif

#if MODULE_TEST_RSSI_ENABLE
    moduleTest_forEdScan();
#endif

#if MODULE_TEST_ADC_ENABLE
    moduleTest_forAdc();
#endif

#if MODULE_TEST_PM_ENABLE
    moduleTest_forPm();
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




