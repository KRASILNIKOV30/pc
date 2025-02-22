#include "GenerateMode.h"
#include <fstream>

float Rand()
{
	return static_cast<float>(std::rand()) / RAND_MAX;
}

void Run(GeneratorMode const& mode)
{
	std::ofstream output(mode.outputFileName);
	const auto width = mode.width;
	const auto height = mode.height;
	output << width << " " << height << std::endl;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			output << (Rand() <= mode.probability ? '#' : ' ');
		}
		output << std::endl;
	}
}
