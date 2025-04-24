#pragma once
#include <atomic>

template <typename T>
class AtomicMax
{
public:
	explicit AtomicMax(T value)
		: m_value(value)
	{
	}

	void Update(T newValue) noexcept
	{
		auto currentValue = m_value.load(std::memory_order_relaxed);

		while (newValue > currentValue &&
			!m_value.compare_exchange_weak(currentValue, newValue,
				std::memory_order_release,
				std::memory_order_relaxed))
		{
		}
	}

	T GetValue() const noexcept
	{
		return m_value.load(std::memory_order_acquire);
	}

private:
	std::atomic<T> m_value;
};