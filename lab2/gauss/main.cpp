#include "Gauss.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

Args ParseCommandLine(int argc, char* argv[])
{
	if (argc != 5)
	{
		throw std::invalid_argument("Wrong number of arguments");
	}

	Args args;
	return {
		.inputFileName = argv[1],
		.outputFileName = argv[2],
		.radius = std::stoi(argv[3]),
		.threadsNum = std::stoi(argv[4]),
	};
}

int main(const int argc, char* argv[])
{
	try
	{
		const Args args = ParseCommandLine(argc, argv);
		GaussBlur(args);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
