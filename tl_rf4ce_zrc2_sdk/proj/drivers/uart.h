#pragma once

#include "./usb/app/usbcdc.h"

/** @addtogroup  TELINK_BLE_HAL TELINK HAL Stack
 *  @{
 */

/** @addtogroup  HAL_UART_Module UART Module
 *  @{
 */


/** @addtogroup  HAL_UART_Module_Constant UART Constants
 *  @{
 */

/**
 *  @brief  Setting of the baud rate
 */
#define DFLT_BAUD_RATE  115200 

/** @} end of group HAL_UART_Module_Constant */



/** @addtogroup  HAL_UART_Module_Types UART Types
 *  @{
 */

/**
 *  @brief  Definition of uart_write function return value type
 */
typedef enum uart_sts_e {
    UART_SUCC = 0,                   //!< The uart msg is sent successfully
    UART_BUSY = 1,                   //!< The uart is busy and the msg is not added to the queue, user has to recall the uart send function
    UART_MULTIBLOCK = 2,        //!< The uart is busy but the msg is added to the queue, so the msg will be sent automatically while the uart is idle
} uart_sts_t;

/**
 *  @brief  Definition of uart message received callback function
 */
typedef s32 (*uart_recvCb_t)(u8 *buf);
/** @} end of group HAL_UART_Module_Types */



/** @addtogroup  HAL_UART_Module_Functions UART APIs
 *  @{
 */
/**
  * @brief      Initialize UART Module
  *
  * @param[in]  baudrate - Baudrate for the uart connection
  * @param[in]  cb       - Uart msg receive callback function
  *
  * @return     None
  */
void uart_open(u32 baudrate, uart_recvCb_t cb);

/**
  * @brief      Send UART data
  *
  * @param[in]  buf - Buffer to be sent
  * @param[in]  len - Length of msg to be sent
  *
  * @return     status
  */
uart_sts_t uart_write(u8* buf, u8 len);


/** @} end of group HAL_UART_Module_Functions */

/** @} end of group HAL_UART_Module */

/** @} end of group TELINK_BLE_HAL */


/*Functions for internal use */
void uart_rxHandler(u8* pData);
void uart_txFinishCb(u8* pData);

