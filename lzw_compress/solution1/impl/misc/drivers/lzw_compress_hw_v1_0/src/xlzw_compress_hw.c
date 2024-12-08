// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlzw_compress_hw.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLzw_compress_hw_CfgInitialize(XLzw_compress_hw *InstancePtr, XLzw_compress_hw_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLzw_compress_hw_Start(XLzw_compress_hw *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLzw_compress_hw_IsDone(XLzw_compress_hw *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLzw_compress_hw_IsIdle(XLzw_compress_hw *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLzw_compress_hw_IsReady(XLzw_compress_hw *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLzw_compress_hw_Continue(XLzw_compress_hw *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL, Data | 0x10);
}

void XLzw_compress_hw_EnableAutoRestart(XLzw_compress_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XLzw_compress_hw_DisableAutoRestart(XLzw_compress_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_AP_CTRL, 0);
}

void XLzw_compress_hw_Set_s1(XLzw_compress_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_S1_DATA, (u32)(Data));
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_S1_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_hw_Get_s1(XLzw_compress_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_S1_DATA);
    Data += (u64)XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_S1_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_hw_Set_length_r(XLzw_compress_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_LENGTH_R_DATA, (u32)(Data));
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_LENGTH_R_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_hw_Get_length_r(XLzw_compress_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_LENGTH_R_DATA);
    Data += (u64)XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_LENGTH_R_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_hw_Set_is_dup(XLzw_compress_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IS_DUP_DATA, (u32)(Data));
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IS_DUP_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_hw_Get_is_dup(XLzw_compress_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IS_DUP_DATA);
    Data += (u64)XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IS_DUP_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_hw_Set_dup_index(XLzw_compress_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_DUP_INDEX_DATA, (u32)(Data));
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_DUP_INDEX_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_hw_Get_dup_index(XLzw_compress_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_DUP_INDEX_DATA);
    Data += (u64)XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_DUP_INDEX_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_hw_Set_temp_out_buffer(XLzw_compress_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_DATA, (u32)(Data));
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_hw_Get_temp_out_buffer(XLzw_compress_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_DATA);
    Data += (u64)XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_hw_Set_temp_out_buffer_size(XLzw_compress_hw *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_SIZE_DATA, (u32)(Data));
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_SIZE_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_hw_Get_temp_out_buffer_size(XLzw_compress_hw *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_SIZE_DATA);
    Data += (u64)XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_TEMP_OUT_BUFFER_SIZE_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_hw_InterruptGlobalEnable(XLzw_compress_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_GIE, 1);
}

void XLzw_compress_hw_InterruptGlobalDisable(XLzw_compress_hw *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_GIE, 0);
}

void XLzw_compress_hw_InterruptEnable(XLzw_compress_hw *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IER);
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IER, Register | Mask);
}

void XLzw_compress_hw_InterruptDisable(XLzw_compress_hw *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IER);
    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IER, Register & (~Mask));
}

void XLzw_compress_hw_InterruptClear(XLzw_compress_hw *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_hw_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_ISR, Mask);
}

u32 XLzw_compress_hw_InterruptGetEnabled(XLzw_compress_hw *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_IER);
}

u32 XLzw_compress_hw_InterruptGetStatus(XLzw_compress_hw *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLzw_compress_hw_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_HW_CONTROL_ADDR_ISR);
}

