#pragma once
#include "../../lib/miniaudio.h"

class WaveGenerator
{
public:
	virtual ma_float GetNextSample() = 0;
	virtual ~WaveGenerator() = default;
};
