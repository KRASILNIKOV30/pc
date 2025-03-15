#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../Bank.h"

SCENARIO("Bank initialization", "[Bank]")
{
	GIVEN("A bank with positive initial cash")
	{
		constexpr Money initialCash = 1000;
		const Bank bank(initialCash);

		THEN("The bank should have the correct amount of cash")
		{
			REQUIRE(bank.GetCash() == initialCash);
		}
	}

	GIVEN("A bank with negative initial cash")
	{
		Money initialCash = -1000;

		THEN("The bank should throw BankOperationError")
		{
			REQUIRE_THROWS_AS(Bank(initialCash), BankOperationError);
		}
	}
}

SCENARIO("Opening and closing accounts", "[Bank]")
{
	GIVEN("A bank with some initial cash")
	{
		Money initialCash = 1000;
		Bank bank(initialCash);

		WHEN("An account is opened")
		{
			AccountId accountId = bank.OpenAccount();

			THEN("The account should have a zero balance")
			{
				REQUIRE(bank.GetAccountBalance(accountId) == 0);
			}

			THEN("The account can be closed")
			{
				Money balance = bank.CloseAccount(accountId);
				REQUIRE(balance == 0);
				REQUIRE(bank.GetCash() == initialCash);
			}
		}
	}
}

SCENARIO("Depositing money", "[Bank]")
{
	GIVEN("A bank with some initial cash and an account")
	{
		Money initialCash = 1000;
		Bank bank(initialCash);
		AccountId accountId = bank.OpenAccount();

		WHEN("Money is deposited into the account")
		{
			Money depositAmount = 500;
			bank.DepositMoney(accountId, depositAmount);

			THEN("The account balance should increase by the deposited amount")
			{
				REQUIRE(bank.GetAccountBalance(accountId) == depositAmount);
			}

			THEN("The bank's cash should decrease by the deposited amount")
			{
				REQUIRE(bank.GetCash() == initialCash - depositAmount);
			}
		}

		WHEN("Trying to deposit more money than the bank has in cash")
		{
			Money depositAmount = initialCash + 100;

			THEN("The bank should throw BankOperationError")
			{
				REQUIRE_THROWS_AS(bank.DepositMoney(accountId, depositAmount), BankOperationError);
			}
		}
	}
}

SCENARIO("Withdrawing money", "[Bank]")
{
	GIVEN("A bank with some initial cash and an account with money")
	{
		Money initialCash = 1000;
		Bank bank(initialCash);
		AccountId accountId = bank.OpenAccount();
		Money depositAmount = 500;
		bank.DepositMoney(accountId, depositAmount);

		WHEN("Money is withdrawn from the account")
		{
			Money withdrawAmount = 200;
			bank.WithdrawMoney(accountId, withdrawAmount);

			THEN("The account balance should decrease by the withdrawn amount")
			{
				REQUIRE(bank.GetAccountBalance(accountId) == depositAmount - withdrawAmount);
			}

			THEN("The bank's cash should increase by the withdrawn amount")
			{
				REQUIRE(bank.GetCash() == initialCash - depositAmount + withdrawAmount);
			}
		}

		WHEN("Trying to withdraw more money than the account has")
		{
			Money withdrawAmount = depositAmount + 100;

			THEN("The bank should throw BankOperationError")
			{
				REQUIRE_THROWS_AS(bank.WithdrawMoney(accountId, withdrawAmount), BankOperationError);
			}
		}
	}
}

SCENARIO("Sending money between accounts", "[Bank]")
{
	GIVEN("A bank with some initial cash and two accounts")
	{
		Money initialCash = 1000;
		Bank bank(initialCash);
		AccountId accountId1 = bank.OpenAccount();
		AccountId accountId2 = bank.OpenAccount();
		Money depositAmount = 500;
		bank.DepositMoney(accountId1, depositAmount);

		WHEN("Money is sent from one account to another")
		{
			Money sendAmount = 200;
			bank.SendMoney(accountId1, accountId2, sendAmount);

			THEN("The source account balance should decrease by the sent amount")
			{
				REQUIRE(bank.GetAccountBalance(accountId1) == depositAmount - sendAmount);
			}

			THEN("The destination account balance should increase by the sent amount")
			{
				REQUIRE(bank.GetAccountBalance(accountId2) == sendAmount);
			}
		}

		WHEN("Trying to send more money than the source account has")
		{
			Money sendAmount = depositAmount + 100;

			THEN("The bank should throw BankOperationError")
			{
				REQUIRE_THROWS_AS(bank.SendMoney(accountId1, accountId2, sendAmount), BankOperationError);
			}
		}
	}
}

SCENARIO("Counting operations", "[Bank]")
{
	GIVEN("A bank with some initial cash")
	{
		Money initialCash = 1000;
		Bank bank(initialCash);

		WHEN("Several operations are performed")
		{
			AccountId accountId1 = bank.OpenAccount();
			AccountId accountId2 = bank.OpenAccount();
			bank.DepositMoney(accountId1, 500);
			bank.WithdrawMoney(accountId1, 200);
			bank.SendMoney(accountId1, accountId2, 100);

			THEN("The operation count should reflect the number of operations performed")
			{
				REQUIRE(bank.GetOperationsCount() == 5);
			}
		}
	}
}