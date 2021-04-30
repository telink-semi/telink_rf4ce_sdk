
#pragma once


#include "common/types.h"
#include "common/bit.h"
#include "common/utility.h"
#include "common/static_assert.h"
#include "common/assert.h"
#include "common/string.h"
#include "common/compiler.h"

#include "os/ev.h"
#include "os/ev_buffer.h"
#include "os/sys.h"
#include "os/timer.h"
#include "drivers/usb/usb.h"

#include "../platform/platform_includes.h"
#include "drivers/nv.h"

#include "../vendor/common/user_config.h"
#include "../vendor/common/user_trace.h"


#define _CODE_MAC_		__attribute__((section(".sect_mac")))
#define _CODE_NWK_		__attribute__((section(".sect_nwk")))
#define _CODE_PBP_		__attribute__((section(".sect_pbp")))
#define _CODE_MSO_		__attribute__((section(".sect_mso")))
#define _CODE_ZRC_		__attribute__((section(".sect_zrc")))
#define _CODE_GDP_		__attribute__((section(".sect_gdp")))
#define _CODE_DRV_		__attribute__((section(".sect_drv")))


