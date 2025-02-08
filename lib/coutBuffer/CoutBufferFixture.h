#include <sstream>
#include <iostream>

struct CoutBufferFixture
{
	std::stringstream buffer;
	std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

	std::string GetOutput() const
	{
		return buffer.str();
	}

	void ClearOutput()
	{
		buffer.str(std::string());
	}

	~CoutBufferFixture()
	{
		std::cout.rdbuf(old);
	}
};