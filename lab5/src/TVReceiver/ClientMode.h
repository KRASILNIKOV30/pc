#pragma once
#include <iostream>
#include <string>
#include "../pch.h"
#include "TVReceiver.h"

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
		TVReceiver receiver(io, mode.address, mode.port);
		std::cout << "Connected to server at " << mode.address << ":" << mode.port << std::endl;
		io.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}