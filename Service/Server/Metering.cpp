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

#include "RpcInterface_h.h" 

using namespace RpcServer;


//
// ctor for Metering
//
Metering::Metering()
{
 
}

//
// dtor for Metering
//
Metering::~Metering()
{
  
}

static char* sHello = "hello";

void Metering::StartMetering(
    _In_ __int64 context)
{
	std::cout << "Metering::StartMetering" << std::endl;

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
			MeteringDataEvent((unsigned long)data->size(), data->data(), m_context);
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
		//MeteringDataEvent(data->size(), data->data(), m_context);
	}
}


