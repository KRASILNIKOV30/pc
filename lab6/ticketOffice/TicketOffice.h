#pragma once
#include <atomic>

class TicketOffice
{
public:
	explicit TicketOffice(const int numTickets)
		: m_numTickets(numTickets)
	{
		if (m_numTickets < 0)
		{
			throw std::invalid_argument("numTickets must be positive");
		}

	}

	TicketOffice(const TicketOffice&) = delete;
	TicketOffice& operator=(const TicketOffice&) = delete;

	/**
	 * Выполняет продажу билета.
	 * Возвращает количество фактически проданных билетов.
	 * Если ticketsToBuy <= 0, выбрасывается исключение std::invalid_argument
	 */
	int SellTickets(const int ticketsToBuy)
	{
		if (ticketsToBuy <= 0)
		{
			throw std::invalid_argument("ticketsToBuy must be greater than 0");
		}

		int currentTickets = m_numTickets.load(std::memory_order_acquire);
		int newCount;
		int ticketsSold;

		do
		{
			if (currentTickets == 0)
			{
				return 0;
			}

			ticketsSold = std::min(currentTickets, ticketsToBuy);
			newCount = currentTickets - ticketsSold;
		} while (!m_numTickets.compare_exchange_weak(currentTickets, newCount, std::memory_order_release, std::memory_order_relaxed));

		return ticketsSold;
	}

	[[nodiscard]] int GetTicketsLeft() const noexcept
	{
		return m_numTickets.load(std::memory_order_relaxed);
	}

private:
	// Количество билетов должно быть атомарным
	std::atomic<int> m_numTickets;
};