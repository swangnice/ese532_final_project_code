// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlzw_compress.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLzw_compress_CfgInitialize(XLzw_compress *InstancePtr, XLzw_compress_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLzw_compress_Start(XLzw_compress *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLzw_compress_IsDone(XLzw_compress *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLzw_compress_IsIdle(XLzw_compress *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLzw_compress_IsReady(XLzw_compress *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLzw_compress_Continue(XLzw_compress *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL, Data | 0x10);
}

void XLzw_compress_EnableAutoRestart(XLzw_compress *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XLzw_compress_DisableAutoRestart(XLzw_compress *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_AP_CTRL, 0);
}

void XLzw_compress_Set_s1(XLzw_compress *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_S1_DATA, (u32)(Data));
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_S1_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_Get_s1(XLzw_compress *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_S1_DATA);
    Data += (u64)XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_S1_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_Set_length_r(XLzw_compress *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_LENGTH_R_DATA, Data);
}

u32 XLzw_compress_Get_length_r(XLzw_compress *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_LENGTH_R_DATA);
    return Data;
}

void XLzw_compress_Set_out_code(XLzw_compress *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_CODE_DATA, (u32)(Data));
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_CODE_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_Get_out_code(XLzw_compress *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_CODE_DATA);
    Data += (u64)XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_CODE_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_Set_out_len(XLzw_compress *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_LEN_DATA, (u32)(Data));
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_LEN_DATA + 4, (u32)(Data >> 32));
}

u64 XLzw_compress_Get_out_len(XLzw_compress *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_LEN_DATA);
    Data += (u64)XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_OUT_LEN_DATA + 4) << 32;
    return Data;
}

void XLzw_compress_InterruptGlobalEnable(XLzw_compress *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_GIE, 1);
}

void XLzw_compress_InterruptGlobalDisable(XLzw_compress *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_GIE, 0);
}

void XLzw_compress_InterruptEnable(XLzw_compress *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_IER);
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_IER, Register | Mask);
}

void XLzw_compress_InterruptDisable(XLzw_compress *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_IER);
    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_IER, Register & (~Mask));
}

void XLzw_compress_InterruptClear(XLzw_compress *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLzw_compress_WriteReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_ISR, Mask);
}

u32 XLzw_compress_InterruptGetEnabled(XLzw_compress *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_IER);
}

u32 XLzw_compress_InterruptGetStatus(XLzw_compress *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLzw_compress_ReadReg(InstancePtr->Control_BaseAddress, XLZW_COMPRESS_CONTROL_ADDR_ISR);
}

