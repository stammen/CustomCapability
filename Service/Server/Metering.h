#include "stdafx.h"
#include "AudioInput.h"

#include <windows.h>
#include <concurrent_queue.h>

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex> 
#include <memory> 

namespace RpcServer
{
    class Metering : public IAudioInputListener
    {
    public:
        Metering();
        void StartMetering( __int64 context);
        void StopMetering();
        ~Metering();
		virtual void OnAudioInput(std::shared_ptr<std::vector<unsigned char>> data);

    private:
		__int64 m_context;
		std::unique_ptr<AudioInput> m_audioInput;
        volatile bool stopMeteringRequested = false;
		Concurrency::concurrent_queue < std::shared_ptr<std::vector<unsigned char>>> m_sampleQueue;
    };
}
