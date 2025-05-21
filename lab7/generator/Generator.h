#pragma once
#include <cassert>
#include <coroutine>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <optional>

template <typename T>
class Generator
{
public:
	struct promise_type;
	using CoroHandle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		std::optional<T> m_value = std::nullopt;
		std::exception_ptr m_exception;

		template <std::convertible_to<T> U>
		std::suspend_always yield_value(U&& value)
		{
			m_value.emplace(std::forward<U>(value));
			return {};
		}

		[[nodiscard]] bool HasException() const noexcept
		{
			return m_exception != nullptr;
		}

		[[nodiscard]] bool HasValue() const noexcept
		{
			return m_value.has_value();
		}

		void ThrowIfException() const
		{
			if (HasException())
			{
				std::rethrow_exception(m_exception);
			}
		}

		T& GetValue()
		{
			ThrowIfException();
			if (HasValue())
			{
				return m_value.value();
			}
			throw std::logic_error("GetValue() is called without resume");
		}

		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }

		void unhandled_exception()
		{
			m_exception = std::current_exception();
		}

		Generator get_return_object()
		{
			return Generator{ CoroHandle::from_promise(*this) };
		}

		void return_void()
		{
		}
	};

	explicit Generator(CoroHandle handle)
		: m_handle(handle)
	{
	}

	// Генератор нельзя копировать. Только перемещать.
	Generator(const Generator& other) = delete;
	Generator& operator=(const Generator& other) = delete;

	Generator(Generator&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}

	Generator& operator=(Generator&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	~Generator()
	{
		if (m_handle)
		{
			m_handle.destroy();
		}
	}

	T& GetValue()
	{
		assert(m_handle);
		return m_handle.promise().GetValue();
	}

	class Iterator
	{
	public:
		// Даём генератору доступ к нашему приватному конструктору
		friend class Generator;

		using iterator_category = std::input_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		Iterator() = default;

		// Поддерживается только перемещение итератора
		Iterator(const Iterator&) = delete;
		Iterator& operator=(const Iterator&) = delete;

		Iterator(Iterator&& other) noexcept
			: m_handle(std::exchange(other.m_handle, nullptr))
		{
		}

		Iterator& operator=(Iterator&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				if (m_handle)
				{
					m_handle.destroy();
				}
				m_handle = std::exchange(other.m_handle, nullptr);
			}
			return *this;
		}

		~Iterator() = default;

		reference operator*() const
		{
			assert(m_handle);
			return m_handle.promise().GetValue();
		}

		pointer operator->() const
		{
			return &operator*();
		}

		Iterator& operator++()
		{
			assert(m_handle);
			assert(!m_handle.done());

			m_handle.resume();
			if (m_handle.done())
			{
				// Если корутина завершилась, то нужно проверить, не выбросила ли она исключение
				auto handle = std::exchange(m_handle, nullptr);
				handle.promise().ThrowIfException();
			}

			return *this;
		}

		bool operator==(std::default_sentinel_t) const noexcept
		{
			return m_handle == nullptr;
		}

	private:
		explicit Iterator(CoroHandle handle)
			: m_handle(handle)
		{
			operator++();
		}

		CoroHandle m_handle = nullptr;
	};

	Iterator begin()
	{
		return Iterator{ m_handle };
	}

	std::default_sentinel_t end()
	{
		return std::default_sentinel;
	}

private:
	CoroHandle m_handle = nullptr;
};
