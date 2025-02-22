#pragma once
#include <string>

struct Args
{
	std::string inputFileName;
	std::string outputFileName;
	int radius;
	int threadsNum;
};

inline void GaussBlur(Args const& args);