#pragma once
#include "AudioPlayer.h"
#include "Client.h"
#include "../PacketType.h"

#include <opus.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

class TVReceiver final : public UdpClient
{
public:
	TVReceiver(asio::io_context& io, const std::string& addressStr, const uint16_t port)
		: UdpClient(io, addressStr, port)
	{
		namedWindow("Video", cv::WINDOW_AUTOSIZE);
		m_videoThread = std::thread(&TVReceiver::VideoRenderThread, this);
		m_opusDecoder = opus_decoder_create(SAMPLE_RATE, 1, nullptr);
	}

	void StopProcessing()
	{
		m_running = false;
		m_threadPool.join();
	}

	~TVReceiver() override
	{
		StopProcessing();
		cv::destroyAllWindows();
	}

private:
	void OnReceive(std::array<uint8_t, BUFFER_SIZE> const& data, const size_t bytesCount) override
	{
		if (!m_running || bytesCount == 0)
		{
			return;
		}

		const auto& header = *reinterpret_cast<const MediaHeader*>(data.data());
		const uint8_t* payload = data.data() + sizeof(MediaHeader);
		const size_t payloadSize = bytesCount - sizeof(MediaHeader);

		CheckSequence(header);

		if (header.type == AUDIO_PACKET)
		{
			auto parsedData = ParseAudioPacket(payload, payloadSize);
			post(m_threadPool, [this, audioData = std::move(parsedData), header = header] {
				ProcessAudio(header, audioData);
			});
		}
		else if (header.type == VIDEO_PACKET)
		{
			std::vector videoPackage(payload, payload + payloadSize);
			post(m_threadPool, [this, videoData = std::move(videoPackage), header = header] {
				ProcessVideo(header, videoData);
			});
		}
	}

	void ProcessVideo(MediaHeader const& header, std::vector<uint8_t> const& data)
	{
		auto image = imdecode(data, cv::IMREAD_COLOR);
		if (image.empty())
		{
			std::cerr << "Received empty image" << std::endl;
			return;
		}
		std::lock_guard lock(m_videoMutex);

		if (header.timestamp > m_currentAudioTs.load() + AUDIO_LEAD_THRESHOLD)
		{
			m_videoQueue.push(std::move(image));
			m_videoCondition.notify_one();
		}
		else
		{
			while (!m_videoQueue.empty())
			{
				m_videoQueue.pop();
			}
			m_currentVideo = std::move(image);
			m_videoCondition.notify_one();
		}
	}

	void ProcessAudio(MediaHeader const& header, const std::vector<int16_t>& data)
	{
		m_currentAudioTs.store(header.timestamp);
		m_audioPlayer.PushData(data);
	}

	std::vector<int16_t> ParseAudioPacket(const uint8_t* payload, const size_t bytesCount)
	{
		std::vector<int16_t> pcmData(5760);
		int samplesDecoded = opus_decode(m_opusDecoder, payload, bytesCount,
			pcmData.data(),
			pcmData.size(),
			0);
		const auto sampleCount = bytesCount / sizeof(int16_t);
		std::vector<int16_t> audioPackage(sampleCount);

		for (size_t i = 0; i < samplesDecoded; i++)
		{
			audioPackage[i] = static_cast<int16_t>(ntohs(pcmData[i]));
		}

		return audioPackage;
	}

	void CheckSequence(MediaHeader const& header)
	{
		if (header.type == 0x01 && header.sequence != m_expectedAudioSeq++)
		{
			std::cerr << "Audio packet loss detected" << std::endl;
			m_expectedAudioSeq = header.sequence + 1;
		}
		if (header.type == 0x02 && header.sequence != m_expectedVideoSeq++)
		{
			std::cerr << "Video packet loss detected" << std::endl;
			m_expectedVideoSeq = header.sequence + 1;
		}
	}

	void VideoRenderThread()
	{
		while (m_running)
		{
			cv::Mat frameToShow;
			bool hasFrame = false;

			{
				std::unique_lock lock(m_videoMutex);
				m_videoCondition.wait(lock, [this] {
					return !m_running || !m_videoQueue.empty() || m_currentVideo.data;
				});

				if (!m_running)
				{
					return;
				}

				if (!m_videoQueue.empty())
				{
					frameToShow = std::move(m_videoQueue.front());
					m_videoQueue.pop();
					hasFrame = true;
				}
				else if (m_currentVideo.data)
				{
					frameToShow = std::move(m_currentVideo);
					hasFrame = true;
				}
			}

			if (hasFrame)
			{
				imshow("Video", frameToShow);
				if (cv::waitKey(1) == 'q')
				{
					StopReceive();
				}
			}
		}
	}

private:
	struct VideoFrame
	{
		cv::Mat frame;
		uint64_t timestamp;
	};

	AudioPlayer m_audioPlayer;
	asio::thread_pool m_threadPool{ std::thread::hardware_concurrency() };
	std::atomic_bool m_running{ true };
	uint32_t m_expectedAudioSeq = 0;
	uint32_t m_expectedVideoSeq = 0;
	std::atomic<uint64_t> m_currentAudioTs;

	std::mutex m_videoMutex;
	std::condition_variable m_videoCondition;
	std::queue<cv::Mat> m_videoQueue;
	cv::Mat m_currentVideo;
	std::thread m_videoThread;

	OpusDecoder* m_opusDecoder;

	static constexpr uint64_t AUDIO_LEAD_THRESHOLD = 50;
};
