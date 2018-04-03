/*
Copyright 2017 Google Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "AudioInput.h"

#include <functional>
#include <ppltasks.h>
#include <robuffer.h> 
#include <MemoryBuffer.h>


using namespace std::placeholders;

using namespace Concurrency;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Media::Audio;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Render;
using namespace Windows::Storage::Streams;

AudioInput::AudioInput(IAudioInputListener* listener)
	:m_listener(listener)
{

}

AudioInput::~AudioInput()
{
	if (m_audioGraph)
	{
		Stop();
		m_audioGraph = nullptr;
	}
}

void AudioInput::Stop()
{
	if (m_audioGraph)
	{
		m_audioGraph->Stop();
	}
}

Concurrency::task<bool> AudioInput::Start()
{
	return create_task([this] {
		AudioGraphSettings^ settings = ref new AudioGraphSettings(AudioRenderCategory::Speech);
		//settings->EncodingProperties = AudioEncodingProperties::CreatePcm(48000, 2, 32);

		settings->QuantumSizeSelectionMode = QuantumSizeSelectionMode::LowestLatency;
		return concurrency::create_task(AudioGraph::CreateAsync(settings));
	}, task_continuation_context::use_arbitrary()).then([this](CreateAudioGraphResult^ result)
	{
		auto r = result->Status;
		if (result->Status != AudioGraphCreationStatus::Success)
		{
			throw ref new Platform::Exception(-1, L"Unable to create audio graph");
		}

		m_audioGraph = result->Graph;

		// Create the FrameInputNode at the same format as the graph, 
		// except explicitly set mono channel and 16K sample rate to match Google Assistant audio
		AudioEncodingProperties^ nodeEncodingProperties = m_audioGraph->EncodingProperties;
		nodeEncodingProperties->ChannelCount = 1;
		nodeEncodingProperties->SampleRate = 16000;
		m_frameOutputNode = m_audioGraph->CreateFrameOutputNode(nodeEncodingProperties);
		m_audioGraph->QuantumStarted += ref new TypedEventHandler<AudioGraph^, Platform::Object^>(std::bind(&AudioInput::OnQuantumStarted, this, _1, _2));

		// Create a device input node using the default audio input device
		return m_audioGraph->CreateDeviceInputNodeAsync(MediaCategory::Speech);
	}, task_continuation_context::use_arbitrary()).then([this](CreateAudioDeviceInputNodeResult^ result)
	{
		if (result->Status != AudioDeviceNodeCreationStatus::Success)
		{
			throw ref new Platform::Exception(-1, L"Unable to create audio capture device");
		}

		m_deviceInputNode = result->DeviceInputNode;
		m_deviceInputNode->AddOutgoingConnection(m_frameOutputNode);

	}, task_continuation_context::use_arbitrary()).then([this](task<void> t)
	{
		try
		{
			t.get();
			auto sampleRate = m_frameOutputNode->EncodingProperties->SampleRate;
			m_channels = m_frameOutputNode->EncodingProperties->ChannelCount;
			m_sampleSize = (m_frameOutputNode->EncodingProperties->BitsPerSample) / 8;

			m_audioGraph->Start();
			return true;
		}
		catch (Platform::Exception^ ex)
		{
			OutputDebugString(ex->Message->Data());
		}
		catch (task_canceled)
		{
		}

		return false;
	});
}

void AudioInput::OnQuantumStarted(Windows::Media::Audio::AudioGraph^ graph, Platform::Object^ args)
{
	auto frame = m_frameOutputNode->GetFrame();
	auto frameBuffer = frame->LockBuffer(AudioBufferAccessMode::Read);
	auto reference = frameBuffer->CreateReference();

	Microsoft::WRL::ComPtr<IMemoryBufferByteAccess> byteAccess;
	if (SUCCEEDED(reinterpret_cast<IUnknown*>(reference)->QueryInterface(IID_PPV_ARGS(&byteAccess))))
	{
		// Get a pointer to the sample buffer
		byte* data;
		unsigned int capacity;
		byteAccess->GetBuffer(&data, &capacity);

		if (capacity > 0)
		{
			std::shared_ptr<std::vector<unsigned char>> audio_data(new std::vector<unsigned char>());
			auto numFrames = capacity / m_sampleSize / m_channels;
			audio_data->resize(numFrames * sizeof(short));
			short* output = (short*)audio_data->data();
			float* input = (float*)data;
			if (numFrames > 0)
			{
				// convert samples from float to short
				for (unsigned int i = 0; i < numFrames; ++i)
				{
					float temp = input[i];
					if (temp >= 1.0)
					{
						output[i] = 32767;
					}
					else if (temp < -1.0)
					{
						output[i] = -32768;
					}
					else
					{
						output[i] = temp * 32768.0f;
					}
				}

				if (m_listener)
				{
					m_listener->OnAudioInput(audio_data);
				}
			}
		}
	}
}
