#include "pch.h"
#include "RpcClient1.h"
#include <ppltasks.h>

using namespace RpcClientUWP;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;

RpcClient1::RpcClient1()
{
}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Open()
{
	return create_async([this]
	{
		__int64 retCode = RpcClientInitialize(&m_rpcclient);
		return retCode == ERROR_SUCCESS;
	});
}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Close()
{
	return create_async([this]
	{
		__int64 retCode = StopMeteringData(m_rpcclient);
		return retCode == ERROR_SUCCESS;
	});
}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Start()
{
	return create_async([this]
	{
		__int64 retCode = RpcClientInitialize(&m_rpcclient);
		return retCode == ERROR_SUCCESS;
	});
}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Stop()
{
	return create_async([this]
	{
		__int64 retCode = RpcClientInitialize(&m_rpcclient);
		return retCode == ERROR_SUCCESS;
	});
}
