#pragma once
#include <string>

struct GeneratorMode
{
	std::string outputFileName;
	int width;
	int height;
	float probability;
};

void Run(GeneratorMode const& mode);