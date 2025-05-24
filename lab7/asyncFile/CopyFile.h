#pragma once
#include "OpenAsyncFile.h"
#include "Task.h"
#include <string>
#include <vector>

inline Task AsyncCopyFile(Dispatcher& dispatcher, std::string const& from, std::string const& to)
{
	const AsyncFile input = co_await AsyncOpenFile(dispatcher, from, OpenMode::Read);
	const AsyncFile output = co_await AsyncOpenFile(dispatcher, to, OpenMode::Write);

	std::vector<char> buffer(1024);

	for (unsigned bytesRead;
	     (bytesRead = co_await input.ReadAsync(dispatcher, buffer.data(), buffer.size())) != 0;
	)
	{
		co_await output.AsyncWrite(dispatcher, buffer.data(), bytesRead);
	}
}

inline Task AsyncCopyTwoFiles(Dispatcher& dispatcher,
	std::string const& from1,
	std::string const& to1,
	std::string const& from2,
	std::string const& to2)
{
	auto t1 = AsyncCopyFile(dispatcher, from1, to1);
	auto t2 = AsyncCopyFile(dispatcher, from2, to2);
	co_await t1;
	co_await t2;
}