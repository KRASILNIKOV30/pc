#pragma once
#include <mutex>
#include <queue>
#include <RtAudio.h>

constexpr int SAMPLE_RATE = 48000;

class AudioPlayer
{
public:
	AudioPlayer()
	{
		Init();
	}

	void PushData(const std::vector<int16_t>& audioData)
	{
		std::lock_guard lock(m_mutex);
		for (auto sample : audioData)
		{
			m_dataQueue.push(sample);
		}
	}

private:
	void Init()
	{
		RtAudio::StreamParameters outputParams;
		outputParams.deviceId = m_player.getDefaultOutputDevice();
		outputParams.nChannels = 1;
		outputParams.firstChannel = 0;

		auto callback = [](void* outputBuffer, void* inputBuffer,
			const unsigned int nFrames,
			double streamTime,
			RtAudioStreamStatus status,
			void* userData) -> int {
			auto* self = static_cast<AudioPlayer*>(userData);
			return self->AudioOutputCallback(outputBuffer, nFrames);
		};

		unsigned int bufferFrames = 512;
		RtAudio::StreamOptions options;
		options.flags = RTAUDIO_MINIMIZE_LATENCY;

		try
		{
			m_player.openStream(
				&outputParams,
				nullptr,
				RTAUDIO_SINT16,
				SAMPLE_RATE,
				&bufferFrames,
				callback,
				this,
				&options
				);
			m_player.startStream();
		}
		catch (RtAudioError& e)
		{
			std::cerr << "Audio output error: " << e.getMessage() << std::endl;
		}
	}

	int AudioOutputCallback(void* outputBuffer, unsigned int nFrames)
	{
		// предусмотреть переполнение
		std::lock_guard lock(m_mutex);
		auto* out = static_cast<int16_t*>(outputBuffer);

		for (unsigned int i = 0; i < nFrames; ++i)
		{
			if (!m_dataQueue.empty())
			{
				out[i] = m_dataQueue.front();
				m_dataQueue.pop();
			}
			else
			{
				out[i] = 0;
			}
		}

		return 0;
	}

private:
	RtAudio m_player;
	std::mutex m_mutex;
	std::queue<int16_t> m_dataQueue;
};



