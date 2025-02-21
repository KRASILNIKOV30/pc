#pragma once
#include <optional>
#include <string>

struct StepMode
{
	std::string inputFileName;
	int threadsNum;
	std::optional<std::string> outputFileName;
};

inline void Run(StepMode const& mode)
{

}