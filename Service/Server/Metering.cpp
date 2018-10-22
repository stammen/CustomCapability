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

#include "stdafx.h"
#include "Metering.h"
#include <assert.h>  
#include <iostream>
#include <functional>

#include "RpcInterface_h.h" 
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;

using namespace RpcServer;
using namespace std::placeholders;


//
// ctor for Metering
//
Metering::Metering()
{
	m_bHasMicPermission = true;
}

//
// dtor for Metering
//
Metering::~Metering()
{
  
}

void Metering::OnAccessChanged(Platform::Object^ sender, Windows::Devices::Enumeration::DeviceAccessChangedEventArgs^ e)
{
	m_bHasMicPermission = e->Status == DeviceAccessStatus::Allowed;
	if (m_bHasMicPermission)
	{
		m_audioInput->Start();
	}
	else
	{
		m_audioInput->Stop();
	}
	std::cout << "Metering::OnAccessChanged " << m_bHasMicPermission << std::endl;
}


void Metering::StartMetering(
    _In_ __int64 context)
{
	std::cout << "Metering::StartMetering" << std::endl;

	m_deviceAccessInformation = DeviceAccessInformation::CreateFromDeviceClass(DeviceClass::AudioCapture);
	m_deviceAccessInformation->AccessChanged += ref new TypedEventHandler<DeviceAccessInformation ^, DeviceAccessChangedEventArgs ^>(std::bind(&Metering::OnAccessChanged, this, _1, _2));

    stopMeteringRequested = false;
	m_context = context;
	
	if (m_audioInput)
	{
		StopMetering();
	}

	m_audioInput = std::make_unique<AudioInput>(this);
	m_audioInput->Start();
	while (true)
	{
		if (stopMeteringRequested || ShutdownRequested)
		{
			break;
		}
		std::shared_ptr<std::vector<unsigned char>> data;
		while(m_sampleQueue.try_pop(data))
		{
			if (m_bHasMicPermission)
			{
				MeteringDataEvent((unsigned long)data->size(), data->data(), m_context);
			}
		}
		Sleep(1); // probably a better way to do this
	}
}

void Metering::StopMetering()
{
	std::cout << "Metering::StopMetering" << std::endl;

	stopMeteringRequested = true;
	if (m_audioInput)
	{
		m_audioInput->Stop();
		m_audioInput.reset();
	}
}


void Metering::OnAudioInput(std::shared_ptr<std::vector<unsigned char>> data)
{
	//std::cout << "Metering::OnAudioInput:" << data->size() << std::endl;
	if (!stopMeteringRequested && !ShutdownRequested)
	{
		m_sampleQueue.push(data);
	}
}


