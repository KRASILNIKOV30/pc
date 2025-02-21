#include "GenerateMode.h"
#include "StepMode.h"
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <variant>

using ProgramMode = std::variant<GeneratorMode, StepMode>;

ProgramMode ParseCommandLine(const int argc, char* argv[])
{
	if (argc < 4)
	{
		throw std::runtime_error("Not enough arguments");
	}

	const std::string mode = argv[1];
	if (mode == "generate")
	{
		if (argc != 6)
		{
			throw std::runtime_error("Invalid arguments number for generate mode");
		}

		return GeneratorMode{
			.outputFileName = argv[2],
			.width = std::stoi(argv[3]),
			.height = std::stoi(argv[4]),
			.probability = std::stof(argv[5]),
		};
	}

	if (mode == "step")
	{
		if (argc != 4 && argc != 5)
		{
			throw std::runtime_error("Invalid arguments number for step mode");
		}

		return StepMode{
			.inputFileName = argv[2],
			.threadsNum = std::stoi(argv[3]),
			.outputFileName = argc == 5 ? std::optional<std::string>(argv[4]) : std::nullopt,
		};
	}

	if (mode == "visualize")
	{
		throw std::runtime_error("not implemented");
	}

	throw std::runtime_error("Unknown mode");
}

int main(const int argc, char* argv[])
{
	std::srand(static_cast<unsigned int>(time(nullptr)));
	try
	{
		auto mode = ParseCommandLine(argc, argv);
		std::visit([](const auto& m) { Run(m); }, mode);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
