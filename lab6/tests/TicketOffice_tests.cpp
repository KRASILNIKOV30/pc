#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../ticketOffice/TicketOffice.h"
#include <thread>

TEST_CASE("TicketOffice initialization")
{
	SECTION("Positive initial tickets")
	{
		const TicketOffice office(100);
		REQUIRE(office.GetTicketsLeft() == 100);
	}

	SECTION("Zero initial tickets")
	{
		const TicketOffice office(0);
		REQUIRE(office.GetTicketsLeft() == 0);
	}

	SECTION("Negative initial tickets throws")
	{
		REQUIRE_THROWS_AS(TicketOffice(-1), std::invalid_argument);
	}
}

TEST_CASE("Single-threaded ticket sales")
{
	TicketOffice office(100);

	SECTION("Successful sale returns requested tickets")
	{
		REQUIRE(office.SellTickets(10) == 10);
		REQUIRE(office.GetTicketsLeft() == 90);
	}

	SECTION("Partial sale when not enough tickets")
	{
		REQUIRE(office.SellTickets(120) == 100);
		REQUIRE(office.GetTicketsLeft() == 0);
	}

	SECTION("Zero tickets request throws")
	{
		REQUIRE_THROWS_AS(office.SellTickets(0), std::invalid_argument);
	}

	SECTION("Negative tickets request throws")
	{
		REQUIRE_THROWS_AS(office.SellTickets(-5), std::invalid_argument);
	}

	SECTION("Multiple sales reduce tickets correctly")
	{
		REQUIRE(office.SellTickets(30) == 30);
		REQUIRE(office.GetTicketsLeft() == 70);
		REQUIRE(office.SellTickets(50) == 50);
		REQUIRE(office.GetTicketsLeft() == 20);
		REQUIRE(office.SellTickets(30) == 20);
		REQUIRE(office.GetTicketsLeft() == 0);
	}

	SECTION("Selling from empty office returns zero")
	{
		TicketOffice empty(0);
		REQUIRE(empty.SellTickets(10) == 0);
	}
}

TEST_CASE("Multi-threaded ticket sales")
{
	constexpr int initialTickets = 10'000;
	constexpr int threadCount = 10;

	TicketOffice office(initialTickets);
	std::vector<int> ticketsSold(threadCount);

	SECTION("Concurrent sales don't oversell tickets")
	{
		constexpr int ticketsPerThread = 1500;
		{
			std::vector<std::jthread> threads;
			for (int i = 0; i < threadCount; ++i)
			{
				threads.emplace_back([&, i] {
					ticketsSold[i] = office.SellTickets(ticketsPerThread);
				});
			}
		}

		int totalSold = 0;
		for (int sold : ticketsSold)
		{
			totalSold += sold;
			REQUIRE(sold >= 0);
			REQUIRE(sold <= ticketsPerThread);
		}

		REQUIRE(totalSold == initialTickets);
		REQUIRE(office.GetTicketsLeft() == 0);
	}

	SECTION("Mixed operations from multiple threads")
	{
		{
			std::atomic start(false);
			std::vector<std::jthread> threads;
			for (int i = 0; i < threadCount; ++i)
			{
				threads.emplace_back([&, i] {
					while (!start.load())
					{
					}

					for (int j = 0; j < 1'000'000; ++j)
					{
						if (j % 2 == 0)
						{
							ticketsSold[i] += office.SellTickets(1);
						}
						else
						{
							const int remaining = office.GetTicketsLeft();
							REQUIRE(remaining >= 0);
							REQUIRE(remaining <= initialTickets);
						}
					}
				});
			}
			start.store(true);
		}

		int totalSold = 0;
		for (int i = 0; i < threadCount; i++)
		{
			totalSold += ticketsSold[i];
		}

		REQUIRE(totalSold <= initialTickets);
		REQUIRE(office.GetTicketsLeft() == (initialTickets - totalSold));
	}
}

TEST_CASE("Edge cases")
{
	SECTION("Massive concurrent sales")
	{
		constexpr int totalTickets = 10000;
		TicketOffice office(totalTickets);
		std::vector<std::jthread> threads;
		std::atomic<int> totalSold(0);

		for (int i = 0; i < 100; ++i)
		{
			threads.emplace_back([&] {
				int sold = office.SellTickets(150);
				totalSold += sold;
			});
		}

		REQUIRE(totalSold == totalTickets);
		REQUIRE(office.GetTicketsLeft() == 0);
	}
}


