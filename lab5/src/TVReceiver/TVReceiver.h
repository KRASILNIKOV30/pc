#pragma once
#include "AudioPlayer.h"
#include "Client.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

class TVReceiver final : public UdpClient
{
public:
	TVReceiver(asio::io_context& io, const std::string& addressStr, const uint16_t port)
		: UdpClient(io, addressStr, port)
		  , m_ioContext(io.get_executor())
	{
		namedWindow("Video", cv::WINDOW_AUTOSIZE);
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

		const uint8_t dataType = data[0];
		const uint8_t* payload = data.data() + 1;
		const size_t payloadSize = bytesCount - 1;

		if (dataType == 0x01)
		{
			const auto sampleCount = payloadSize / sizeof(int16_t);
			std::vector<int16_t> audioPackage(sampleCount);
			memcpy(audioPackage.data(), payload, payloadSize);
			post(m_threadPool, [this, audioData = std::move(audioPackage)] {
				ProcessAudio(audioData);
			});
		}
		else if (dataType == 0x02)
		{
			std::vector videoPackage(payload, payload + payloadSize);
			post(m_threadPool, [this, videoData = std::move(videoPackage)] {
				ProcessVideo(videoData);
			});
		}
	}

	void ProcessVideo(std::vector<uint8_t> const& data)
	{
		const auto image = imdecode(data, cv::IMREAD_COLOR);
		if (image.empty())
		{
			std::cerr << "Received empty image" << std::endl;
			return;
		}
		imshow("Video", image);
		if (cv::waitKey(1) == 'q')
		{
			StopReceive();
		}
	}

	void ProcessAudio(const std::vector<int16_t>& data)
	{
		m_audioPlayer.PushData(data);
	}

private:
	AudioPlayer m_audioPlayer;
	asio::thread_pool m_threadPool{ std::thread::hardware_concurrency() };
	std::atomic_bool m_running{ true };
	asio::io_context::executor_type m_ioContext;
};
