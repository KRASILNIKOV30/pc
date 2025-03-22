#pragma once
#include "WaveGenerator.h"
#include <numbers>

class PulseWaveGenerator final : public WaveGenerator
{
	PulseWaveGenerator(ma_uint32 sampleRate, ma_float frequency, ma_float amplitude, ma_float amplitudeDelta)
		: m_sampleRate{ sampleRate }
		  , m_frequency{ frequency }
		  , m_amplitude{ amplitude }
		  , m_amplitudeDelta(amplitudeDelta)
	{
	}

	ma_float GetNextSample() override
	{
		constexpr auto twoPi = static_cast<ma_float>(2.f * std::numbers::pi);

		const auto sample = m_amplitude * std::sin(m_phase);
		m_phase = std::fmod(m_phase + m_phaseShift, twoPi);
		m_amplitude += m_amplitudeDelta;

		return sample;
	}

private:
	ma_uint32 m_sampleRate;
	ma_float m_frequency;
	ma_float m_amplitude;
	ma_float m_amplitudeDelta;
	ma_float m_phase = 0.f;
	ma_float m_phaseShift = static_cast<ma_float>(2.f * std::numbers::pi * m_frequency / m_sampleRate);
};
