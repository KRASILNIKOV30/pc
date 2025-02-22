#pragma once
#include <string>

struct Args
{
	std::string inputFileName;
	std::string outputFileName;
	int radius;
	int threadsNum;
};

void GaussBlur(Args const& args);