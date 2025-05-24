#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../myTask/MyTask.h"

SCENARIO("MyTask handles string results from coroutines")
{
	GIVEN("A coroutine returning a string")
	{
		auto SimpleCoroutine = [](std::string s1, std::string s2) -> MyTask {
			co_return s1 + s2;
		};
		WHEN("the coroutine returns 'Hello from coroutine!'")
		{
			const MyTask task = SimpleCoroutine("a", "b");
			THEN("should return the same string")
			{
				REQUIRE(task.GetResult() == "Hello from coroutine!");
			}
		}
	}

	GIVEN("A coroutine returning an empty string")
	{
		auto EmptyCoroutine = []() -> MyTask {
			co_return "";
		};

		WHEN("the coroutine is executed")
		{
			const MyTask task = EmptyCoroutine();

			THEN("should return an empty string")
			{
				REQUIRE(task.GetResult().empty());
			}
		}
	}
}

SCENARIO("MyTask supports multiple independent coroutines")
{
	GIVEN("Two coroutines with different messages")
	{
		auto CoroutineA = []() -> MyTask { co_return "Message A"; };
		auto CoroutineB = []() -> MyTask { co_return "Message B"; };

		WHEN("both coroutines are executed")
		{
			MyTask taskA = CoroutineA();
			MyTask taskB = CoroutineB();

			THEN("they should return their respective messages")
			{
				REQUIRE(taskA.GetResult() == "Message A");
				REQUIRE(taskB.GetResult() == "Message B");
			}
		}
	}
}

