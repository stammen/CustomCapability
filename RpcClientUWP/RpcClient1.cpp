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
		__int64 retCode = RpcClientClose(m_rpcclient);
		return retCode == ERROR_SUCCESS;
	});
}

void AudioCallback(const unsigned int length, unsigned char* data, __int64 context)
{

}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Start()
{
	return create_async([this]
	{
		__int64 retCode = StartMeteringAndWaitForStop(m_rpcclient, AudioCallback);
		return retCode == ERROR_SUCCESS;
	});
}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Stop()
{
	return create_async([this]
	{
		__int64 retCode = StopMeteringData(m_rpcclient);
		return retCode == ERROR_SUCCESS;
	});
}
