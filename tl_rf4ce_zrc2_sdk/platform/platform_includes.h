
#pragma once


#if MCU_CORE_826x
#include "./tlsr_826x/register_826x.h"
#include "./tlsr_826x/clock_826x.h"
#include "./tlsr_826x/pm_826x.h"
#include "./tlsr_826x/rf_drv_826x.h"
#include "./tlsr_826x/adc_826x.h"
#include "./tlsr_826x/gpio_826x.h"
#include "./tlsr_826x/gpio_default_826x.h"
#include "./tlsr_826x/pwm_826x.h"
#include "./tlsr_826x/aes_ccm_826x.h"
#include "./tlsr_826x/uart_826x.h"
#include "./tlsr_826x/i2c_826x.h"
#include "./tlsr_826x/bsp_826x.h"
#include "./tlsr_826x/flash_826x.h"
#include "./tlsr_826x/audio_826x.h"
#include "./tlsr_826x/spi_826x.h"
#elif MCU_CORE_8258
#include "./tlsr_8258/driver_8258.h"

#elif MCU_CORE_8278
#include "./tlsr_8278/driver_8278.h"


#endif

