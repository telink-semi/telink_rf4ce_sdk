#!/bin/bash 
echo "*****************************************************"
echo "this is post build!! current configure is :$1"
tc32-elf-objcopy -v -O binary $2.elf  $1.bin
../tl_check_fw2.exe  $1.bin
../rf4ceOtaHex.exe $1.bin $1.HEX
echo "**************** end of post build ******************"
