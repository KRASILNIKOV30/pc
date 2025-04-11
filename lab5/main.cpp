#include "src/Client.h"
#include "src/Server.h"
#include <filesystem>
#include <variant>

namespace fs = std::filesystem;

using ProgramMode = std::variant<ClientMode, ServerMode>;

uint16_t GetPort(const char* arg)
{
	const uint16_t port = std::stoul(arg);
	if (port < 1 || port >= std::numeric_limits<uint16_t>::max())
	{
		throw std::runtime_error("Invalid port");
	}

	return port;
}

ProgramMode ParseCommandLine(const int argc, char* argv[])
{
	if (argc != 2 && argc != 3)
	{
		throw std::invalid_argument("Invalid number of arguments");
	}

	if (argc == 2)
	{
		return ServerMode{
			.port = GetPort(argv[1])
		};
	}

	return ClientMode{
		.address = argv[1],
		.port = GetPort(argv[2])
	};
}

int main(const int argc, char* argv[])
{
	try
	{
		auto mode = ParseCommandLine(argc, argv);
		std::visit([](const auto& m) { Run(m); }, mode);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
