#pragma once
#include "CharactersBase.h"

#include <thread>

class Homer final : public CharacterWithCard
{
public:
	Homer(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: CharacterWithCard(cash, accountId, characters, bank, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (SendMoney(characters.marge->GetAccountId(), 500))
		{
			Log("Homer sent money to Marge.");
		}

		if (SendMoney(characters.burns->GetAccountId(), 200))
		{
			Log("Homer paid for electricity.");
		}

		if (WithdrawMoney(100))
		{
			Log("Homer withdrew cash for kids.");
			if (TransferCash(*characters.bart, 50))
			{
				Log("Homer transfer cash to Bart");
			}
			if (TransferCash(*characters.lisa, 50))
			{
				Log("Homer transfer cash to Lisa");
			}
		}
	}
};

class Marge final : public CharacterWithCard
{
public:
	Marge(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: CharacterWithCard(cash, accountId, characters, bank, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (SendMoney(characters.apu->GetAccountId(), 300))
		{
			Log("Marge bought groceries from Apu.");
		}
	}
};

class Bart final : public Character
{
public:
	Bart(Money cash, Characters& characters, bool log)
		: Character(cash, characters, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (TransferCash(*characters.apu, 10))
		{
			Log("Bart spent cash at Apu's store.");
		}
	}
};

class Lisa final : public Character
{
public:
	Lisa(Money cash, Characters& characters, bool log)
		: Character(cash, characters, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (TransferCash(*characters.apu, 10))
		{
			Log("Lisa spent cash at Apu's store.");
		}
	}
};

class Apu final : public CharacterWithCard
{
public:
	Apu(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: CharacterWithCard(cash, accountId, characters, bank, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (DepositMoney(GetCash()))
		{
			Log("Apu deposited cash to bank.");
		}

		if (SendMoney(characters.burns->GetAccountId(), 100))
		{
			Log("Apu paid for electricity.");
		}
	}
};

class Burns final : public CharacterWithCard
{
public:
	Burns(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: CharacterWithCard(cash, accountId, characters, bank, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (SendMoney(characters.homer->GetAccountId(), 1000))
		{
			Log("Burns paid salary to Homer.");
		}

		if (SendMoney(characters.smithers->GetAccountId(), 500))
		{
			Log("Burns paid salary to Smithers.");
		}
	}
};

class Nelson final : public Character
{
public:
	Nelson(Money cash, Characters& characters, bool log)
		: Character(cash, characters, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (characters.bart->StealCache(*this, 20))
		{
			Log("Nelson stole cash from Bart.");
		}

		if (TransferCash(*characters.apu, 10))
		{
			Log("Nelson bought cigarettes from Apu.");
		}
	}
};

class Snake final : public CharacterWithCard
{
public:
	Snake(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: CharacterWithCard(cash, accountId, characters, bank, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (characters.homer->StealMoney(GetAccountId(), 100))
		{
			Log("Snake stole money from Homer.");
		}

		if (SendMoney(characters.apu->GetAccountId(), 50))
		{
			Log("Snake bought groceries from Apu.");
		}
	}
};

class Smithers final : public CharacterWithCard
{
public:
	Smithers(Money cash, AccountId accountId, Characters& characters, Bank& bank, bool log)
		: CharacterWithCard(cash, accountId, characters, bank, log)
	{
	}

	void Step(const Characters& characters) override
	{
		if (SendMoney(characters.apu->GetAccountId(), 200))
		{
			Log("Smithers bought groceries from Apu.");
		}

		CloseAccount();
		OpenAccount();
		Log("Smithers closed and reopened his account.");
	}
};