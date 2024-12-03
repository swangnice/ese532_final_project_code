

## Common Instructions
For Mac, receive msgs from serial port:
``` bash
screen /dev/tty.usbserial-1234_oj11 115200
```
For Mac, transmit file from host to ssh device:
``` bash
scp -r /Users/oushiha/Documents/ESE5320/ese532_final_project_code/host root@169.254.128.222:/media/sd-mmcblk0p1
```
For Mac, transmit LittlePrince.txt(any test file) via UDP:
``` bash
./client_mac -i 169.254.128.222 -f LittlePrince.txt
```





