// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XLZW_COMPRESS_H
#define XLZW_COMPRESS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xlzw_compress_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XLzw_compress_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XLzw_compress;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XLzw_compress_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XLzw_compress_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XLzw_compress_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XLzw_compress_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XLzw_compress_Initialize(XLzw_compress *InstancePtr, u16 DeviceId);
XLzw_compress_Config* XLzw_compress_LookupConfig(u16 DeviceId);
int XLzw_compress_CfgInitialize(XLzw_compress *InstancePtr, XLzw_compress_Config *ConfigPtr);
#else
int XLzw_compress_Initialize(XLzw_compress *InstancePtr, const char* InstanceName);
int XLzw_compress_Release(XLzw_compress *InstancePtr);
#endif

void XLzw_compress_Start(XLzw_compress *InstancePtr);
u32 XLzw_compress_IsDone(XLzw_compress *InstancePtr);
u32 XLzw_compress_IsIdle(XLzw_compress *InstancePtr);
u32 XLzw_compress_IsReady(XLzw_compress *InstancePtr);
void XLzw_compress_Continue(XLzw_compress *InstancePtr);
void XLzw_compress_EnableAutoRestart(XLzw_compress *InstancePtr);
void XLzw_compress_DisableAutoRestart(XLzw_compress *InstancePtr);

void XLzw_compress_Set_s1(XLzw_compress *InstancePtr, u64 Data);
u64 XLzw_compress_Get_s1(XLzw_compress *InstancePtr);
void XLzw_compress_Set_length_r(XLzw_compress *InstancePtr, u32 Data);
u32 XLzw_compress_Get_length_r(XLzw_compress *InstancePtr);
void XLzw_compress_Set_out_code(XLzw_compress *InstancePtr, u64 Data);
u64 XLzw_compress_Get_out_code(XLzw_compress *InstancePtr);
void XLzw_compress_Set_out_len(XLzw_compress *InstancePtr, u64 Data);
u64 XLzw_compress_Get_out_len(XLzw_compress *InstancePtr);

void XLzw_compress_InterruptGlobalEnable(XLzw_compress *InstancePtr);
void XLzw_compress_InterruptGlobalDisable(XLzw_compress *InstancePtr);
void XLzw_compress_InterruptEnable(XLzw_compress *InstancePtr, u32 Mask);
void XLzw_compress_InterruptDisable(XLzw_compress *InstancePtr, u32 Mask);
void XLzw_compress_InterruptClear(XLzw_compress *InstancePtr, u32 Mask);
u32 XLzw_compress_InterruptGetEnabled(XLzw_compress *InstancePtr);
u32 XLzw_compress_InterruptGetStatus(XLzw_compress *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
