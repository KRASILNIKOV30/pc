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
		m_startTime.store(steady_clock::now());
		std::jthread videoThread([this] { m_camera.Run(); });
		std::jthread audioThread([this] { m_microphone.Run(); });
	}

private:
	void HandleFrame(cv::Mat const& frame)
	{
		const auto timestamp = GetTimestamp();
		cv::Mat resizedFrame;
		// исправить размер (исправлено)
		resize(frame, resizedFrame, cv::Size(frame.cols / 2, frame.rows / 2));

		std::vector<uint8_t> buffer;
		buffer.reserve(BUFFER_SIZE);
		constexpr int quality = 70;
		const std::vector params{ cv::IMWRITE_JPEG_QUALITY, quality };
		imencode(".jpg", resizedFrame, buffer, params);

		const MediaHeader header{ VIDEO_PACKET, timestamp, m_videoSequence++ };
		std::vector<uint8_t> packet(sizeof(header) + buffer.size());
		memcpy(packet.data(), &header, sizeof(header));
		memcpy(packet.data() + sizeof(header), buffer.data(), buffer.size());

		Send(packet.data(), packet.size());
	}

	void HandleAudio(const std::vector<int16_t>& audioData, uint32_t sampleRate)
	{
		const auto timestamp = GetTimestamp();
		// синхронизация
		const size_t bytesCount = audioData.size() * sizeof(int16_t);
		const MediaHeader header{ 0x01, timestamp, m_audioSequence++ };
		std::vector<uint8_t> packet(sizeof(header) + bytesCount);
		memcpy(packet.data(), &header, sizeof(header));

		auto dest = packet.data() + sizeof(header);
		for (const auto sample : audioData)
		{
			constexpr auto step = sizeof(uint16_t);
			auto networkSample = htons(sample);
			const auto bytes = reinterpret_cast<uint8_t*>(&networkSample);
			memcpy(dest, bytes, step);
			dest += step;
		}

		Send(packet.data(), packet.size());
	}

	[[nodiscard]] uint64_t GetTimestamp() const
	{
		return duration_cast<microseconds>(steady_clock::now() - m_startTime.load()).count();
	}

private:
	VideoCamera m_camera;
	Microphone m_microphone;
	std::atomic<time_point<steady_clock>> m_startTime;
	uint32_t m_videoSequence = 0;
	uint32_t m_audioSequence = 0;
};


