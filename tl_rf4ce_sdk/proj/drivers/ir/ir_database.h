/********************************************************************************************************
 * @file    ir_database.h
 *
 * @brief   This is the header file for ir_database.h
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

#ifndef __IR_DATABASE_H__
#define __IR_DATABASE_H__

#include "../../tl_common.h"


#define NO_SPECIAL              0xff

#define IR_ENTRY_TYPE_8         0x00
#define IR_ENTRY_TYPE_16        0x01
#define IR_ENTRY_TYPE_DTV       0x02

// pulse parameters in usec
/* Following definition for known IR type */
typedef enum {
    IR_TYPE_NOT_SUPPORTED,
    IR_TYPE_NEC,
    IR_TYPE_SONY,
    IR_TYPE_RC5,
    IR_TYPE_JVC,
    IR_TYPE_MITSUBISHI,
    IR_TYPE_TC9012,
    IR_TYPE_SHARP,
    IR_TYPE_M50119P,
    IR_TYPE_LC7461,
    IR_TYPE_MITC8D8,
    IR_TYEP_MN6014_C6D6,
    IR_TYEP_MN6014_C5D6,
    IR_TYPE_GEMINI,
    IR_TYPE_VICTOR,
    IR_TYPE_LC7464M,
    IR_TYPE_NECE2,
    IR_TYPE_M50642,

} ir_type_t;

typedef enum {
    IR_KEY_POWER,
    IR_KEY_VOL_UP,
    IR_KEY_VOL_DN,
    IR_KEY_MUTE,
    IR_SUPPORTED_KEY_NUM,
} ir_key_t;

typedef enum {
    IR_SKEY_POWER,
    IR_SKEY_VOL_UP,
    IR_SKEY_VOL_DN,
    IR_SKEY_MUTE,
    IR_SKEY_CH_UP,
    IR_SKEY_CH_DN,
    IR_KEY_0,
    IR_KEY_1,
    IR_KEY_2,
    IR_KEY_3,
    IR_KEY_4,
    IR_KEY_5,
    IR_KEY_6,
    IR_KEY_7,
    IR_KEY_8,
    IR_KEY_9,
    IR_SKEY_INFO,// 0x08
    IR_SKEY_LAST,// 0x0E
    IR_SKEY_MENU, // 0x3E
    IR_SKEY_PAGEUP,//0x36
    IR_SKEY_PAGEDN,//0x37
    IR_SKEY_EXIT,//0x2C
    IR_SKEY_ENTER,//0x0C
    IR_SKEY_LEFT,//0x03
    IR_SKEY_RIGHT,//0x04

    IR_SAMPLE_SUPPORTED_KEY_NUM,
} ir_samplekey_t;




typedef void ( *ir_sendFn_t )( u8 addr, u8 cmd, u8 fRepeat);
typedef void ( *ir_sendFn_16_t )( u16 addr, u16 cmd, u8 fRepeat );

typedef struct {
    u16 tvcode;
    u8  type;
    u8  addr;
    u8  cmd[IR_SUPPORTED_KEY_NUM];
    ir_sendFn_t fn;
} ir_generaldb_t;

typedef struct {
    u16  tvcode;
    u8   type;
    u16  addr;
    u16  cmd[IR_SUPPORTED_KEY_NUM];
    ir_sendFn_16_t fn;
} ir_bit16db_t;

typedef struct {
    u16 tvcode;
    u8 type;
    u8 addr;
    u8 cmd[IR_SAMPLE_SUPPORTED_KEY_NUM];
    ir_sendFn_t fn;
} ir_sampledb_t;





// NEC  uPD6121G,uPD6121F...
#define NEC_BASE_TIME           (550 / IR_BASE_TIME)
#define NEC_HDR_MARK            9000
#define NEC_HDR_SPACE           4500
#define NEC_BIT_MARK            560
#define NEC_ONE_SPACE           1600
#define NEC_ZERO_SPACE          560
#define NEC_RPT_SPACE           2250
#define NEC_RPT_INTERVAL        110000


// SONY  uPD6124
#define SONY_HDR_MARK           2400
#define SONY_HDR_SPACE          600
#define SONY_BIT_MARK           600
#define SONY_ONE_MARK           1200
#define SONY_ZERO_MARK          600
#define SONY_RPT_LENGTH         45000
#define SONY_DOUBLE_SPACE_USECS  500  // usually ssee 713 - not using ticks as get number wrapround


// RC5 SAA3010-RC5
#define RC5_T1                  900
#define RC5_BIT_MARK            900
#define RC5_RPT_LENGTH          46000


