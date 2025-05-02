#pragma once
#include <stop_token>

class StopSource;

class StopToken
{
public:
	friend class StopSource;

	bool StopRequested() const
	{
		return m_stopRequested.test(std::memory_order_acquire);
	}

private:
	explicit StopToken(std::atomic_flag const& stopRequested)
		: m_stopRequested(stopRequested)
	{
	}

private:
	std::atomic_flag const& m_stopRequested;
};

class StopSource
{
public:
	bool RequestStop()
	{
		return !m_stopRequested.test_and_set(std::memory_order_acq_rel);
	}

	StopToken GetToken() const
	{
		return StopToken{ m_stopRequested };
	}

private:
	std::atomic_flag m_stopRequested = ATOMIC_FLAG_INIT;
};
