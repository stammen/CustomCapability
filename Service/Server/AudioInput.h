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

#include <vector>
#include <memory>
#include <ppltasks.h>

class IAudioInputListener
{
public:
	virtual void OnAudioInput(std::shared_ptr<std::vector<unsigned char>> data) = 0;
};

class AudioInput {
public:
	AudioInput(IAudioInputListener* listener = nullptr);
	~AudioInput();
	Concurrency::task<bool> Start();
	void Stop();

private:
	void OnQuantumStarted(Windows::Media::Audio::AudioGraph^ graph, Platform::Object^ args);
	Windows::Media::Audio::AudioGraph^ m_audioGraph;
	Windows::Media::Audio::AudioFrameOutputNode^ m_frameOutputNode;
	Windows::Media::Audio::AudioDeviceInputNode^ m_deviceInputNode;
	IAudioInputListener* m_listener;
	unsigned int m_channels;
	unsigned int m_sampleSize;
};
