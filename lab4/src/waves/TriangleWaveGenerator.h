#pragma once
#include "WaveGenerator.h"
#include <cmath>
#include <numbers>
#include "Epsilon.h"

class TriangleWaveGenerator final : public WaveGenerator
{
public:
	TriangleWaveGenerator(ma_uint32 sampleRate, ma_float frequency, ma_float amplitude, ma_float amplitudeDelta, ma_float startPhase)
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
		constexpr float pi = std::numbers::pi;
		constexpr auto twoPi = static_cast<ma_float>(2.f * std::numbers::pi);
		m_phase = std::fmod(m_phase + m_phaseShift, twoPi);
		ma_float sample;
		if (m_phase < pi / 2.f)
		{
			sample = m_amplitude * (m_phase / (pi / 2));
		}
		else if (m_phase > 1.5f * pi)
		{
			sample = m_amplitude * ((m_phase - 1.5f * pi) / (pi / 2.f) - 1.f);
		}
		else
		{
			sample = m_amplitude * (1.f - (m_phase - pi / 2) / pi * 2.f);
		}

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