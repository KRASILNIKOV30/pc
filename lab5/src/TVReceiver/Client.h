#pragma once
#include <iostream>
#include <string>
#include "../pch.h"

constexpr int BUFFER_SIZE = 65536;

class UdpClient
{
public:
	UdpClient(asio::io_context& io, const std::string& addressStr, const uint16_t port)
		: m_socket(io, udp::v6())
		  , m_server(asio::ip::make_address(addressStr), port)
	{
		Bind(port);
		StartReceive();
	}

	virtual ~UdpClient() = default;

protected:
	void StopReceive()
	{
		m_running.store(false);
	}

	virtual void OnReceive(std::array<uint8_t, BUFFER_SIZE> const& data, size_t bytesCount) = 0;

private:
	void StartReceive()
	{
		m_socket.async_receive_from(
			asio::buffer(m_buffer),
			m_server,
			[this](const boost::system::error_code ec, const std::size_t bytes) {
				if (!ec)
				{
					OnReceive(m_buffer, bytes);
				}
				if (m_running.load(std::memory_order_relaxed))
				{
					StartReceive();
				}
			});
	}

	void Bind(uint16_t port)
	{
		boost::system::error_code error;
		if (m_socket.bind({ m_socket.local_endpoint(error).protocol(), port }, error))
		{
			std::cerr << "bind error: " << error.message() << std::endl;
		}
	}

	asio::ip::udp::socket m_socket;
	asio::ip::udp::endpoint m_server;
	std::array<uint8_t, BUFFER_SIZE> m_buffer{};
	std::atomic_bool m_running{ true };
};
