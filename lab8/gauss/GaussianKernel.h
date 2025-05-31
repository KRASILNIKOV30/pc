#pragma once
#include <math.h>
#include <vector>

inline std::vector<float> GenerateGaussianKernel(const int radius, const float sigma)
{
	const int size = 2 * radius + 1;
	std::vector<float> kernel(size);

	float sum = 0.0f;
	const float sigma2 = sigma * sigma;
	const float twoSigma2 = 2.0f * sigma2;
	const float piSigma2 = M_PI * sigma2;

	for (int x = -radius; x <= radius; ++x)
	{
		const float value = std::exp(-(x * x) / twoSigma2) / std::sqrt(2 * piSigma2);
		kernel[x + radius] = value;
		sum += value;
	}

	for (int x = 0; x < size; ++x)
	{
		kernel[x] /= sum;
	}

	return kernel;
}