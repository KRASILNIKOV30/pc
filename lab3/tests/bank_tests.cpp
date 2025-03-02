#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../bank.h"
SCENARIO("bank tests")
{
	GIVEN("a bank")
	{
		CHECK_THROWS_AS(Bank(-1), BankOperationError);
		CHECK_NOTHROW(Bank(0));
		Bank bank(10);
	}
}