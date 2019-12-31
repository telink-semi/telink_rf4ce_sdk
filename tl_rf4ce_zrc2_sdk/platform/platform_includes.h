
#pragma once


#if MCU_CORE_826x
#include "./tlsr_826x/includes/register_826x.h"
#include "./tlsr_826x/includes/clock_826x.h"
#include "./tlsr_826x/includes/pm_826x.h"
#include "./tlsr_826x/includes/rf_drv_826x.h"
#include "./tlsr_826x/includes/adc_826x.h"
#include "./tlsr_826x/includes/gpio_826x.h"
#include "./tlsr_826x/includes/gpio_default_826x.h"
#include "./tlsr_826x/includes/pwm_826x.h"
#include "./tlsr_826x/includes/aes_ccm_826x.h"
#include "./tlsr_826x/includes/uart_826x.h"
#include "./tlsr_826x/includes/i2c_826x.h"
#include "./tlsr_826x/includes/bsp_826x.h"
#include "./tlsr_826x/includes/flash_826x.h"
#include "./tlsr_826x/includes/audio_826x.h"

#elif MCU_CORE_8258
#include "./tlsr_8258/driver_8258.h"

#endif

