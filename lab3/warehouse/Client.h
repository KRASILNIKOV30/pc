#pragma once
#include "Warehouse.h"
#include <iostream>

class Client
{
public:
	Client(int id, int maxAmount, Warehouse& warehouse, std::atomic<bool>& stopFlag)
		: m_id(id), m_maxAmount(maxAmount), m_warehouse(warehouse), m_stopFlag(stopFlag)
	{
	}

	void Run()
	{
		while (!m_stopFlag)
		{
			int amount = 1 + rand() % m_maxAmount;
			if (m_warehouse.RemoveGoods(amount))
			{
				m_totalRemoved += amount;
				std::osyncstream(std::cout) << "Client " << m_id << " removed goods: " << amount << std::endl;
			}
			else
			{
				std::osyncstream(std::cout) << "Clients " << m_id << " can not remove goods" << std::endl;
			}
		}
		std::osyncstream(std::cout) << "Client " << m_id << " removed goods: " << m_totalRemoved << std::endl;
	}

private:
	int m_id;
	int m_maxAmount;
	Warehouse& m_warehouse;
	std::atomic<bool>& m_stopFlag;
	int m_totalRemoved = 0;
};