#pragma once
#include "Client.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

class TVReceiver final : public UdpClient
{
public:
	TVReceiver(asio::io_context& io, const std::string& addressStr, const uint16_t port)
		: UdpClient(io, addressStr, port)
	{
	}

private:
	void OnReceive(std::array<uint8_t, BUFFER_SIZE> const& data, size_t bytesCount) override
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
};
