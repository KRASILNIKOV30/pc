#pragma once
#include "Microphone.h"
#include "Server.h"
#include "VideoCamera.h"
#include "../PacketType.h"
#include "../TVReceiver/Client.h"

class TVStation : public Server
{
public:
	TVStation(asio::io_context& io, const uint16_t port)
		: Server(io, port)
		  , m_camera([this](const auto& frame) { HandleFrame(frame); })
		  , m_microphone([this](const auto& audioData, uint32_t sampleRate) { HandleAudio(audioData, sampleRate); })
	{
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
		buffer.insert(buffer.begin(), VIDEO_PACKET);

		Send(buffer.data(), buffer.size());
	}

	void HandleAudio(const std::vector<int16_t>& audioData, uint32_t sampleRate)
	{
		const size_t bytesCount = audioData.size() * sizeof(int16_t);
		std::vector<uint8_t> packet;
		packet.reserve(1 + bytesCount);
		packet.push_back(AUDIO_PACKET);

		for (const auto sample : audioData)
		{
			auto networkSample = htons(static_cast<uint16_t>(sample));
			const auto bytes = reinterpret_cast<uint8_t*>(&networkSample);
			packet.insert(packet.end(), bytes, bytes + sizeof(uint16_t));
		}

		Send(packet.data(), packet.size());
	}

private:
	VideoCamera m_camera;
	Microphone m_microphone;
};