// JVC
#define JVC_HDR_MARK            8400
#define JVC_HDR_SPACE           4200
#define JVC_BIT_MARK            600
#define JVC_ONE_SPACE           1600
#define JVC_ZERO_SPACE          550
#define JVC_RPT_LENGTH          60000


// Mitsubishi
#define MITSUBISHI_HDR_MARK     8000
#define MITSUBISHI_HDR_SPACE    4000
#define MITSUBISHI_BIT_MARK     560
#define MITSUBISHI_ONE_SPACE    1600
#define MITSUBISHI_ZERO_SPACE   560
#define MITUSBISHI_GAP          4000
#define MITUSBISHI_MIN_REPEAT   3
#define MITUSBISHI_STOP_TIME    20000
#define MITUSBISHI_RPT_LENGTH   60000


// TC9012
#define TC9012_HDR_MARK         4500
#define TC9012_HDR_SPACE        4500
#define TC9012_BIT_MARK         560
#define TC9012_ONE_SPACE        1600
#define TC9012_ZERO_SPACE       560
#define TC9012_RPT_LENGTH       108000


// SHARP
#define SHARP_BIT_MARK          300
#define SHARP_ONE_SPACE         1700
#define SHARP_ZERO_SPACE        700
#define SHART_RPT_LENGTH        68000


// M50119P
#define M50119P_BIT_MARK        500
#define M50119P_ONE_SPACE       1500
#define M50119P_ZERO_SPACE      500
#define M50119P_RPT_LENGTH      30000

// M50119L
#define M50119L_BIT_MARK        250
#define M50119L_ONE_SPACE       1750
#define M50119L_ZERO_SPACE      750
#define M50119L_RPT_LENGTH      25500


// LC7461-C13
#define LC7461_HDR_MARK         9000
#define LC7461_HDR_SPACE        4500
#define LC7461_BIT_MARK         560
#define LC7461_ONE_SPACE        1600
#define LC7461_ZERO_SPACE       560
#define LC7461_RPT_LENGTH       108000

// MIT-C8D8
#define MITC8D8_BIT_MARK        300
#define MITC8D8_ONE_SPACE       2100
#define MITC8D8_ZERO_SPACE      800
#define MITC8D8_RPT_LENGTH      50000


// MN6014-C6D6
#define MN6014_C6D6_HDR_MARK    3300
#define MN6014_C6D6_HDR_SPACE   3300
#define MN6014_C6D6_BIT_MARK    850
#define MN6014_C6D6_ONE_SPACE   2500
#define MN6014_C6D6_ZERO_SPACE  850
#define MN6014_C6D6_RPT_LENGTH  105000
#define MN6014_C5D6_RPT_LENGTH  100000

// GEMINI
#define GEMINI_HDR_MARK         500
#define GEMINI_HDR_SPACE        2600
#define GEMINI_BIT_MARK         500
#define GEMINI_GAP              21000
#define GEMINI_RPT_LENGTH       90700


// VICTOR-C8D8
#define VICTOR_HDR_MARK         8500
#define VICTOR_HDR_SPACE        4150
#define VICTOR_BIT_MARK         560
#define VICTOR_ONE_SPACE        1500
#define VICTOR_ZERO_SPACE       560
#define VICTOR_GAP              21000
#define VICTOR_RPT_LENGTH       60000

//FB937A


//NEC2-E2
#define NEC2E2_HDR_MARK         9000
#define NEC2E2_HDR_SPACE        4500
#define NEC2E2_BIT_MARK         550
#define NEC2E2_ONE_SPACE        1600
#define NEC2E2_ZERO_SPACE       550
#define NEC2E2_RPT_LENGTH       89250


//LC7464M Panasonic
#define LC7464M_HDR_MARK        3400
#define LC7464M_HDR_SPACE       1700
#define LC7464M_BIT_MARK        400
#define LC7464M_ONE_SPACE       1400
#define LC7464M_ZERO_SPACE      450
#define LC7464M_RPT_LENGTH      83000

//M50462
#define M50462_BIT_MARK        250
#define M50462_ONE_SPACE       1800
#define M50462_ZERO_SPACE      800
#define M50462_RPT_LENGTH      45000

//PHILIPS
#define PHILIPS_HDR_MARK        2650
#define PHILIPS_HDR_SPACE       850
#define PHILIPS_BIT_MARK        450
#define PHILIPS_TOGGLE_BIT      900;
#define PHILIPS_RPT_LENGTH      113000







/* Following definition for unknown IR type */
#define IR_BASE_TIME            50


ir_generaldb_t* irdb_getEntry(u16 tvcode);
ir_bit16db_t* irdb16_getEntry(u16 tvcode);
ir_sampledb_t* irdb_getSampleEntry(void);



#endif // __IR_DATABASE_H__

