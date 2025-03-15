#pragma once
#include "Warehouse.h"
#include <iostream>
#include <syncstream>

class Auditor
{
public:
	Auditor(int id, int maxAmount, Warehouse& warehouse, std::atomic<bool>& stopFlag)
		: m_id(id), m_warehouse(warehouse), m_stopFlag(stopFlag)
	{
	}

	void Run() const
	{
		while (!m_stopFlag)
		{
			int stock = m_warehouse.GetStock();
			std::osyncstream(std::cout) << "Auditor " << m_id << " reports stock: " << stock << " goods.\n";
		}
	}

private:
	int m_id;
	Warehouse& m_warehouse;
	std::atomic<bool>& m_stopFlag;
};