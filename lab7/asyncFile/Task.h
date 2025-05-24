#pragma once
#include <coroutine>
#include <exception>

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

		static std::suspend_never initial_suspend() { return {}; }
		static std::suspend_always final_suspend() noexcept { return {}; }

		static void return_void()
		{
		}

		static void unhandled_exception() { std::terminate(); }
	};

	struct Awaiter
	{
		handle_type coro;

		static bool await_ready() noexcept { return false; }

		void await_suspend(const std::coroutine_handle<> awaiting) const noexcept
		{
			coro.resume();
			awaiting.resume();
		}

		static void await_resume() noexcept
		{
		}
	};

	explicit Task(handle_type h)
		: coro(h)
	{
	}

	[[nodiscard]] bool Done() const noexcept
	{
		return !coro || coro.done();
	}

	void Wait() const noexcept
	{
		while (!Done())
		{
			coro.resume();
		}
	}


	~Task()
	{
		if (coro)
		{
			coro.destroy();
		}
	}

	Awaiter operator co_await() const
	{
		return Awaiter{ coro };
	}

private:
	handle_type coro;
};