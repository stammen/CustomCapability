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
#include "pch.h"

#pragma push_macro("WINAPI_FAMILY")
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#include "RpcClient.h"
#pragma pop_macro("WINAPI_FAMILY")

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <SDKDDKVer.h>

#include <windows.h>
#include <windows.h>
#include <Rpc.h>
#include <rpcdce.h>

//
// Open metering service RPC endpoint
//
__int64 RpcClient::Initialize()
{
    RPC_STATUS status;
    RPC_WSTR pszStringBinding = nullptr;

    status = RpcStringBindingCompose(
        NULL,
        reinterpret_cast<RPC_WSTR>(L"ncalrpc"),
        NULL,
        reinterpret_cast<RPC_WSTR>(RPC_STATIC_ENDPOINT),
        NULL,
        &pszStringBinding);

    if (status)
    {
        goto error_status;
    }

    status = RpcBindingFromStringBinding(
        pszStringBinding,
        &hRpcBinding);

    if (status)
    {
        goto error_status;
    }

    status = RpcStringFree(&pszStringBinding);

    if (status)
    {
        goto error_status;
    }

    RpcTryExcept
    {
        ::RemoteOpen(hRpcBinding, &phContext);
    }
    RpcExcept(1)
    {
        status = RpcExceptionCode();
    }
    RpcEndExcept

error_status :

    return status;
}

//
// Make RPC call to start metering. This is a blocking call and 
// will return only after StopMetering is called.
//
__int64 RpcClient::StartMeteringAndWaitForStop(AudioDataCallback callback, void* context)
{
    __int64 ulCode = 0;
    CallbackCount = 0;
	m_audioCallback = callback;
	m_audioCallbackContext = context;
    RpcTryExcept
    {
        ::StartMetering(phContext, (__int64)this);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
    }
    RpcEndExcept

    return ulCode;
}

//
// Make rpc call StopMetering
//
__int64 RpcClient::StopMetering()
{
    __int64 ulCode = 0;
    RpcTryExcept
    {
        ::StopMetering(phContext);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
    }
    RpcEndExcept

    return ulCode;
}

//
// dtor
//
RpcClient::~RpcClient()
{
    RPC_STATUS status;

    if (hRpcBinding != NULL)
    {
		::RemoteClose(&phContext);
        status = RpcBindingFree(&hRpcBinding);
        hRpcBinding = NULL;
    }
}

void RpcClient::DoAudioCallback(unsigned long length, byte* data)
{
	if (m_audioCallback != nullptr)
	{
		m_audioCallback(length, data, m_audioCallbackContext);
	}
}

//
// Metering rpc callback
//
void MeteringDataEvent(unsigned long length, byte* data, __int64 context)
{
    RpcClient* client = static_cast<RpcClient*>((PVOID)context);
    ++client->CallbackCount;
	client->DoAudioCallback(length, data);
}

///******************************************************/
///*         MIDL allocate and free                     */
///******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}