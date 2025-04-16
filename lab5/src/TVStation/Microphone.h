#pragma once
#include <RtAudio.h>
#include <functional>
#include <thread>
#include <vector>

class Microphone
{
public:
	using AudioHandler = std::function<void(const std::vector<int16_t>&, uint32_t sampleRate)>;

	explicit Microphone(AudioHandler audioHandler,
		const uint32_t sampleRate = 44100,
		const uint32_t frameSize = 512,
		const uint32_t channels = 1)
		: m_audioHandler(std::move(audioHandler))
		  , m_sampleRate(sampleRate)
		  , m_frameSize(frameSize)
		  , m_channels(channels)
	{
		try
		{
			SetupAudioStream();
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(std::string("Audio initialization failed: ") + e.what());
		}
	}

	// копирование

	void Run()
	{
		m_running = true;
		m_audio.startStream();

		while (m_running)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	void Stop()
	{
		m_running = false;
		if (m_audio.isStreamRunning())
		{
			m_audio.stopStream();
		}
	}

	~Microphone()
	{
		if (m_audio.isStreamOpen())
		{
			m_audio.closeStream();
		}
	}

private:
	void SetupAudioStream()
	{
		RtAudio::StreamParameters params;
		params.deviceId = m_audio.getDefaultInputDevice();
		params.nChannels = m_channels;
		params.firstChannel = 0;

		RtAudio::StreamOptions options;
		options.flags = RTAUDIO_MINIMIZE_LATENCY;
		options.streamName = "Microphone";

		const RtAudioCallback callback = [](void* outputBuffer, void* inputBuffer,
			const unsigned int nFrames,
			double streamTime,
			RtAudioStreamStatus status,
			void* userData) -> int {
			const auto* self = static_cast<Microphone*>(userData);
			return self->AudioCallback(inputBuffer, nFrames);
		};

		unsigned int bufferFrames = m_frameSize;
		m_audio.openStream(nullptr, &params, RTAUDIO_SINT16,
			m_sampleRate, &bufferFrames,
			callback, this, &options);
	}

	int AudioCallback(const void* inputBuffer, const unsigned int nFrames) const
	{
		const auto* samples = static_cast<const int16_t*>(inputBuffer);
		const std::vector audioData(samples, samples + nFrames * m_channels);

		m_audioHandler(audioData, m_sampleRate);
		return 0;
	}

	RtAudio m_audio;
	AudioHandler m_audioHandler;
	uint32_t m_sampleRate;
	uint32_t m_frameSize;
	uint32_t m_channels;
	std::atomic<bool> m_running{ false };
};