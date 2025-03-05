#pragma once
#include "Bank.h"
#include <iostream>
#include <syncstream>

class Character;
class CharacterWithCard;

struct Characters
{
	CharacterWithCard* homer = nullptr;
	CharacterWithCard* marge = nullptr;
	Character* lisa = nullptr;
	Character* bart = nullptr;
	CharacterWithCard* apu = nullptr;
	CharacterWithCard* burns = nullptr;
	Character* nelson = nullptr;
	CharacterWithCard* snake = nullptr;
	CharacterWithCard* smithers = nullptr;
};

class Character
{
public:
	explicit Character(Money cash, Characters& characters, bool log)
		: m_cash(cash)
		  , m_log(log)
		  , m_characters(characters)
	{
	}

	[[nodiscard]] Money GetCash() const
	{
		std::shared_lock lock(m_mutex);
		return m_cash;
	}

	[[nodiscard]] bool TransferCash(Character& recipient, Money amount)
	{
		const auto spent = SpendCash(amount);
		if (spent)
		{
			recipient.AddCash(amount);
		}

		return spent;
	}

	[[nodiscard]] bool StealCache(Character& thief, Money amount)
	{
		return TransferCash(thief, amount);
	}

	void Log(std::string const& str) const
	{
		if (m_log)
		{
			std::osyncstream(std::cout) << str << std::endl;
		}
	}

	void Run(const std::atomic_bool& stopFlag)
	{
		while (!stopFlag.load())
		{
			Step(m_characters);
		}
	}

	virtual void Step(Characters const& characters) = 0;

	virtual ~Character() = default;

protected:
	[[nodiscard]] bool SpendCash(Money amount)
	{
		std::unique_lock lock(m_mutex);
		if (m_cash < amount)
		{
			return false;
		}

		m_cash -= amount;
		return true;
	}

	void AddCash(Money amount)
	{
		std::unique_lock lock(m_mutex);
		m_cash += amount;
	}

private:
	bool m_log;
	Money m_cash;
	// посмотреть порядок, в котором захват происходит
	mutable std::shared_mutex m_mutex;
	Characters& m_characters;
};

class CharacterWithCard : public Character
{
public:
	CharacterWithCard(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: Character(cash, characters, log)
		  , m_accountId(accountId)
		  , m_bank(bank)
	{
		DepositMoney(GetCash());
	}

	[[nodiscard]] bool SendMoney(AccountId dstAccountId, Money amount)
	{
		return m_bank.TrySendMoney(m_accountId, dstAccountId, amount);
	}

	[[nodiscard]] bool StealMoney(AccountId thief, Money amount)
	{
		std::unique_lock lock(m_mutex);
		return m_bank.TrySendMoney(m_accountId, thief, amount);
	}

	[[nodiscard]] AccountId GetAccountId() const
	{
		std::shared_lock lock(m_mutex);
		return m_accountId;
	}

	void CloseAccount()
	{
		AddCash(m_bank.CloseAccount(m_accountId));
	}

	void OpenAccount()
	{
		std::unique_lock lock(m_mutex);
		m_accountId = m_bank.OpenAccount();
	}

	bool DepositMoney(Money amount)
	{
		std::unique_lock lock(m_mutex);
		const auto success = SpendCash(amount);
		// тут можно украсть (исправлено)
		if (success)
		{
			m_bank.DepositMoney(m_accountId, amount);
		}

		return success;
	}

	[[nodiscard]] bool WithdrawMoney(Money amount)
	{
		const auto success = m_bank.TryWithdrawMoney(m_accountId, amount);
		if (success)
		{
			AddCash(amount);
		}
		return success;
	}

	[[nodiscard]] Money GetAccountBalance() const
	{
		return m_bank.GetAccountBalance(m_accountId);
	}

private:
	mutable std::shared_mutex m_mutex;
	AccountId m_accountId;
	Bank& m_bank;
};
