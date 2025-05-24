#pragma once
#include "AsyncFile.h"

struct OpenAwaiter
{
	Dispatcher& dispatcher;
	std::string path;
	int flags;
	mode_t mode;
	OperationState state;

	static bool await_ready() noexcept { return false; }

	void await_suspend(std::coroutine_handle<> handle)
	{
		state.handle = handle;
		dispatcher.QueueOpen(&state, path.c_str(), flags, mode);
	}

	[[nodiscard]] AsyncFile await_resume() const
	{
		if (state.res < 0)
		{
			throw std::runtime_error("File open failed");
		}
		return AsyncFile(state.res);
	}
};

inline OpenAwaiter AsyncOpenFile(Dispatcher& dispatcher, std::string const& path, OpenMode const mode)
{
	const int flags = (mode == OpenMode::Read) ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
	constexpr mode_t file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	return OpenAwaiter{ dispatcher, path, flags, file_mode };
}