#pragma once
#include "Dispatcher.h"

class AsyncFile
{
public:
	explicit AsyncFile(const int fd)
		: m_fd(fd)
	{
	}

	~AsyncFile()
	{
		if (m_fd != -1)
		{
			close(m_fd);
		}
	}

	AsyncFile(const AsyncFile&) = delete;
	AsyncFile& operator=(const AsyncFile&) = delete;

	AsyncFile(AsyncFile&& other) noexcept
		: m_fd(other.m_fd)
	{
		other.m_fd = -1;
	}

	AsyncFile& operator=(AsyncFile&& other) noexcept
	{
		if (this != &other)
		{
			if (m_fd != -1)
			{
				close(m_fd);
			}
			m_fd = other.m_fd;
			other.m_fd = -1;
		}
		return *this;
	}

	struct ReadAwaiter
	{
		Dispatcher& dispatcher;
		int fd{};
		void* buf{};
		size_t count{};
		OperationState state;

		static bool await_ready() noexcept { return false; }

		void await_suspend(std::coroutine_handle<> handle)
		{
			state.handle = handle;
			dispatcher.QueueRead(&state, fd, buf, count);
		}

		[[nodiscard]] unsigned await_resume() const
		{
			if (state.res < 0)
			{
				throw std::runtime_error("Read failed");
			}
			return static_cast<unsigned>(state.res);
		}
	};

	ReadAwaiter ReadAsync(Dispatcher& dispatcher, void* buf, size_t count) const
	{
		return ReadAwaiter{ dispatcher, m_fd, buf, count };
	}

	struct WriteAwaiter
	{
		Dispatcher& dispatcher;
		int fd{};
		const void* buf{};
		size_t count{};
		OperationState state;

		static bool await_ready() noexcept { return false; }

		void await_suspend(std::coroutine_handle<> handle)
		{
			state.handle = handle;
			dispatcher.QueueWrite(&state, fd, buf, count);
		}

		[[nodiscard]] unsigned await_resume() const
		{
			if (state.res < 0)
			{
				throw std::runtime_error("Write failed");
			}
			return static_cast<unsigned>(state.res);
		}
	};

	WriteAwaiter AsyncWrite(Dispatcher& dispatcher, const void* buf, size_t count) const
	{
		return WriteAwaiter{ dispatcher, m_fd, buf, count };
	}

private:
	int m_fd;
};