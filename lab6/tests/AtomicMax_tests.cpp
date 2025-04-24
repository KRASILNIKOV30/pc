#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../atomicMax/AtomicMax.h"
#include <thread>

TEST_CASE("atomic max tests")
{
	const auto threadsNum = std::thread::hardware_concurrency();
	std::vector threadLocalMaxes(threadsNum, 0);
	AtomicMax globalMax(0);

	{
		std::atomic start(false);
		std::vector<std::jthread> threads;
		for (int i = 0; i < threadsNum; i++)
		{
			threads.emplace_back([&, i] {
				while (!start.load())
				{
				}

				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution dist(1, 100);

				for (int j = 0; j < 1'000'000; j++)
				{
					const auto value = dist(gen);
					globalMax.Update(value);
					if (value > threadLocalMaxes[i])
					{
						threadLocalMaxes[i] = value;
					}
				}
			});
		}

		start.store(true);
	}

	auto greatestLocalMax = 0;
	for (const auto localMax : threadLocalMaxes)
	{
		if (localMax > greatestLocalMax)
		{
			greatestLocalMax = localMax;
		}
	}

	REQUIRE(greatestLocalMax == globalMax.GetValue());
}

