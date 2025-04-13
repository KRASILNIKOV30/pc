#pragma once
#include "Server.h"
#include "VideoCamera.h"
#include "../TVReceiver/Client.h"

class TVStation : public Server
{
public:
	TVStation(asio::io_context& io, const uint16_t port)
		: Server(io, port)
		  , m_camera([this](const auto& frame) { HandleFrame(frame); })
	{
	}

	void Run()
	{
		m_camera.Run();
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

private:
	VideoCamera m_camera;
};


