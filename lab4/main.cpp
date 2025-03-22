#include "src/ChordsGenerator.h"
#include "src/Parser.h"
#include "src/Player.h"
#include <iostream>
#include <span>
#include <fstream>
#include <SFML/Graphics.hpp>

constexpr int WINDOW_WIDTH = 1700;
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

void SetPlayerDataCallback(Player& player, ChordGenerator& chordGenerator, std::vector<float>& samplesBuffer, std::mutex& mutex)
{
	player.SetDataCallback([&](void* output, ma_uint32 frameCount) mutable {
		auto samples = std::span(static_cast<ma_float*>(output), frameCount);
		std::vector<float> newSamples;
		newSamples.reserve(samples.size());
		for (auto& sample : samples)
		{
			sample = chordGenerator.GetNextSample();
			newSamples.push_back(sample);
		}
		std::lock_guard lock(mutex);
		std::swap(samplesBuffer, newSamples);
	});
}

void RunVisualization(sf::RenderWindow& window, std::vector<float> const& samples, std::mutex& mutex)
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
			const auto size = samples.size();
			sf::VertexArray waveform(sf::LineStrip, size);
			for (size_t i = 0; i < size; ++i)
			{
				const auto x = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(size);
				const auto y = static_cast<float>(WINDOW_HEIGHT) / 2 + samples[i] * 200;
				waveform[i].position = sf::Vector2f(x * static_cast<float>(i), y);
			}
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
		std::vector<float> samples;
		sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sound Wave Visualization");
		sf::VertexArray waveform(sf::LineStrip, WINDOW_WIDTH);

		const auto [inputFileName] = ParseArgs(argc, argv);
		std::ifstream input(inputFileName);

		Player player(ma_format_f32, 1);
		auto chordGenerator = InitChordGenerator(input, player.GetSampleRate());

		SetPlayerDataCallback(player, chordGenerator, samples, mutex);
		player.Start();
		RunVisualization(window, samples, mutex);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}