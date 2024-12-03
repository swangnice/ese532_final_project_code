

## Common Instructions
For Mac, receive msgs from serial port:
``` bash
screen /dev/tty.usbserial-1234_oj11 115200
```
For Mac, transmit file from host to ssh device:
``` bash
scp -r /Users/oushiha/Documents/ESE5320/ese532_final_project_code/encoder root@169.254.123.001:/media/sd-mmcblk0p1
```
For Mac, transmit LittlePrince.txt(any test file) via UDP:
``` bash
./client_mac -i 169.254.123.001 -f LittlePrince.txt
```

For Ultra96, assign a fixed IP:
``` bash
ifconfig eth0 169.254.123.24 netmask 255.255.0.0
```

Build SSH connection with board:
``` bash
ssh root@169.254.123.24
```



