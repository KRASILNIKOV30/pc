#pragma once
#include "../gpuRunner/GPURunner.h"
#include <algorithm>
#include <vector>
#include <wx/wx.h>

static inline const char* KERNEL_SOURCE = R"(
void compareSwap(__global int* array, int i, int j, int dir) {
    if ((array[i] > array[j]) == dir) {
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

__kernel void bitonic_sort(
    const int size,
    const int stage,
    const int passOfStage,
    const int direction,
	__global int* array,
	__global int* out
) {
    int tid = get_global_id(0);
    int pairDistance = 1 << (stage - passOfStage);
    int blockWidth = 2 << stage;
    int left = (tid >> passOfStage) * blockWidth + (tid % pairDistance);
    int right = left + pairDistance;
    
    if (right < size) {
        compareSwap(array, left, right, direction);
    }
}
)";

class BitonicSorter
{
public:
	BitonicSorter(std::vector<int>& array)
		: m_array(array)
		, m_size(array.size())
	{
	}

	void Sort()
	{
		if (m_size <= 1)
		{
			return;
		}

		KernelArgs sortArgs;
		std::vector<int> temp(m_array.size());
		sortArgs.argValues = { m_size, 0, 0, 1 };
		sortArgs.inputBuffers = { m_gpuRunner.GetInputBuffer(m_array) };
		sortArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(temp);

		for (int stage = 0; stage < log2(m_size); ++stage)
		{
			for (int passOfStage = 0; passOfStage <= stage; ++passOfStage)
			{
				sortArgs.argValues[1] = stage;
				sortArgs.argValues[2] = passOfStage;
				sortArgs.globalSize = cl::NDRange(m_size / 2);
				m_gpuRunner.Run("bitonic_sort", sortArgs, temp);
			}
		}
	}

private:
	std::vector<int>& m_array;
	int m_size = 0;
	GPURunner m_gpuRunner{ KERNEL_SOURCE };
};