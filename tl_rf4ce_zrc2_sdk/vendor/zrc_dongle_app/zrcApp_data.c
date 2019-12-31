/********************************************************************************************************
 * @file     zrcApp_data.c
 *
 * @brief    the callback for receiving data from ZRC2 stack
 *
 * @author
 * @date     July. 2, 2019
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#include "../../proj/tl_common.h"
#include "zrcApp_data.h"

#include "../common/TL_specificData/tl_specific_data.h"

void zrc_appDataIndCb(u8 pairingRef, u8 *pd, u8 len){
	tl_appDataIndicate( pd, len);
}
