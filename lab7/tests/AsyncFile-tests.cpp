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

				//cppcoro::sync_wait(copy_task);
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

// SCENARIO("Copy two files")
// {
// 	GIVEN("A Dispatcher and test files")
// 	{
// 		Dispatcher dispatcher;
// 		const std::string testInput1 = "test_input1.txt";
// 		const std::string testOutput1 = "test_output1.txt";
// 		const std::string testInput2 = "test_input2.txt";
// 		const std::string testOutput2 = "test_output2.txt";
//
// 		{
// 			std::ofstream out1(testInput1);
// 			out1 << "Hello, async world!\nThis is a test file number one.";
// 			std::ofstream out2(testInput2);
// 			out2 << "Hello, async world!\nThis is a test file number two.";
// 		}
//
// 		WHEN("We copy two files asynchronously")
// 		{
// 			Task copyTask = AsyncCopyTwoFiles(dispatcher, testInput1, testOutput1, testInput2, testOutput2);
//
// 			THEN("The task should complete successfully")
// 			{
// 				std::jthread worker([&]() { dispatcher.Run(); });
//
// 				std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
// 				//cppcoro::sync_wait(copy_task);
// 				dispatcher.Stop();
//
// 				AND_THEN("The output file should match the input")
// 				{
// 					std::ifstream in1(testInput1);
// 					std::ifstream out1(testOutput1);
//
// 					std::ifstream in2(testInput2);
// 					std::ifstream out2(testOutput2);
//
// 					REQUIRE(std::equal(
// 						std::istreambuf_iterator<char>(in1),
// 						std::istreambuf_iterator<char>(),
// 						std::istreambuf_iterator<char>(out1)));
//
// 					REQUIRE(std::equal(
// 						std::istreambuf_iterator<char>(in2),
// 						std::istreambuf_iterator<char>(),
// 						std::istreambuf_iterator<char>(out2)));
// 				}
// 			}
// 		}
//
// 		std::remove(testInput1.c_str());
// 		std::remove(testOutput1.c_str());
// 		std::remove(testInput2.c_str());
// 		std::remove(testOutput2.c_str());
// 	}
// }
