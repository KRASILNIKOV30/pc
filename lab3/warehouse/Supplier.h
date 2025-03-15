#pragma once
#include "Warehouse.h"

#include <iostream>

class Supplier
{
public:
	Supplier(int id, int maxAmount, Warehouse& warehouse, std::atomic<bool>& stopFlag)
		: m_id(id)
		  , m_maxAmount(maxAmount)
		  , m_warehouse(warehouse)
		  , m_stopFlag(stopFlag)
	{
	}

	void Run()
	{
		while (!m_stopFlag)
		{
			int amount = 1 + rand() % m_maxAmount;
			if (m_warehouse.AddGoods(amount))
			{
				m_totalAdded += amount;
				std::osyncstream(std::cout) << "Supplier " << m_id << " added goods: " << amount << std::endl;
			}
			else
			{
				std::osyncstream(std::cout) << "Supplier " << m_id << " can not added goods" << std::endl;
			}
		}
		std::osyncstream(std::cout) << "Supplier " << m_id << " added goods: " << m_totalAdded << std::endl;
	}

private:
	int m_id;
	int m_maxAmount;
	Warehouse& m_warehouse;
	std::atomic<bool>& m_stopFlag;
	int m_totalAdded = 0;
};