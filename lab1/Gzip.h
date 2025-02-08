#pragma once
#include <vector>
#include <string>
#include <stdexcept>

inline void GzipFile(std::string const& param, std::string const& filename)
{
	const std::string command = "gzip " + param + " " + filename;
	if (system(command.c_str()) != 0)
	{
		throw std::runtime_error("Error generating gzip file");
	}
}

inline void GzipFiles(std::string const& param, std::vector<std::string> const& files)
{
	for (const auto& file : files)
	{
		GzipFile(param, file);
	}
}