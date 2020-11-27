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

## V2.3.0
### Bug Fixs:
* fix parameter error of queue in USB CDC TX function
* update 8258 ADC driver to avoid ram being flushed by ADC data
* optimize the continuity of the audio waveform by dongle only reporting valid voice data

### Features:
* support 8278 A1 chip
* change the pairing key of 8278 RCU back to sw5
* add frequency hopping function when transmitting voice data



## V2.2.0
### Bug Fixs:
* update 826x adc driver to improve the battery detection precision

### Features:
* support 8278 A0 chip
* change the 8278 RCU's pairing key to SW23, 8258/826x RCU remain unchanged

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
