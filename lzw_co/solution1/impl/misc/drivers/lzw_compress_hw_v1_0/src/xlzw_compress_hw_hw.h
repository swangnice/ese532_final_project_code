// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 4  - ap_continue (Read/Write/SC)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - enable ap_done interrupt (Read/Write)
//        bit 1  - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - ap_done (COR/TOW)
//        bit 1  - ap_ready (COR/TOW)
//        others - reserved
// 0x10 : Data signal of s1
//        bit 31~0 - s1[31:0] (Read/Write)
// 0x14 : Data signal of s1
//        bit 31~0 - s1[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of length_r
//        bit 31~0 - length_r[31:0] (Read/Write)
// 0x20 : Data signal of length_r
//        bit 31~0 - length_r[63:32] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of is_dup
//        bit 31~0 - is_dup[31:0] (Read/Write)
// 0x2c : Data signal of is_dup
//        bit 31~0 - is_dup[63:32] (Read/Write)
// 0x30 : reserved
// 0x34 : Data signal of dup_index
//        bit 31~0 - dup_index[31:0] (Read/Write)
// 0x38 : Data signal of dup_index
//        bit 31~0 - dup_index[63:32] (Read/Write)
// 0x3c : reserved
// 0x40 : Data signal of temp_out_buffer
//        bit 31~0 - temp_out_buffer[31:0] (Read/Write)
// 0x44 : Data signal of temp_out_buffer
//        bit 31~0 - temp_out_buffer[63:32] (Read/Write)
// 0x48 : reserved
// 0x4c : Data signal of temp_out_buffer_size
//        bit 31~0 - temp_out_buffer_size[31:0] (Read/Write)
// 0x50 : Data signal of temp_out_buffer_size
//        bit 31~0 - temp_out_buffer_size[63:32] (Read/Write)
// 0x54 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL                   0x00
#define XLZW_COMPRESS_HW_CONTROL_ADDR_GIE                       0x04
#define XLZW_COMPRESS_HW_CONTROL_ADDR_IER                       0x08
#define XLZW_COMPRESS_HW_CONTROL_ADDR_ISR                       0x0c
#define XLZW_COMPRESS_HW_CONTROL_ADDR_S1_DATA                   0x10
#define XLZW_COMPRESS_HW_CONTROL_BITS_S1_DATA                   64
#define XLZW_COMPRESS_HW_CONTROL_ADDR_LENGTH_R_DATA             0x1c
#define XLZW_COMPRESS_HW_CONTROL_BITS_LENGTH_R_DATA             64
#define XLZW_COMPRESS_HW_CONTROL_ADDR_IS_DUP_DATA               0x28
#define XLZW_COMPRESS_HW_CONTROL_BITS_IS_DUP_DATA               64
#define XLZW_COMPRESS_HW_CONTROL_ADDR_DUP_INDEX_DATA            0x34
#define XLZW_COMPRESS_HW_CONTROL_BITS_DUP_INDEX_DATA            64
#define XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_DATA      0x40
#define XLZW_COMPRESS_HW_CONTROL_BITS_TEMP_OUT_BUFFER_DATA      64
#define XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_SIZE_DATA 0x4c
#define XLZW_COMPRESS_HW_CONTROL_BITS_TEMP_OUT_BUFFER_SIZE_DATA 64

