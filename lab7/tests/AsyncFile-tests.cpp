#define CATCH_CONFIG_MAIN
#include"../../lib/catch2/catch.hpp"
#include "../asyncFile/Dispatcher.h"
#include "../asyncFile/Task.h"
#include "../asyncFile/CopyFile.h"
#include <thread>

SCENARIO("Async file operations work correctly")
{
	GIVEN("A Dispatcher and test files")
	{
		Dispatcher dispatcher;
		const std::string testInput = "test_input.txt";
		const std::string testOutput = "test_output.txt";

		{
			std::ofstream out(testInput);
			out << "Hello, async world!\nThis is a test file.";
		}

		WHEN("We copy a file asynchronously")
		{
			Task copyTask = AsyncCopyFile(dispatcher, testInput, testOutput);

			THEN("The task should complete successfully")
			{
				std::jthread worker([&]() { dispatcher.Run(); });

				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				// cppcoro::sync_wait(copy_task);
				//dispatcher.Wait();
				dispatcher.Stop();

				AND_THEN("The output file should match the input")
				{
					std::ifstream in(testInput);
					std::ifstream out(testOutput);

					REQUIRE(std::equal(
						std::istreambuf_iterator<char>(in),
						std::istreambuf_iterator<char>(),
						std::istreambuf_iterator<char>(out)));
				}
			}
		}

		std::remove(testInput.c_str());
		std::remove(testOutput.c_str());
	}
}
