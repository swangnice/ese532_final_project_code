// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xlzw_compress.h"

extern XLzw_compress_Config XLzw_compress_ConfigTable[];

XLzw_compress_Config *XLzw_compress_LookupConfig(u16 DeviceId) {
	XLzw_compress_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XLZW_COMPRESS_NUM_INSTANCES; Index++) {
		if (XLzw_compress_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XLzw_compress_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XLzw_compress_Initialize(XLzw_compress *InstancePtr, u16 DeviceId) {
	XLzw_compress_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XLzw_compress_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XLzw_compress_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

