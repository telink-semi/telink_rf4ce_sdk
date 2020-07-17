
#pragma  once

/**
 *  @brief Definition for the report data interval, 1s
 */

/**
 *  @brief Definition for the mso app cmd 
 */
enum {
    ZRC_APP_ID_PARING_KEYCODE =  0x20,
    ZRC_APP_ID_NORMAL_KEY =  0x21,
};

enum {
    ZRC_APP_CMD_CODE_IND =  0xF1,
    ZRC_APP_VALIDATION_KEY_IND =  0xF2,
    ZRC_APP_VALIDATION_CODE_IND =  0xF3,
    ZRC_APP_SET_ATTR_IND =  0xF4,
    ZRC_APP_VALIDATION_SUCC_IND =  0xF5,
    ZRC_APP_UNPIAR_IND =  0xF6,
};

/**
 *  @brief Definition for the mso app validation state 
 */

enum {
    ZRC_APP_INVALID_STATE,
    ZRC_APP_START_VALIDATION,
    ZRC_APP_VALIDATING_STATE,
    ZRC_APP_VALIDATED_STATE,
    ZRC_APP_NORMAL_STATE,
};

