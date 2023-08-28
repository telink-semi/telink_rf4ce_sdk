## V2.4.2.0

### Features
* Support B92 A3
### Bug Fixes
* N/A
### BREAKING CHANGES
* N/A
### CodeSize

  * tl_rf4ce_sdk/build/tlsr_riscv/zrc2_rc_b92
    - Flash:106k
    - RAM:25k
  * tl_rf4ce_sdk/build/tlsr_riscv/zrc2_dongle_b92
    - Flash:88k
    - RAM:19k
  * tl_rf4ce_sdk/build/tlsr_riscv/mso_rc_b92
    - Flash:88k
    - RAM:24k
  * tl_rf4ce_sdk/build/tlsr_riscv/mso_dongle_b92
    - Flash:74k
    - RAM:19k

## V2.4.1.1
### Features
* Add SDK version information in the bin firmware.
* Add CRC check to ensure the integrity of new firmware upgraded by OTA.
* Update ADC and flash driver for 8258 with embedded zbit flash.
### Bug Fixes
* Fix the extended channel save issue.
### BREAKING CHANGES
* N/A

### Features
* 在bin文件里增加SDK版本信息
* 增加CRC校验，确保OTA升级的新固件的完整性
* 为内嵌zbit flash的8258/8278更新ADC和Flash驱动
### Bug Fixes
* 修复扩展频点保存的问题
### BREAKING CHANGES
* N/A

## V2.4.0.1
### Features
* Normalizes the naming of content related to private schemas.
### Bug Fixes
* N/A
### BREAKING CHANGES
* N/A

### Features
* 规范化与私有模式相关内容的命名.
### Bug Fixes
* N/A
### BREAKING CHANGES
* N/A

## V2.4.0.0(Public release)
### Features
* Support mso profile.([ab6f52ff](http://192.168.48.36/src/rf4ce/telink_rf4ce_src/commit/ab6f52ff4981bcffa095c95af81eb90af5b77bb5))
* Replace Telink BSD license with Apache 2.0 License.([0b01472f](http://192.168.48.36/src/rf4ce/telink_rf4ce_src/commit/0b01472faacd619581461e0d041a7712f1ae4285))
### Bug Fixes
* DRIVER
  - Fixed 8278 watchdog API and driver abstraction layer mismatch([61c9ef92](http://192.168.48.36/src/rf4ce/telink_rf4ce_src/commit/61c9ef927e72b5114d654ae8dd779d6ceab724d6))
### BREAKING CHANGES
* N/A

### Features
* 支持mso profile.([ab6f52ff](http://192.168.48.36/src/rf4ce/telink_rf4ce_src/commit/ab6f52ff4981bcffa095c95af81eb90af5b77bb5))
* license由Telink BSD更新为Apache 2.0.([0b01472f](http://192.168.48.36/src/rf4ce/telink_rf4ce_src/commit/0b01472faacd619581461e0d041a7712f1ae4285))
### Bug Fixes
* DRIVER
  - 修复8278 watchdog API 和 驱动抽象层不匹配问题([61c9ef92](http://192.168.48.36/src/rf4ce/telink_rf4ce_src/commit/61c9ef927e72b5114d654ae8dd779d6ceab724d6))
### BREAKING CHANGES
* N/A




## V2.3.3
### Features
* support zbit flash
* calibrate the power supply voltage of zbit on 8258/8278 chips
* add flash lock functions,the default state of lock is OFF
### Bug Fixes
* DRIVER
  - fix the issue that the USBCDC state can't be restored when USBCDC missing ACK
### BREAKING CHANGES
* N/A

### Features
* 支持zbit flash
* 8258/8278芯片根据校准值校准zbit的电源电压
* 增加flash锁功能，锁默认状态是关闭的
### Bug Fixes
* DRIVER
  - 修复USBCDC丢失ACK，状态不恢复问题
### BREAKING CHANGES
* N/A

## V2.3.2
### Features
* add ir dma mode on 8258/8278 chips
* add Flash adaptive functions
### Bug Fixes
* MAC
  - Turn off RF when pull Request timeout occurs
### BREAKING CHANGES
* N/A

### Features
* 8258/8278芯片增加红外DMA模式
* 增加flash自适应函数
### Bug Fixes
* MAC
  - pull Request超时关闭RF
### BREAKING CHANGES
* N/A

## V2.3.1
### Bug Fixs:
* add macros to enable GPIO UART print

### Features:
* support 8278 A2 chip
* optimize the voice data transmission process and resend voice data at 16K sampling rate

### BREAKING CHANGES
* N/A

## V2.3.0
### Bug Fixs:
* fix parameter error of queue in USB CDC TX function
* update 8258 ADC driver to avoid ram being flushed by ADC data
* optimize the continuity of the audio waveform by dongle only reporting valid voice data

### Features:
* support 8278 A1 chip
* change the pairing key of 8278 RCU back to sw5
* add frequency hopping function when transmitting voice data

### BREAKING CHANGES
* N/A

## V2.2.0
### Bug Fixs:
* update 826x adc driver to improve the battery detection precision

### Features:
* support 8278 A0 chip
* change the 8278 RCU's pairing key to SW23, 8258/826x RCU remain unchanged

### BREAKING CHANGES
* N/A

## V2.1.0
### Bug Fixs
* fix mac address issue
* fix quick set IR toggle bit issue

### Features
* Auto-generated ota file in the proj
* add rsp code in the callback function for pushing attribute response
* add callback for all of the command, optimize the discovery procedure
* change led action for pair, audio issue for 8258
* add demo for rf4ce/ble switch

### BREAKING CHANGES
* N/A