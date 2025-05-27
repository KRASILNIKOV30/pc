#pragma once
#include <random>

inline float RandomFloat()
{
	thread_local std::mt19937 generator(std::random_device{}());
	thread_local std::uniform_real_distribution distribution(0.0, 1.0);
	return static_cast<float>(distribution(generator));
}

inline float RandomFloat(float min, float max)
{
	return std::lerp(min, max, RandomFloat());
}