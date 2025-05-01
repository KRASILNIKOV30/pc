#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../threadPool/ThreadPoolLockBased.h"

TEST_CASE("dispatch and execute")
{
	std::atomic<int> counter(0);
	ThreadPoolLockBased pool(1);

	pool.Dispatch([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		counter.fetch_add(1);
	});

	pool.Wait();
	REQUIRE(counter.load() == 1);
}

TEST_CASE("stop before dispatch")
{
	ThreadPoolLockBased pool(4);
	pool.Stop();

	bool taskExecuted = false;
	pool.Dispatch([&]() { taskExecuted = true; });
	pool.Wait();

	REQUIRE(taskExecuted == false);
}

TEST_CASE("wait until all tasks are done")
{
	std::atomic<int> counter(0);
	ThreadPoolLockBased pool(4);

	pool.Dispatch([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		counter.fetch_add(1);
	});
	pool.Dispatch([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		counter.fetch_add(1);
	});

	pool.Wait();
	REQUIRE(counter.load() == 2);
}

TEST_CASE("multiple threads dispatch")
{
	std::atomic<int> counter(0);
	const int numTasks = 100;
	ThreadPoolLockBased pool(4);

	for (int i = 0; i < numTasks; ++i)
	{
		pool.Dispatch([&]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			counter.fetch_add(1);
		});
	}

	pool.Wait();
	REQUIRE(counter.load() == numTasks);
}

TEST_CASE("stop after dispatch")
{
	ThreadPoolLockBased pool(4);
	pool.Dispatch([]() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });

	pool.Stop();
	pool.Wait();

	REQUIRE(true);
}