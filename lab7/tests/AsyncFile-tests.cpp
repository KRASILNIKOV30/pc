#define CATCH_CONFIG_MAIN
#include"../../lib/catch2/catch.hpp"
#include "../asyncFile/Dispatcher.h"
#include <thread>

SCENARIO("Async file operations work correctly", "[async][file]")
{
	GIVEN("A Dispatcher and test files")
	{
		Dispatcher dispatcher;
		const std::string test_input = "test_input.txt";
		const std::string test_output = "test_output.txt";

		{
			std::ofstream out(test_input);
			out << "Hello, async world!\nThis is a test file.";
		}

		WHEN("We copy a file asynchronously")
		{
			Task copy_task = AsyncCopyFile(dispatcher, test_input, test_output);

			THEN("The task should complete successfully")
			{
				std::jthread worker([&]() { dispatcher.Run(); });

				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				//cppcoro::sync_wait(copy_task);
				dispatcher.Stop();

				AND_THEN("The output file should match the input")
				{
					std::ifstream in(test_input);
					std::ifstream out(test_output);

					REQUIRE(std::equal(
						std::istreambuf_iterator<char>(in),
						std::istreambuf_iterator<char>(),
						std::istreambuf_iterator<char>(out)));
				}
			}
		}

		// Удаляем тестовые файлы
		std::remove(test_input.c_str());
		std::remove(test_output.c_str());
	}
}

// SCENARIO("AsyncOpenFile with non-existent file")
// {
// 	Dispatcher dispatcher;
// 	const std::string non_existent = "non_existent_file.xyz";
//
// 	WHEN("Trying to open non-existent file")
// 	{
// 		auto open_task = AsyncOpenFile(dispatcher, non_existent, OpenMode::Read);
// 		std::jthread worker([&]() { dispatcher.Run(); });
//
// 		THEN("It should fail")
// 		{
// 			REQUIRE_THROWS_AS([&]() {
// 				std::this_thread::sleep_for(std::chrono::milliseconds(100));
// 				}(), std::runtime_error);
// 		}
//
// 		dispatcher.Stop();
// 		// pthread_kill(worker.native_handle(), SIGINT);
// 		// worker.join();
// 	}
// }
