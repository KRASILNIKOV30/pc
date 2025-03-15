#include "Simulation.h"

#include <iostream>

int main(const int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <single|multi> [log]" << std::endl;
		return 1;
	}

	const bool multiThreaded = (std::string(argv[1]) == "multi");
	const bool log = (argc > 2 && std::string(argv[2]) == "log");

	Simulation simulation(multiThreaded, log);

	simulation.Start();

	return 0;
}