#include "Auditor.h"
#include "Client.h"
#include "Supplier.h"
#include "Warehouse.h"
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

constexpr int MAX_AMOUNT = 10;

inline std::atomic stopFlag = false;

inline void SignalHandler(int signum)
{
	stopFlag.store(true);
}

struct Args
{
	int numSuppliers;
	int numClients;
	int numAuditors;
};

Args ParseArgs(int argc, char* argv[])
{
	if (argc != 4)
	{
		throw std::invalid_argument("Invalid number of arguments");
	}

	return Args{
		.numSuppliers = std::stoi(argv[1]),
		.numClients = std::stoi(argv[2]),
		.numAuditors = std::stoi(argv[3]),
	};
}

template <typename T>
void AddClients(int n, std::vector<std::jthread>& v, Warehouse& wh, std::atomic<bool>& stopFlag)
{
	for (int i = 0; i < n; ++i)
	{
		v.emplace_back([&] {
			T client(i, MAX_AMOUNT, wh, stopFlag);
			client.Run();
		});
	}
}

int main(int argc, char* argv[])
{
	Warehouse wh(100);
	try
	{
		const auto [numSuppliers, numClients, numAuditors] = ParseArgs(argc, argv);
		std::signal(SIGINT, SignalHandler);
		std::signal(SIGTERM, SignalHandler);

		std::vector<std::jthread> suppliers;
		std::vector<std::jthread> clients;
		std::vector<std::jthread> auditors;

		AddClients<Supplier>(numSuppliers, suppliers, wh, stopFlag);
		AddClients<Client>(numClients, clients, wh, stopFlag);
		AddClients<Auditor>(numAuditors, auditors, wh, stopFlag);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}

	std::cout << "Final stock: " << wh.GetStock() << std::endl;

	return 0;
}
