#pragma once
#include <iostream>
#include <string>
#include "pch.h"

class UdpClient
{
public:
	UdpClient(asio::io_context& io, const std::string& addressStr, const uint16_t port)
		: m_socket(io, udp::v6())
	{
		const auto address = asio::ip::make_address(addressStr);
		m_server = udp::endpoint(address, port);
		boost::system::error_code error;
		m_socket.bind({ m_socket.local_endpoint(error).protocol(), port }, error);
		if (error)
		{
			std::cerr << "UDPPeer::bind error: " << error.message() << std::endl;
		}
		StartReceive();
	}

	virtual ~UdpClient() = default;

protected:
	virtual void OnReceive(std::string const& data) = 0;

private:
	void StartReceive()
	{
		m_socket.async_receive_from(
			asio::buffer(m_buffer),
			m_server,
			[this](const boost::system::error_code ec, const std::size_t bytes) {
				if (!ec)
				{
					const std::string msg(m_buffer.data(), bytes);
					OnReceive(msg);
				}
				StartReceive();
			}
			);
	}

	asio::ip::udp::socket m_socket;
	asio::ip::udp::endpoint m_server;
	std::array<char, 1024> m_buffer{};
};

class Receiver final : public UdpClient
{
public:
	Receiver(asio::io_context& io, const std::string& addressStr, const uint16_t port)
		: UdpClient(io, addressStr, port)
	{
	}

private:
	void OnReceive(std::string const& data) override
	{
		std::cout << "Received data: " << data << std::endl;
	}
};

struct ClientMode
{
	std::string address;
	uint16_t port;
};

inline void Run(const ClientMode& mode)
{
	try
	{
		asio::io_context io;
		std::jthread thread([&io] { io.run(); });
		Receiver receiver(io, mode.address, mode.port);
		std::cout << "Connected to server at " << mode.address << ":" << mode.port << std::endl;
		thread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}