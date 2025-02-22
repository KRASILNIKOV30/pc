#pragma once
#include <fstream>
#include "Life.h"
#include "../../lab1/Timer.h"

#include <optional>
#include <string>

struct StepMode
{
	std::string inputFileName;
	int threadsNum;
	std::optional<std::string> outputFileName;
};

inline Field ReadField(const std::string& inputFileName)
{
	std::ifstream input(inputFileName);
	int width;
	int height;
	input >> width >> height;

	Cells cells;
	cells.reserve(width * height);
	char ch;
	while (input.get(ch))
	{
		if (ch == LIVE_CELL || ch == DEAD_CELL)
		{
			cells.emplace_back(ch);
		}
	}

	return Field(width, height, cells);
}

inline void WriteField(const std::string& outputFileName, const Field& field)
{
	std::ofstream output(outputFileName);
	const auto [width, height, cells] = field;
	output << width << ' ' << height << std::endl;
	for (int i = 0; i < cells.size(); ++i)
	{
		output << cells[i];
		if ((i + 1) % width == 0 && i != 0)
		{
			output << std::endl;
		}
	}
}


inline void Run(StepMode const& mode)
{
	const auto [inputFileName, threadsNum, outputFileName] = mode;
	const auto field = ReadField(inputFileName);
	Life life(field, threadsNum);
	MeasureTime(std::cout, "Life", &Life::NextStep, life);
	WriteField(outputFileName.value_or(inputFileName), life.GetField());
}