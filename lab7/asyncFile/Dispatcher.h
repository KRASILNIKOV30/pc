#pragma once
#include <liburing.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <coroutine>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

enum class OpenMode
{
	Read,
	Write
};

class Dispatcher;

struct OperationState
{
	std::coroutine_handle<> handle;
	int res;
	bool completed = false;
};

class Dispatcher
{
public:
	Dispatcher()
	{
		if (io_uring_queue_init(1024, &ring_, 0) < 0)
		{
			throw std::runtime_error("Failed to initialize io_uring");
		}
	}

	~Dispatcher()
	{
		io_uring_queue_exit(&ring_);
	}

	void Stop()
	{
		m_isRunning = false;
	}

	void Run()
	{
		while (m_isRunning)
		{
			io_uring_submit_and_wait(&ring_, 1);

			io_uring_cqe* cqe;
			unsigned head;
			io_uring_for_each_cqe(&ring_, head, cqe)
			{
				auto state = static_cast<OperationState*>(io_uring_cqe_get_data(cqe));
				if (state)
				{
					state->res = cqe->res;
					state->completed = true;
					if (state->handle)
					{
						state->handle.resume();
					}
				}
				io_uring_cqe_seen(&ring_, cqe);
			}
		}
	}

	void QueueOpen(OperationState* state, const char* path, int flags, mode_t mode)
	{
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
		io_uring_prep_openat(sqe, AT_FDCWD, path, flags, mode);
		io_uring_sqe_set_data(sqe, state);
	}

	void QueueRead(OperationState* state, int fd, void* buf, size_t count)
	{
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
		io_uring_prep_read(sqe, fd, buf, count, 0);
		io_uring_sqe_set_data(sqe, state);
	}

	void QueueWrite(OperationState* state, int fd, const void* buf, size_t count)
	{
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
		io_uring_prep_write(sqe, fd, buf, count, 0);
		io_uring_sqe_set_data(sqe, state);
	}

	void QueueClose(OperationState* state, int fd)
	{
		io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
		io_uring_prep_close(sqe, fd);
		io_uring_sqe_set_data(sqe, state);
	}

private:
	io_uring ring_{};
	std::atomic_bool m_isRunning = true;
};

class AsyncFile
{
public:
	AsyncFile(int fd)
		: fd_(fd)
	{
	}

	~AsyncFile()
	{
		if (fd_ != -1)
		{
			close(fd_);
		}
	}

	AsyncFile(const AsyncFile&) = delete;
	AsyncFile& operator=(const AsyncFile&) = delete;

	AsyncFile(AsyncFile&& other) noexcept
		: fd_(other.fd_)
	{
		other.fd_ = -1;
	}

	AsyncFile& operator=(AsyncFile&& other) noexcept
	{
		if (this != &other)
		{
			if (fd_ != -1)
			{
				close(fd_);
			}
			fd_ = other.fd_;
			other.fd_ = -1;
		}
		return *this;
	}

	struct ReadAwaiter
	{
		Dispatcher& dispatcher;
		int fd;
		void* buf;
		size_t count;
		OperationState state;

		bool await_ready() const noexcept { return false; }

		void await_suspend(std::coroutine_handle<> handle)
		{
			state.handle = handle;
			dispatcher.QueueRead(&state, fd, buf, count);
		}

		unsigned await_resume() const
		{
			if (state.res < 0)
			{
				throw std::runtime_error("Read failed");
			}
			return static_cast<unsigned>(state.res);
		}
	};

	ReadAwaiter ReadAsync(Dispatcher& dispatcher, void* buf, size_t count)
	{
		return ReadAwaiter{ dispatcher, fd_, buf, count };
	}

	struct WriteAwaiter
	{
		Dispatcher& dispatcher;
		int fd;
		const void* buf;
		size_t count;
		OperationState state;

		bool await_ready() const noexcept { return false; }

		void await_suspend(std::coroutine_handle<> handle)
		{
			state.handle = handle;
			dispatcher.QueueWrite(&state, fd, buf, count);
		}

		unsigned await_resume() const
		{
			if (state.res < 0)
			{
				throw std::runtime_error("Write failed");
			}
			return static_cast<unsigned>(state.res);
		}
	};

	WriteAwaiter AsyncWrite(Dispatcher& dispatcher, const void* buf, size_t count)
	{
		return WriteAwaiter{ dispatcher, fd_, buf, count };
	}

private:
	int fd_;
};

struct OpenAwaiter
{
	Dispatcher& dispatcher;
	std::string path;
	int flags;
	mode_t mode;
	OperationState state;

	bool await_ready() const noexcept { return false; }

	void await_suspend(std::coroutine_handle<> handle)
	{
		state.handle = handle;
		dispatcher.QueueOpen(&state, path.c_str(), flags, mode);
	}

	AsyncFile await_resume() const
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

struct Task
{
	struct promise_type;

	using handle_type = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		Task get_return_object()
		{
			return Task{ handle_type::from_promise(*this) };
		}

		std::suspend_never initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }

		void return_void()
		{
		}

		void unhandled_exception() { std::terminate(); }
	};

	struct Awaiter
	{
		handle_type coro;

		bool await_ready() const noexcept { return false; }

		void await_suspend(std::coroutine_handle<> awaiting) noexcept
		{
			coro.resume();
			awaiting.resume();
		}

		void await_resume() noexcept
		{
		}
	};

	Task(handle_type h)
		: coro(h)
	{
	}

	~Task()
	{
		if (coro)
			coro.destroy();
	}

	Awaiter operator co_await() const
	{
		return Awaiter{ coro };
	}

private:
	handle_type coro;
};

// Пример использования
inline Task AsyncCopyFile(Dispatcher& dispatcher, std::string const& from, std::string to)
{
	AsyncFile input = co_await AsyncOpenFile(dispatcher, from, OpenMode::Read);
	AsyncFile output = co_await AsyncOpenFile(dispatcher, to, OpenMode::Write);

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