#include "src/ChordsGenerator.h"
#include "src/Parser.h"
#include "src/Player.h"
#include <iostream>
#include <span>
#include <fstream>

struct Args
{
	std::string inputFileName;
};

Args ParseArgs(int argc, char* argv[])
{
	if (argc != 2)
	{
		throw std::runtime_error("Wrong number of arguments");
	}

	return {
		.inputFileName = argv[1]
	};
}

int main(int argc, char* argv[])
{
	try
	{
		const auto [inputFileName] = ParseArgs(argc, argv);
		std::ifstream input(inputFileName);
		Parser parser(input);
		const auto bpm = parser.GetBpm();
		const auto chords = parser.GetChords();
		Player player(ma_format_f32, 1);
		ChordGenerator chordGenerator(player.GetSampleRate(), bpm, chords, 1.f);

		player.SetDataCallback([&chordGenerator](void* output, ma_uint32 frameCount) mutable {
			auto samples = std::span(static_cast<ma_float*>(output), frameCount);
			for (auto& sample : samples)
			{
				sample = chordGenerator.GetNextSample();
			}
		});

		player.Start();

		std::string s;
		std::getline(std::cin, s);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}