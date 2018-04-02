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

#define RPC_STATIC_ENDPOINT L"HsaSampleRpcEndpoint"

#include "RpcInterface_h.h"
#include "RpcClientApi.h"


/// <summary>
/// Client side RPC implementation
/// </summary>
class RpcClient sealed
{
public:
	~RpcClient();
	__int64 Initialize();
    __int64 StartMeteringAndWaitForStop(AudioDataCallback callback, void* context);
	__int64 StopMetering();
	void DoAudioCallback(unsigned long length, byte* data);
	int CallbackCount;
private:
	AudioDataCallback m_audioCallback;
	void* m_audioCallbackContext;
    handle_t hRpcBinding;
    PCONTEXT_HANDLE_TYPE phContext;
};
