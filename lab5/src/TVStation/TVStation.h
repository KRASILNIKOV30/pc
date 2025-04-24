#pragma once
#include "Microphone.h"
#include "Server.h"
#include "VideoCamera.h"
#include "../PacketType.h"
#include "../TVReceiver/Client.h"
#include <opus/opus.h>

class TVStation : public Server
{
public:
	TVStation(asio::io_context& io, const uint16_t port)
		: Server(io, port)
		  , m_camera([this](const auto& frame) { HandleFrame(frame); })
		  , m_microphone([this](const auto& audioData, uint32_t sampleRate) { HandleAudio(audioData, sampleRate); })
	{
		InitOpusEncoder();
	}

	void Run()
	{
		m_startTime.store(steady_clock::now());
		std::jthread videoThread([this] { m_camera.Run(); });
		std::jthread audioThread([this] { m_microphone.Run(); });
	}

	~TVStation()
	{
		if (m_opusEncoder)
		{
			opus_encoder_destroy(m_opusEncoder);
		}
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
		std::vector<uint8_t> compressedData(1275);
		const int compressedSize = opus_encode(m_opusEncoder, audioData.data(), audioData.size(), compressedData.data(), compressedData.size());

		if (compressedSize < 0)
		{
			std::cerr << "Opus encoding error: " << opus_strerror(compressedSize) << std::endl;
			return;
		}

		const MediaHeader header{ AUDIO_PACKET, timestamp, m_audioSequence++ };
		std::vector<uint8_t> packet(sizeof(header) + compressedSize);

		memcpy(packet.data(), &header, sizeof(header));
		memcpy(packet.data() + sizeof(header), compressedData.data(), compressedSize);

		Send(packet.data(), packet.size());
	}

	[[nodiscard]] uint64_t GetTimestamp() const
	{
		return duration_cast<microseconds>(steady_clock::now() - m_startTime.load()).count();
	}

	void InitOpusEncoder()
	{
		m_opusEncoder = opus_encoder_create(SAMPLE_RATE, 1, OPUS_APPLICATION_AUDIO, &m_opusError);
		if (m_opusError != OPUS_OK)
		{
			std::cout << m_opusError;
			throw std::runtime_error("Failed to create Opus encoder");
		}

		opus_encoder_ctl(m_opusEncoder, OPUS_SET_BITRATE(OPUS_AUTO));
		opus_encoder_ctl(m_opusEncoder, OPUS_SET_COMPLEXITY(10));
	}

private:
	VideoCamera m_camera;
	Microphone m_microphone;
	std::atomic<time_point<steady_clock>> m_startTime;
	uint32_t m_videoSequence = 0;
	uint32_t m_audioSequence = 0;
	OpusEncoder* m_opusEncoder;
	int m_opusError;
};


