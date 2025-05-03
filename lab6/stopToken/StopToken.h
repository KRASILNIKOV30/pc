#pragma once
#include <memory>
#include <stop_token>

class StopSource;

class StopToken
{
public:
	friend class StopSource;

	[[nodiscard]] bool StopRequested() const
	{
		return m_stopRequested->test(std::memory_order_acquire);
	}

private:
	explicit StopToken(std::shared_ptr<std::atomic_flag> const& stopRequested)
		: m_stopRequested(stopRequested)
	{
	}

private:
	std::shared_ptr<std::atomic_flag> m_stopRequested;
};

class StopSource
{
public:
	StopSource() = default;
	StopSource(StopToken const& stopToken) = delete;
	StopSource& operator=(StopToken const& stopToken) = delete;

	bool RequestStop()
	{
		return !m_stopRequested->test_and_set(std::memory_order_acq_rel);
	}

	[[nodiscard]] StopToken GetToken() const
	{
		return StopToken{ m_stopRequested };
	}

private:
	std::shared_ptr<std::atomic_flag> m_stopRequested = std::make_shared<std::atomic_flag>();
};
