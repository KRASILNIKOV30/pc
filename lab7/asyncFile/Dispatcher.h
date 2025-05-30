#pragma once
#include "Task.h"
#include <liburing.h>
#include <fcntl.h>
#include <unistd.h>
#include <coroutine>
#include <stdexcept>
#include <condition_variable>
#include <shared_mutex>

enum class OpenMode
{
	Read,
	Write
};

class Dispatcher;

struct OperationState
{
	std::coroutine_handle<> handle;
	int res{};
	bool completed = false;
};

class Dispatcher
{
public:
	Dispatcher()
	{
		if (io_uring_queue_init(1024, &m_ring, 0) < 0)
		{
			throw std::runtime_error("Failed to initialize io_uring");
		}
	}

	Dispatcher(const Dispatcher&) = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

	~Dispatcher()
	{
		io_uring_queue_exit(&m_ring);
	}

	void Stop()
	{
		m_isRunning = false;
	}

	void Run()
	{
		while (m_isRunning)
		{
			io_uring_submit_and_wait(&m_ring, 1);

			io_uring_cqe* cqe;
			unsigned head;
			io_uring_for_each_cqe(&m_ring, head, cqe)
			{
				const auto state = static_cast<OperationState*>(io_uring_cqe_get_data(cqe));
				if (state)
				{
					state->res = cqe->res;
					state->completed = true;
					if (state->handle)
					{
						state->handle.resume();
					}
				}

				io_uring_cqe_seen(&m_ring, cqe);
			}
		}
	}

	void QueueOpen(OperationState* state, const char* path, int flags, mode_t mode)
	{
		std::lock_guard lock(m_mutex);
		++m_activeOperations;
		io_uring_sqe* sqe = io_uring_get_sqe(&m_ring);
		io_uring_prep_openat(sqe, AT_FDCWD, path, flags, mode);
		io_uring_sqe_set_data(sqe, state);
	}

	void QueueRead(OperationState* state, int fd, void* buf, size_t count)
	{
		std::lock_guard lock(m_mutex);
		++m_activeOperations;
		io_uring_sqe* sqe = io_uring_get_sqe(&m_ring);
		io_uring_prep_read(sqe, fd, buf, count, 0);
		io_uring_sqe_set_data(sqe, state);
	}

	void QueueWrite(OperationState* state, int fd, const void* buf, size_t count)
	{
		std::lock_guard lock(m_mutex);
		++m_activeOperations;
		io_uring_sqe* sqe = io_uring_get_sqe(&m_ring);
		io_uring_prep_write(sqe, fd, buf, count, 0);
		io_uring_sqe_set_data(sqe, state);
	}

	void QueueClose(OperationState* state, int fd)
	{
		std::lock_guard lock(m_mutex);
		++m_activeOperations;
		io_uring_sqe* sqe = io_uring_get_sqe(&m_ring);
		io_uring_prep_close(sqe, fd);
		io_uring_sqe_set_data(sqe, state);
	}

	void Wait() const
	{
		// std::unique_lock lock(m_mutex);
		// m_cv.wait(lock, [this] { return m_activeOperations == 0; });
	}

private:
	io_uring m_ring{};
	std::atomic_bool m_isRunning = true;
	std::atomic<int> m_activeOperations;
	std::shared_mutex m_mutex;
	std::condition_variable m_cv;
};
