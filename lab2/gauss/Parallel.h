#pragma once
#include <functional>
#include <thread>

inline void ComputeParallel(const size_t size, const int threadsNum, std::function<void(size_t start, size_t end)> const& callback)
{
	const auto blockSize = size / threadsNum;
	{
		std::vector<std::jthread> threads(threadsNum - 1);
		size_t blockStart = 0;
		for (size_t i = 0; i < threadsNum - 1; ++i)
		{
			auto blockEnd = blockStart;
			blockEnd += blockSize;
			threads[i] = std::jthread{ callback, blockStart, blockEnd };
			blockStart = blockEnd;
		}
		callback(blockStart, size);
	}
}