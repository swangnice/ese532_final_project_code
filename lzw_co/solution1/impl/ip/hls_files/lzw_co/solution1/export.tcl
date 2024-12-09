############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project lzw_co
set_top lzw_compress_hw
add_files Server/lzw.h
add_files Server/lzw.cpp
add_files -tb test.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vitis
set_part {xczu3eg-sbva484-1-i}
create_clock -period 150MHz -name default
config_export -format xo -output /home1/s/swang01/Documents/ese532_code/lzw_compress_hw.xo -rtl verilog
config_interface -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512 -m_axi_offset slave
config_rtl -register_reset_num 3
source "./lzw_co/solution1/directives.tcl"
export_design -rtl verilog -format xo -output /home1/s/swang01/Documents/ese532_code/lzw_compress_hw.xo
