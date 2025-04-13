#pragma once
#include "Microphone.h"
#include "Server.h"
#include "VideoCamera.h"
#include "../TVReceiver/Client.h"

class TVStation : public Server
{
public:
	TVStation(asio::io_context& io, const uint16_t port)
		: Server(io, port)
		  , m_camera([this](const auto& frame) { HandleFrame(frame); })
		  , m_microphone([this](const auto& audioData, uint32_t sampleRate) { HandleAudio(audioData, sampleRate); })
	{
		SetupAudioOutput();
	}

	void Run()
	{
		std::jthread videoThread([this] { m_camera.Run(); });
		std::jthread audioThread([this] { m_microphone.Run(); });
	}

private:
	void HandleFrame(cv::Mat const& frame)
	{
		cv::Mat resizedFrame;
		resize(frame, resizedFrame, cv::Size(640, 480));

		std::vector<uint8_t> buffer;
		buffer.reserve(BUFFER_SIZE);
		constexpr int quality = 70;
		const std::vector params{ cv::IMWRITE_JPEG_QUALITY, quality };
		imencode(".jpg", resizedFrame, buffer, params);

		Send(buffer.data(), buffer.size());
	}

	void HandleAudio(const std::vector<int16_t>& audioData, uint32_t sampleRate)
	{
		std::lock_guard lock(m_audioMutex);
		for (auto sample : audioData)
		{
			m_audioQueue.push(sample);
		}
	}

	void SetupAudioOutput()
	{
		RtAudio::StreamParameters outputParams;
		outputParams.deviceId = m_audioPlayer.getDefaultOutputDevice();
		outputParams.nChannels = 1;
		outputParams.firstChannel = 0;

		auto callback = [](void* outputBuffer, void* inputBuffer,
			unsigned int nFrames,
			double streamTime,
			RtAudioStreamStatus status,
			void* userData) -> int {
			auto* self = static_cast<TVStation*>(userData);
			return self->AudioOutputCallback(outputBuffer, nFrames);
		};

		unsigned int sampleRate = 44100;
		unsigned int bufferFrames = 512;

		RtAudio::StreamOptions options;
		options.flags = RTAUDIO_MINIMIZE_LATENCY;

		try
		{
			m_audioPlayer.openStream(
				&outputParams, // output parameters
				nullptr, // no input
				RTAUDIO_SINT16, // format
				sampleRate,
				&bufferFrames,
				callback,
				this,
				&options
				);
			m_audioPlayer.startStream();
		}
		catch (RtAudioError& e)
		{
			std::cerr << "Audio output error: " << e.getMessage() << std::endl;
		}
	}

	int AudioOutputCallback(void* outputBuffer, unsigned int nFrames)
	{
		std::lock_guard lock(m_audioMutex);
		auto* out = static_cast<int16_t*>(outputBuffer);

		for (unsigned int i = 0; i < nFrames; ++i)
		{
			if (!m_audioQueue.empty())
			{
				out[i] = m_audioQueue.front();
				m_audioQueue.pop();
			}
			else
			{
				out[i] = 0;
			}
		}

		return 0;
	}

private:
	VideoCamera m_camera;
	Microphone m_microphone;
	RtAudio m_audioPlayer;
	std::mutex m_audioMutex;
	std::queue<int16_t> m_audioQueue;
};


