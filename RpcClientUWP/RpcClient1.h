#pragma once

#include "RpcClientApi.h"

namespace RpcClientUWP
{
	public delegate void OnAudioInputHandler(Windows::Storage::Streams::IBuffer^ buffer);

    public ref class RpcClient1 sealed
    {
    public:
		RpcClient1();
		event OnAudioInputHandler^ OnAudioInput;

		Windows::Foundation::IAsyncOperation<bool>^ Open();
		Windows::Foundation::IAsyncOperation<bool>^ Start();
		Windows::Foundation::IAsyncOperation<bool>^ Stop();
		Windows::Foundation::IAsyncOperation<bool>^ Close();
		void SendAudioInput(Windows::Storage::Streams::IBuffer^ buffer);

	private:
		RPC_CLIENT_HANDLE m_rpcclient;

    };
}
