#pragma once
#include "pch.h"
#include <iostream>

class Server
{
public:
	explicit Server(asio::io_context& io, const uint16_t port)
		: m_socket(io, udp::v6())
		  , m_endpoint(udp::v6(), port)
	{
	}

	void Send(const std::string& message)
	{
		m_socket.async_send_to(
			asio::buffer(message),
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

struct ServerMode
{
	uint16_t port;
};

inline void Run(const ServerMode& mode)
{
	asio::io_context io;
	std::jthread ioThread([&io] { io.run(); });

	Server server(io, mode.port);

	std::cout << "Server on port " << mode.port << std::endl;

	std::string str;
	while (std::getline(std::cin, str))
	{
		server.Send(str);
	}

	io.stop();
}

