#pragma once
#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPoolLockBased
{
public:
	using Task = std::function<void()>;

	explicit ThreadPoolLockBased(const unsigned threadsNum)
	{
		for (unsigned i = 0; i < threadsNum; ++i)
		{
			m_workers.emplace_back([this](const std::stop_token& stopToken) {
				WorkerThread(stopToken);
			});
		}
	}

	ThreadPoolLockBased(const ThreadPoolLockBased&) = delete;
	ThreadPoolLockBased& operator=(const ThreadPoolLockBased&) = delete;

	~ThreadPoolLockBased()
	{
		Stop();
	}

	void Dispatch(Task task)
	{
		{
			std::lock_guard lock(m_queueMutex);
			if (m_stopFlag)
			{
				return;
			}
			m_tasks.push(std::move(task));
		}
		m_condVar.notify_one();
	}

	void Wait()
	{
		std::unique_lock lock(m_queueMutex);
		m_condVar.wait(lock, [this]() { return m_tasks.empty() && m_activeTasks == 0; });
	}

	void Stop()
	{
		m_stopFlag = true;
		m_condVar.notify_all();
	}

private:
	void WorkerThread(const std::stop_token& stopToken)
	{
		while (!stopToken.stop_requested())
		{
			Task task;
			{
				std::unique_lock lock(m_queueMutex);
				m_condVar.wait(lock, [this]() { return m_stopFlag || !m_tasks.empty(); });

				if (m_stopFlag && m_tasks.empty())
				{
					return;
				}

				task = std::move(m_tasks.front());
				m_tasks.pop();
				m_activeTasks.fetch_add(1);
			}

			try
			{
				task();
			}
			catch (const std::exception& e)
			{
				std::cerr << "Exception caught in background task: " << e.what() << std::endl;
			}

			m_activeTasks.fetch_sub(1);
			if (m_tasks.empty() && m_activeTasks == 0)
			{
				m_condVar.notify_all();
			}
		}
	}

	std::queue<Task> m_tasks;
	std::mutex m_queueMutex;
	std::condition_variable m_condVar;
	std::atomic<bool> m_stopFlag;
	std::atomic<int> m_activeTasks = 0;
	std::vector<std::jthread> m_workers;
};
