//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

//
// Exported API's for RPC client that can be used in
// Win32/UWP app. This can be also consumed in managed 
// code using PInvoke
//
extern "C" {
typedef void * RPC_CLIENT_HANDLE;
typedef void(*AudioDataCallback) (const unsigned int length, unsigned char* data, void* context);

// Opens metering RPC endpoint
__declspec(dllexport)
__int64 RpcClientInitialize(RPC_CLIENT_HANDLE* RpcClientHandle);

// Starts metering service and blocks till metering is stopped
__declspec(dllexport)
__int64 StartMeteringAndWaitForStop(RPC_CLIENT_HANDLE RpcClientHandle, AudioDataCallback callback, void* context);

// Sends stop metering to RPC Service
__declspec(dllexport)
__int64 StopMeteringData(RPC_CLIENT_HANDLE RpcClientHandle);

// Retrieve the metering callback count for the current metering session
__declspec(dllexport)
__int64 GetCallbackCount(RPC_CLIENT_HANDLE RpcClientHandle, __int64* MeteringData);

// Closes metering RPC endpoint
__declspec(dllexport)
__int64 RpcClientClose(RPC_CLIENT_HANDLE RpcClientHandle);
}