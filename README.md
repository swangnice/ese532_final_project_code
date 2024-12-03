

## Common Instructions
For Mac, receive msgs from serial port:
``` bash
screen /dev/tty.usbserial-1234_oj11 115200
```
For Mac, transmit file from host to ssh device:
``` bash
scp -r /Users/oushiha/Documents/ESE5320/ese532_final_project_code/host root@169.254.123.001:/media/sd-mmcblk0p1
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

chunk1:
05 40 20 06 90 6E 07 30 69 06 40 65 02 00 6F 06 60 20 06 10 20 06 20 6F 10 C0 63 06 F1 03 07 40 72 06 90 63 07 40 6F 07 20 2C 02 00 73 06 F0 20 07 40 68 06 10 74 11 F0 68 10

02 00 69 06 E0 73 06 90 64 06 50 20 06 F0 66 02 00 61 02 00 62 06 F1 0B 06 30 6F 10 20 74 07 20 69 06 30 74 06 F0 72 02 C0 20 07 30 6F 02 00 74 06 80 61 07 41 1E 06 81 06 06 70 72 06 F


chunk2:
0x000005f2 06 20 6F 06 10 20 06 30 6F 06 E0 73 07 40 72 06 90 63 07 40 6F 07 20 73 02 C0 20 07 70 68 06 50 74 11 30 72 02 00 66 07 20 6F 06 D0 20 11 50 65 02 00 69 10 60 69 06 41 1F 10 D1 1D 11 30 20 06 F0 75 07 40 73 12 30 65 11 00 61 06 E0 64 02 01 24 07 60 6F 07 41 1F 06 D0 79 07 30 65 06 C0 66 12 01 06 13 80 61 13 


0x000005f805 40 20  06 20 6F 06 10 20 06 30 6F 06 E0 73 07 40 72 06 90 63 07 40 6F 07 20 73 02 C0 20 07 70 68 06 50 74 11 50 72 02 00 66 07 20 6F 06 D0 20 11 70 65 02 00 69 10 80 69 06 41 21 10 F1 1F 11 50 20 06 F0 75 07 40 73 12 50 65 11 20 61 06 E0 64 02 01 26 07 60 6F 07 41 21 06 D0 79 07 30 65 06 C0 66 12 21 08