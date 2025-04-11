#pragma once
#include "TVStation.h"
#include "../pch.h"

struct ServerMode
{
	uint16_t port;
};

inline void Run(const ServerMode& mode)
{
	asio::io_context io;
	std::jthread ioThread([&io] { io.run(); });

	TVStation station{ io, mode.port };

	std::cout << "TV station on port " << mode.port << std::endl;

	station.Run();
	io.stop();
}