#include "CopyFile.h"
#include "Dispatcher.h"
#include "Task.h"
#include <thread>

int main()
{
	Dispatcher dispatcher;
	Task task1 = AsyncCopyFile(dispatcher, "a.in", "a.out");
	Task task2 = AsyncCopyFile(dispatcher, "b.in", "b.out");
	std::jthread t{ [&] { dispatcher.Run(); } };
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	dispatcher.Wait();
	// dispatcher.Stop();
}