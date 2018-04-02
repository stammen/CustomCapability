#include "pch.h"
#include "RpcClient1.h"
#include <ppltasks.h>

using namespace RpcClientUWP;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;

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

void RpcClient1::SendAudioInput(Windows::Storage::Streams::IBuffer^ buffer)
{
	OnAudioInput(buffer);
}

void AudioCallback(const unsigned int length, unsigned char* data, void* context)
{
	RpcClient1^ client = reinterpret_cast<RpcClient1^>(context);
	DataWriter ^writer = ref new DataWriter();
	writer->WriteBytes(Platform::ArrayReference<BYTE>(data, length));
	IBuffer ^buffer = writer->DetachBuffer();
	client->SendAudioInput(buffer);
}

Windows::Foundation::IAsyncOperation<bool>^ RpcClient1::Start()
{
	return create_async([this]
	{
		__int64 retCode = StartMeteringAndWaitForStop(m_rpcclient, AudioCallback, (void*)this);
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
