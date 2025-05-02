#include "../lib/timer/Timer.h"
#include "threadPool/ThreadPoolLockBased.h"
#include "threadPool/ThreadPoolLockFree.h"

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <tbb/concurrent_hash_map.h>

int main()
{
	for (int threadNum = 1; threadNum <= std::thread::hardware_concurrency() * 2; threadNum++)
	{
		ThreadPoolLockFree threadPool(threadNum);

		MeasureTime(
			std::cout,
			"Work with " + std::to_string(threadNum) + " threads",
			[&] {
				std::atomic counter(0);
				for (int i = 0; i < 1'000'000; i++)
				{
					threadPool.Dispatch([&] {
						counter.fetch_add(1, std::memory_order_relaxed);
					});
				}
				threadPool.Wait();
			});
	}

	return EXIT_SUCCESS;
}

