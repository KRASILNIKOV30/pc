#pragma once
#include <coroutine>
#include <string>

class MyTask
{
	struct promise_type;
	using HandleType = std::coroutine_handle<promise_type>;

public:
	struct promise_type
	{
		MyTask get_return_object()
		{
			return MyTask(HandleType::from_promise(*this));
		}

		std::suspend_never initial_suspend() noexcept { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_value(const std::string& val) { m_value = val; }
		void unhandled_exception() { std::terminate(); }

		std::string m_value;
	};

	[[nodiscard]] std::string GetResult() const
	{
		return m_handle.promise().m_value;
	}

	explicit MyTask(const std::coroutine_handle<promise_type> handle)
		: m_handle(handle)
	{
	}

	MyTask(MyTask&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}

	MyTask& operator=(MyTask&& other) noexcept
	{
		if (std::addressof(other) != this)
		{
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	MyTask(const MyTask&) = delete;
	MyTask& operator=(const MyTask&) = delete;

	~MyTask()
	{
		if (m_handle)
		{
			m_handle.destroy();
		}
	}

private:
	std::coroutine_handle<promise_type> m_handle;
};
