#pragma once
#include <mutex>
#include <condition_variable>
#include <shared_mutex>

class Warehouse
{
public:
	explicit Warehouse(int capacity)
		: m_capacity(capacity)
	{
	}

	bool AddGoods(int amount)
	{
		std::unique_lock lock(m_mutex);
		if (m_currentStock + amount > m_capacity)
		{
			return false;
		}
		m_currentStock += amount;
		return true;
	}

	bool RemoveGoods(int amount)
	{
		std::unique_lock lock(m_mutex);
		if (m_currentStock < amount)
		{
			return false;
		}
		m_currentStock -= amount;
		return true;
	}

	int GetStock() const
	{
		std::shared_lock lock(m_mutex);
		return m_currentStock;
	}

private:
	mutable std::shared_mutex m_mutex;
	int m_capacity;
	int m_currentStock = 0;
};
