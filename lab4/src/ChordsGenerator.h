#pragma once
#include "SineWaveGenerator.h"
#include "Chord.h"

constexpr int SECONDS_PER_MINUTE = 60;
constexpr ma_float A440 = 440;
constexpr int FIRST_OCTAVE_INDEX = 4;

class ChordGenerator
{
public:
	ChordGenerator(ma_uint32 sampleRate, unsigned bpm, std::vector<Chord> const& chords, ma_float amplitude = 1.f)
		: m_sampleRate(sampleRate)
		  , m_chords(chords)
		  , m_amplitude(amplitude)
		  , m_bpm(bpm)
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
			sample += generator.GetNextSample();
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
		m_generators.clear();
		m_generators.reserve(chord.size());
		const auto amplitude = m_amplitude / static_cast<ma_float>(chord.size());
		for (const auto& note : chord)
		{
			const auto amplitudeDelta = note.dim ? -amplitude / static_cast<ma_float>(m_samplesInBeat) : 0;
			m_generators.emplace_back(m_sampleRate, GetNoteFrequency(note), amplitude, amplitudeDelta);
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

private:
	ma_uint32 m_sampleRate;
	std::vector<Chord> m_chords;
	size_t m_currentChordIndex = 0;
	ma_float m_amplitude;
	ma_uint32 m_bpm;
	ma_uint32 m_samplesInBeat = m_sampleRate * SECONDS_PER_MINUTE / m_bpm;
	ma_uint32 m_beatCount = m_samplesInBeat;
	std::vector<SineWaveGenerator> m_generators;
};