############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project final
set_top lzw_compress
add_files Server/lzw.cpp
add_files Server/lzw.h
open_solution "solution1" -flow_target vitis
set_part {xczu3eg-sbva484-1-i}
create_clock -period 150MHz -name default
config_export -format xo -output /home1/s/swang01/Documents/ese532_code/lzw_compress.xo -rtl verilog
#source "./final/solution1/directives.tcl"
export_design -rtl verilog -format xo -output /mnt/castor/seas_home/s/swang01/Documents/ese532_code/lzw_compress.xo