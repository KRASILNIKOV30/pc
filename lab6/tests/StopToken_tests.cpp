#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../stopToken/StopToken.h"

#include <thread>

TEST_CASE("StopSource and StopToken basic functionality")
{
	StopSource source;

	SECTION("Initially, stop is not requested")
	{
		StopToken token = source.GetToken();
		REQUIRE_FALSE(token.StopRequested());
	}

	SECTION("Requesting stop changes the state")
	{
		StopToken token = source.GetToken();
		REQUIRE_FALSE(token.StopRequested());

		bool firstRequest = source.RequestStop();
		REQUIRE(firstRequest);
		REQUIRE(token.StopRequested());

		bool secondRequest = source.RequestStop();
		REQUIRE_FALSE(secondRequest);
	}

	SECTION("Multiple tokens see the same state")
	{
		StopToken token1 = source.GetToken();
		StopToken token2 = source.GetToken();

		REQUIRE_FALSE(token1.StopRequested());
		REQUIRE_FALSE(token2.StopRequested());

		source.RequestStop();

		REQUIRE(token1.StopRequested());
		REQUIRE(token2.StopRequested());
	}

	SECTION("Moving tokens works correctly")
	{
		StopToken token1 = source.GetToken();
		StopToken token2 = std::move(token1);

		REQUIRE_FALSE(token2.StopRequested());
		source.RequestStop();
		REQUIRE(token2.StopRequested());
	}
}

TEST_CASE("Thread-safety checks")
{
	StopSource source;
	const StopToken token = source.GetToken();

	SECTION("Concurrent stop requests")
	{
		bool stopped = false;

		{
			auto requester = [&] {
				stopped = source.RequestStop();
			};
			std::jthread t1(requester);
			std::jthread t2(requester);
		}

		REQUIRE_FALSE(stopped);
		REQUIRE(token.StopRequested());
	}

	SECTION("Token sees changes from another thread")
	{
		std::atomic observerSawStop(false);
		{
			std::jthread stopper([&]() {
				source.RequestStop();
			});

			std::jthread observer([&]() {
				while (!token.StopRequested())
				{
					std::this_thread::yield();
				}
				observerSawStop = true;
			});
		}

		REQUIRE(observerSawStop);
	}
}