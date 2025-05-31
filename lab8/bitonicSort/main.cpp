#include "../../lib/timer/Timer.h"
#include "../Random.h"
#include "BitonicSorter.h"
#include <algorithm>
#include <execution>
#include <vector>

constexpr int SIZE = 65536;

void SortOnGPU(std::vector<int> array)
{
	BitonicSorter sorter(array);

	MeasureTime(std::cout, "Bitonic sort", [&] {
		sorter.Sort();
	});
}

void SortOnCPU(std::vector<int> array)
{
	MeasureTime(std::cout, "std::sort", [&] {
		std::sort(std::execution::par, array.begin(), array.end());
	});
}

int main()
{
	std::vector<int> array(SIZE);
	for (auto& i : array)
	{
		i = RandomInt(0, 1000);
	}

	SortOnCPU(array);
	SortOnGPU(array);
}
