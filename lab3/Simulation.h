#pragma once
#include "Bank.h"
#include "Characters.h"
#include <memory>
#include <vector>

class Simulation
{
public:
	Simulation(bool multiThreaded, bool log)
		: m_multiThreaded(multiThreaded)
		  , m_log(log)
	{
		m_bank = std::make_unique<Bank>(m_initialCash); // Начальное количество наличных
		m_homerAccount = m_bank->OpenAccount();
		m_margeAccount = m_bank->OpenAccount();
		m_apuAccount = m_bank->OpenAccount();
		m_burnsAccount = m_bank->OpenAccount();
		m_snakeAccount = m_bank->OpenAccount();
		m_smithersAccount = m_bank->OpenAccount();

		m_homer = std::make_unique<Homer>(10'000, m_homerAccount, m_characters, *m_bank, m_log);
		m_marge = std::make_unique<Marge>(100, m_margeAccount, m_characters, *m_bank, m_log);
		m_bart = std::make_unique<Bart>(100, m_characters, m_log);
		m_lisa = std::make_unique<Lisa>(100, m_characters, m_log);
		m_apu = std::make_unique<Apu>(200, m_apuAccount, m_characters, *m_bank, m_log);
		m_burns = std::make_unique<Burns>(100'000, m_burnsAccount, m_characters, *m_bank, m_log);
		m_nelson = std::make_unique<Nelson>(0, m_characters, m_log);
		m_snake = std::make_unique<Snake>(0, m_snakeAccount, m_characters, *m_bank, m_log);
		m_smithers = std::make_unique<Smithers>(0, m_smithersAccount, m_characters, *m_bank, m_log);

		m_characters.homer = m_homer.get();
		m_characters.marge = m_marge.get();
		m_characters.bart = m_bart.get();
		m_characters.lisa = m_lisa.get();
		m_characters.apu = m_apu.get();
		m_characters.burns = m_burns.get();
		m_characters.nelson = m_nelson.get();
		m_characters.snake = m_snake.get();
		m_characters.smithers = m_smithers.get();
	}

	void Start()
	{
		if (m_multiThreaded)
		{
			std::vector<std::jthread> threads;
			threads.emplace_back(&Homer::Run, m_homer.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Marge::Run, m_marge.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Bart::Run, m_bart.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Lisa::Run, m_lisa.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Apu::Run, m_apu.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Burns::Run, m_burns.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Nelson::Run, m_nelson.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Snake::Run, m_snake.get(), std::ref(m_stopFlag));
			threads.emplace_back(&Smithers::Run, m_smithers.get(), std::ref(m_stopFlag));
		}
		else
		{
			while (!m_stopFlag.load())
			{
				m_homer->Step(m_characters);
				m_marge->Step(m_characters);
				m_bart->Step(m_characters);
				m_lisa->Step(m_characters);
				m_apu->Step(m_characters);
				m_burns->Step(m_characters);
				m_nelson->Step(m_characters);
				m_snake->Step(m_characters);
				m_smithers->Step(m_characters);
			}
		}

		std::cout << "Total bank operations: " << m_bank->GetOperationsCount() << std::endl;
		std::cout << (IsConsistent() ? "OK" : "FAIL") << std::endl;
	}

	void Stop()
	{
		m_stopFlag.store(true);
	}

private:
	[[nodiscard]] bool IsConsistent() const
	{
		const Money bankCash = m_bank->GetCash();

		const Money totalCharacterCash =
			m_homer->GetCash() +
			m_marge->GetCash() +
			m_bart->GetCash() +
			m_lisa->GetCash() +
			m_apu->GetCash() +
			m_burns->GetCash() +
			m_nelson->GetCash() +
			m_snake->GetCash() +
			m_smithers->GetCash();

		const Money totalCharacterBalance =
			m_homer->GetAccountBalance() +
			m_marge->GetAccountBalance() +
			m_apu->GetAccountBalance() +
			m_burns->GetAccountBalance() +
			m_snake->GetAccountBalance() +
			m_smithers->GetAccountBalance();
		const Money totalMoney = totalCharacterBalance + totalCharacterCash;
		if (m_log)
		{
			std::osyncstream(std::cout) << "Bank cash: " << bankCash << "\n"
				<< "Total accounts: " << totalCharacterBalance << "\n"
				<< "Total character cash: " << totalCharacterCash << "\n"
				<< "Initial cash: " << m_initialCash << "\n"
				<< "Total money: " << totalMoney << "\n";
		}

		return totalMoney == m_initialCash;
	}

private:
	bool m_multiThreaded;
	bool m_log;
	std::atomic<bool> m_stopFlag;
	std::unique_ptr<Bank> m_bank;
	AccountId m_homerAccount;
	AccountId m_margeAccount;
	AccountId m_apuAccount;
	AccountId m_burnsAccount;
	AccountId m_snakeAccount;
	AccountId m_smithersAccount;

	std::unique_ptr<Homer> m_homer;
	std::unique_ptr<Marge> m_marge;
	std::unique_ptr<Bart> m_bart;
	std::unique_ptr<Lisa> m_lisa;
	std::unique_ptr<Apu> m_apu;
	std::unique_ptr<Burns> m_burns;
	std::unique_ptr<Nelson> m_nelson;
	std::unique_ptr<Snake> m_snake;
	std::unique_ptr<Smithers> m_smithers;

	Characters m_characters;

	Money m_initialCash = 110'500;
};