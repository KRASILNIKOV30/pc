#pragma once
#include <cassert>
#include <vector>

template <typename T>
inline std::vector<std::vector<T>> ChunkVector(std::vector<T> v, size_t chunkSize)
{
	assert(chunkSize >= 1);
	auto begin = std::begin(v);
	auto end = std::end(v);
	auto chunkBegin = begin;
	auto chunkEnd = begin;
	chunkBegin = chunkEnd = begin;
	std::vector<std::vector<T>> chunks;

	do
	{
		if (std::distance(chunkEnd, end) < chunkSize)
		{
			chunkEnd = end;
		}
		else
		{
			std::advance(chunkEnd, chunkSize);
		}
		chunks.emplace_back(chunkBegin, chunkEnd);
		chunkBegin = chunkEnd;
	} while (std::distance(chunkBegin, v.end()) > 0);

	return chunks;
}