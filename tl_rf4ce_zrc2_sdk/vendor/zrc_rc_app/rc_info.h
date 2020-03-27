#pragma  once

//enum version{
//    MAJOR_VER = 1,
//    MINOR_VER = 0,
//    PATCH_VER = 1,
//    BUILD_VER = 4
//};
#define firmwareVersion 	0x01000104
#define APP_VENDOR_ID_LIST  0x1141

#if (MCU_CORE_8258)
#define CHIP_ID  0x8258
#elif (MCU_CORE_826x)
#define CHIP_ID  0x8269
#elif (MCU_CORE_8278)
#define CHIP_ID  0x8278
#else
#define CHIP_ID  0x0000
#endif
