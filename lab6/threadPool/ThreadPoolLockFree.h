#pragma once
#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <boost/lockfree/queue.hpp>
#include <thread>
#include <vector>
#include <stop_token>

constexpr int QUEUE_SIZE = 1024;

class ThreadPoolLockFree
{
public:
	using Task = std::function<void()>;

	explicit ThreadPoolLockFree(unsigned threadsNum = std::thread::hardware_concurrency())
	{
		if (threadsNum == 0)
		{
			threadsNum = 1;
		}

		m_workers.reserve(threadsNum);
		for (unsigned i = 0; i < threadsNum; ++i)
		{
			m_workers.emplace_back([this](const std::stop_token& stopToken) {
				WorkerThread(stopToken);
			});
		}
	}

	ThreadPoolLockFree(const ThreadPoolLockFree&) = delete;
	ThreadPoolLockFree& operator=(const ThreadPoolLockFree&) = delete;

	~ThreadPoolLockFree()
	{
		Stop();
	}

	bool Dispatch(Task task)
	{
		if (m_stopFlag.load(std::memory_order_relaxed))
		{
			return false;
		}

		auto* taskPtr = new Task(std::move(task));
		if (!m_tasks.push(taskPtr))
		{
			delete taskPtr;
			return false;
		}

		m_condVar.notify_one();
		return true;
	}

	void Wait()
	{
		std::unique_lock lock(m_condMutex);
		m_condVar.wait(lock, [this]() { return m_tasks.empty() && m_activeTasks == 0; });
	}

	void Stop()
	{
		if (m_stopFlag.exchange(true))
		{
			return;
		}

		m_condVar.notify_all();

		for (auto& worker : m_workers)
		{
			if (worker.joinable())
			{
				worker.request_stop();
			}
		}
	}

private:
	void WorkerThread(const std::stop_token& stopToken)
	{
		while (!stopToken.stop_requested())
		{
			Task* task = nullptr;

			if (!m_tasks.pop(task))
			{
				if (m_stopFlag.load(std::memory_order_relaxed))
				{
					return;
				}

				std::unique_lock lock(m_condMutex);
				m_condVar.wait(lock, [this, &stopToken]() {
					return stopToken.stop_requested() ||
						m_stopFlag.load(std::memory_order_relaxed) ||
						!m_tasks.empty();
				});
				continue;
			}

			m_activeTasks.fetch_add(1, std::memory_order_release);

			try
			{
				(*task)();
			}
			catch (const std::exception& e)
			{
				std::cerr << "Exception in worker thread: " << e.what() << '\n';
			}

			delete task;
			m_activeTasks.fetch_sub(1, std::memory_order_release);
			if (m_tasks.empty() && m_activeTasks == 0)
			{
				m_condVar.notify_all();
			}
		}
	}

	boost::lockfree::queue<Task*> m_tasks{ QUEUE_SIZE };
	std::atomic<bool> m_stopFlag{ false };
	std::atomic<size_t> m_activeTasks{ 0 };
	std::vector<std::jthread> m_workers;
	std::mutex m_condMutex;
	std::condition_variable m_condVar;
};