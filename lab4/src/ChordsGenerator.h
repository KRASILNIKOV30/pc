#pragma once
#include "waves/SineWaveGenerator.h"
#include "Chord.h"
#include "waves/PulseWaveGenerator.h"
#include "waves/SawtoothWaveGenerator.h"
#include "waves/TriangleWaveGenerator.h"

#include <memory>
#include <optional>
#include <utility>

constexpr int SECONDS_PER_MINUTE = 60;
constexpr ma_float A440 = 440;
constexpr int FIRST_OCTAVE_INDEX = 4;

class ChordGenerator
{
public:
	// Устранить разрыв по фазе (Исправлено)
	ChordGenerator(ma_uint32 sampleRate, unsigned bpm, std::vector<Chord> const& chords, std::string type, ma_float amplitude = 1.f)
		: m_sampleRate(sampleRate)
		  , m_chords(chords)
		  , m_amplitude(amplitude)
		  , m_bpm(bpm)
		  , m_type(std::move(type))
	{
		InitGenerators();
	}

	ma_float GetNextSample()
	{
		if (m_beatCount == 0)
		{
			if (IsEnd())
			{
				return 0;
			}
			NextChord();
		}
		--m_beatCount;
		ma_float sample = 0;
		for (auto& generator : m_generators)
		{
			sample += generator->GetNextSample();
		}

		return sample;
	}

private:
	[[nodiscard]] bool IsEnd() const
	{
		return m_currentChordIndex == m_chords.size() - 1;
	}

	void NextChord()
	{
		m_beatCount = m_samplesInBeat;
		++m_currentChordIndex;
		InitGenerators();
	}

	void InitGenerators()
	{
		const auto chord = m_chords.at(m_currentChordIndex);
		std::swap(m_prevGenerators, m_generators);
		m_generators.clear();
		m_generators.reserve(chord.size());
		const auto amplitude = m_amplitude / static_cast<ma_float>(chord.size());
		size_t i = 0;
		for (const auto& note : chord)
		{
			ma_float startPhase = 0.f;
			if (i < m_prevGenerators.size())
			{
				startPhase = m_prevGenerators.at(i)->GetPhase();
			}
			const auto amplitudeDelta = note.dim ? -amplitude / static_cast<ma_float>(m_samplesInBeat) : 0;
			m_generators.emplace_back(CreateGenerator(m_sampleRate, GetNoteFrequency(note), amplitude, amplitudeDelta, startPhase));
			++i;
		}
	}

	static ma_float GetNoteFrequency(Note const& note)
	{
		const auto noteType = note.type;
		const auto frequency = A440 * powf(2.0f, static_cast<ma_float>(noteType) / 12.f);
		if (noteType < C)
		{
			return frequency * powf(2, static_cast<ma_float>(note.octave - FIRST_OCTAVE_INDEX));
		}
		return frequency * powf(2, static_cast<ma_float>(note.octave - FIRST_OCTAVE_INDEX - 1));
	}

	std::unique_ptr<WaveGenerator> CreateGenerator(ma_uint32 sampleRate, ma_float frequency, ma_float amplitude, ma_float amplitudeDelta, ma_float startPhase)
	{
		if (m_type == "s")
		{
			return std::make_unique<SineWaveGenerator>(sampleRate, frequency, amplitude, amplitudeDelta, startPhase);
		}
		if (m_type == "p")
		{
			return std::make_unique<PulseWaveGenerator>(sampleRate, frequency, amplitude, amplitudeDelta, startPhase);
		}
		if (m_type == "z")
		{
			return std::make_unique<SawtoothWaveGenerator>(sampleRate, frequency, amplitude, amplitudeDelta, startPhase);
		}
		if (m_type == "t")
		{
			return std::make_unique<TriangleWaveGenerator>(sampleRate, frequency, amplitude, amplitudeDelta, startPhase);
		}

		throw std::invalid_argument("Unknown type '" + m_type + "'");
	}

private:
	ma_uint32 m_sampleRate;
	std::vector<Chord> m_chords;
	size_t m_currentChordIndex = 0;
	ma_float m_amplitude;
	ma_uint32 m_bpm;
	ma_uint32 m_samplesInBeat = m_sampleRate * SECONDS_PER_MINUTE / m_bpm;
	ma_uint32 m_beatCount = m_samplesInBeat;
	std::vector<std::unique_ptr<WaveGenerator>> m_generators{};
	std::vector<std::unique_ptr<WaveGenerator>> m_prevGenerators{};
	std::string m_type;
};