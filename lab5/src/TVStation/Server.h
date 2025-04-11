#pragma once
#include "../pch.h"
#include <iostream>

class Server
{
public:
	explicit Server(asio::io_context& io, const uint16_t port)
		: m_socket(io, udp::v6())
		  , m_endpoint(udp::v6(), port)
	{
	}

	void Send(const unsigned char* bytes, const size_t bytesCount)
	{
		m_socket.async_send_to(
			asio::buffer(bytes, bytesCount),
			m_endpoint,
			[](const boost::system::error_code& ec, std::size_t _) {
				if (ec)
				{
					std::cerr << "Send failed: " << ec.message() << std::endl;
				}
			});
	}

private:
	udp::socket m_socket;
	udp::endpoint m_endpoint;
};

