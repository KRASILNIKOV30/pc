#include <coroutine>
#include <iostream>
#include <utility>

struct MyAwaiter
{
	bool await_ready()
	{
		return false;
	}

	void await_suspend(const std::coroutine_handle<> handle)
	{
		// handle.resume();
	}

	[[nodiscard]] int await_resume() const noexcept
	{
		return x + y;
	}

	int x;
	int y;
};

class MyTask
{
public:
	struct promise_type;
	using HandleType = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		MyTask get_return_object()
		{
			return MyTask{ HandleType::from_promise(*this) };
		}

		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void unhandled_exception() { std::terminate(); }

		void return_void()
		{
		}

	};

	explicit MyTask(HandleType const handle)
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

	MyTask& operator=(MyTask const& other) = delete;
	MyTask(MyTask const&) = delete;

	~MyTask()
	{
		if (m_handle)
		{
			m_handle.destroy();
		}
	}

	void Resume() const
	{
		if (!m_handle.done())
		{
			m_handle.resume();
		}
	}

private:
	HandleType m_handle;
};

MyTask CoroutineWithAwait(int x, int y)
{
	std::cout << "Before await\n";
	// Приостанавливает работу. При возобновлении возвращает сумму аргументов
	const int result = co_await MyAwaiter{ x, y };
	std::cout << result << std::endl;
	std::cout << "After await" << std::endl;
}

int main()
{
	const auto task = CoroutineWithAwait(30, 12);
	std::cout << "Before resume" << std::endl;
	task.Resume();
	task.Resume();
	std::cout << "After resume" << std::endl;
	CoroutineWithAwait(5, 10).Resume();
	std::cout << "End of main" << std::endl;
}