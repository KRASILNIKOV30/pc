#include "src/ChordsGenerator.h"
#include "src/Parser.h"
#include "src/Player.h"
#include <iostream>
#include <span>
#include <fstream>
#include <SFML/Graphics.hpp>

constexpr int WINDOW_WIDTH = 1400;
constexpr int WINDOW_HEIGHT = 600;

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

void SetPlayerDataCallback(Player& player, ChordGenerator& chordGenerator, sf::VertexArray& waveform, std::mutex& mutex)
{
	player.SetDataCallback([&](void* output, ma_uint32 frameCount) mutable {
		auto samples = std::span(static_cast<ma_float*>(output), frameCount);
		size_t i = 0;
		std::lock_guard lock(mutex);
		for (auto& sample : samples)
		{
			sample = chordGenerator.GetNextSample();
			if (i < waveform.getVertexCount())
			{
				waveform[i].position = sf::Vector2f(static_cast<float>(i), static_cast<float>(WINDOW_HEIGHT) / 2 + sample * 200);
			}
			++i;
		}
	});
}

void RunVisualization(sf::RenderWindow& window, sf::VertexArray const& waveform, std::mutex& mutex)
{
	while (window.isOpen())
	{
		sf::Event event{};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear();
		{
			std::lock_guard lock(mutex);
			window.draw(waveform);
		}
		window.display();
	}
}

ChordGenerator InitChordGenerator(std::istream& input, ma_uint32 sampleRate)
{
	const Parser parser(input);
	const auto bpm = parser.GetBpm();
	const auto chords = parser.GetChords();
	const auto type = parser.GetType();

	return { sampleRate, bpm, chords, type, 1.f };
}

int main(int argc, char* argv[])
{
	try
	{
		std::mutex mutex{};
		sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sound Wave Visualization");
		sf::VertexArray waveform(sf::LineStrip, WINDOW_WIDTH);

		const auto [inputFileName] = ParseArgs(argc, argv);
		std::ifstream input(inputFileName);

		Player player(ma_format_f32, 1);
		auto chordGenerator = InitChordGenerator(input, player.GetSampleRate());

		SetPlayerDataCallback(player, chordGenerator, waveform, mutex);
		player.Start();
		RunVisualization(window, waveform, mutex);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}