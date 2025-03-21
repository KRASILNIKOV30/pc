#include "src/ChordsGenerator.h"
#include "src/Player.h"
#include <iostream>
#include <span>

int main()
{
	Player player(ma_format_f32, 1);

	ChordGenerator chordGenerator(player.GetSampleRate(), 120, {
		{ { C, 1 } },
		{ { D, 1 } },
		{ { E, 1 } },
		{ { F, 1 } },
		{ { G, 1 } },
		{ { A, 1 } },
		{ { B, 1 } },
		{ { C, 2 } },
	}, 1.f);

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