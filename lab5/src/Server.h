#pragma once
#include "pch.h"
#include <iostream>

class Server
{
public:
	explicit Server(asio::io_context& io, uint16_t port)
		: m_socket(io, udp::v6())

	{
		boost::system::error_code ec;
		m_socket.bind({ udp::v6(), port }, ec);
		if (ec)
		{
			std::cerr << "UDPPeer::bind error: " << ec.message() << std::endl;
		}
		// boost::system::error_code ec;
		//m_socket.set_option(asio::socket_base::broadcast(true), ec);
		// if (ec)
		// {
		// 	throw std::runtime_error("Failed to set broadcast option: " + ec.message());
		// }
	}

	void Send(const std::string& message)
	{
		m_socket.send_to(
			asio::buffer(message),
			m_endpoint
			// [message](const boost::system::error_code& ec, std::size_t bytesSent) {
			// 	std::cout << "hello" << std::endl;
			// 	if (ec)
			// 	{
			// 		if (ec != asio::error::operation_aborted)
			// 		{
			// 			std::cerr << "Send failed: " << ec.message() << "\n";
			// 		}
			// 		return;
			// 	}
			// 	std::cout << "Sent: " << message << " (" << bytesSent << " bytes)\n";
			// }
			);
	}

private:
	udp::socket m_socket;
	udp::endpoint m_endpoint{ udp::v6(), 8080 };
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

