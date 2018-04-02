#pragma once

#include "RpcClientApi.h"

namespace RpcClientUWP
{
    public ref class RpcClient1 sealed
    {
    public:
		RpcClient1();

		Windows::Foundation::IAsyncOperation<bool>^ Open();
		Windows::Foundation::IAsyncOperation<bool>^ Start();
		Windows::Foundation::IAsyncOperation<bool>^ Stop();
		Windows::Foundation::IAsyncOperation<bool>^ Close();

	private:
		RPC_CLIENT_HANDLE m_rpcclient;

    };
}
