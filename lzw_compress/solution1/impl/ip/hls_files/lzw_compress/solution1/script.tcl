############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project lzw_compress
set_top lzw_compress_hw
add_files Server/lzw.cpp
add_files Server/lzw.h
add_files -tb test.cpp
open_solution "solution1" -flow_target vitis
set_part {xczu3eg-sbva484-1-i}
create_clock -period 150MHz -name default
#source "./lzw_compress/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format xo -output /home1/s/swang01/Documents/lzw_compress_hw.xo