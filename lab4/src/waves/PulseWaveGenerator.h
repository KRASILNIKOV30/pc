#pragma once
#include "Epsilon.h"
#include "WaveGenerator.h"
#include <cmath>
#include <numbers>

class PulseWaveGenerator final : public WaveGenerator
{
public:
	PulseWaveGenerator(ma_uint32 sampleRate, ma_float frequency, ma_float amplitude, ma_float amplitudeDelta, ma_float startPhase)
		: m_sampleRate{ sampleRate }
		  , m_frequency{ frequency }
		  , m_amplitude{ amplitude }
		  , m_amplitudeDelta(amplitudeDelta)
		  , m_phase(startPhase)
	{
		m_phaseShift = static_cast<ma_float>(2.f * std::numbers::pi * m_frequency / m_sampleRate);
	}

	ma_float GetNextSample() override
	{
		constexpr auto pi = static_cast<ma_float>(std::numbers::pi);
		const ma_float sample = (m_phase < pi) ? m_amplitude : -m_amplitude;
		m_phase = std::fmod(m_phase + m_phaseShift, 2.f * pi);
		m_amplitude += m_amplitudeDelta;

		return sample;
	}

	[[nodiscard]] ma_float GetPhase() const override
	{
		return std::abs(m_amplitude) < EPSILON ? 0.f : m_phase;
	}

private:
	ma_uint32 m_sampleRate;
	ma_float m_frequency;
	ma_float m_amplitude;
	ma_float m_amplitudeDelta;
	ma_float m_phase = 0.f;
	ma_float m_phaseShift = 0.f;
};
