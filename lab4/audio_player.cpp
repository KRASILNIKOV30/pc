#include "miniaudio.h"
#include <cmath>
#include <functional>
#include <iostream>
#include <numbers>
#include <span>
#include <string>

namespace audio
{

constexpr int SECONDS_PER_MINUTE = 60;
constexpr ma_float A440 = 440;
constexpr int FIRST_OCTAVE_INDEX = 4;

inline const std::error_category& ErrorCategory()
{
	class AudioErrorCategory : public std::error_category
	{
	public:
		[[nodiscard]] const char* name() const noexcept override
		{
			return "Audio error category";
		}

		[[nodiscard]] std::string message(int errorCode) const override
		{
			return ma_result_description(static_cast<ma_result>(errorCode));
		}
	};

	static AudioErrorCategory errorCategory;
	return errorCategory;
}

class Device
{
public:
	using DataCallback = std::function<void(void*, const void*, ma_uint32)>;

	explicit Device(ma_device_config config)
	{
		config.pUserData = this;
		config.dataCallback = [](ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
			static_cast<Device*>(device->pUserData)->OnDataCallback(output, input, frameCount);
		};

		if (ma_device_init(nullptr, &config, &m_device) != MA_SUCCESS)
		{
			throw std::runtime_error("Device initialization failed");
		}
	}

	void Start()
	{
		if (auto result = ma_device_start(&m_device); result != MA_SUCCESS)
		{
			throw std::system_error(result, ErrorCategory());
		}
	}

	void Stop()
	{
		if (auto result = ma_device_stop(&m_device); result != MA_SUCCESS)
		{
			throw std::system_error(result, ErrorCategory());
		}
	}

	ma_device* operator->() noexcept
	{
		return &m_device;
	}

	const ma_device* operator->() const noexcept
	{
		return &m_device;
	}

	void SetDataCallback(DataCallback dataCallback)
	{
		m_dataCallback = std::move(dataCallback);
	}

	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;

	~Device()
	{
		ma_device_uninit(&m_device);
	}

private:
	void OnDataCallback(void* output, const void* input, ma_uint32 frameCount) const
	{
		if (m_dataCallback)
		{
			m_dataCallback(output, input, frameCount);
		}
	}

	ma_device m_device{};
	DataCallback m_dataCallback;
};

class Player
{
public:
	using DataCallback = std::function<void(void* output, ma_uint32 frameCount)>;

	Player(ma_format format, ma_uint32 channels, ma_uint32 sampleRate = 48000)
		: m_device(CreateConfig(format, channels, sampleRate))
	{
		m_device.SetDataCallback([this](void* output, const void*, ma_uint32 frameCount) {
			if (m_dataCallback)
			{
				m_dataCallback(output, frameCount);
			}
		});
	}

	void Start()
	{
		m_device.Start();
	}

	void Stop()
	{
		m_device.Stop();
	}

	[[nodiscard]] ma_format GetFormat() const noexcept
	{
		return m_device->playback.format;
	}

	[[nodiscard]] ma_uint32 GetChannels() const noexcept
	{
		return m_device->playback.channels;
	}

	[[nodiscard]] ma_uint32 GetSampleRate() const noexcept
	{
		return m_device->sampleRate;
	}

	void SetDataCallback(DataCallback callback)
	{
		m_dataCallback = std::move(callback);
	}

private:
	static ma_device_config CreateConfig(ma_format format, ma_uint32 channels, ma_uint32 sampleRate)
	{
		auto config = ma_device_config_init(ma_device_type_playback);
		auto& playback = config.playback;

		playback.format = format;
		playback.channels = channels;
		config.sampleRate = sampleRate;

		return config;
	}

	DataCallback m_dataCallback;
	Device m_device;
};

class SineWaveGenerator
{
public:
	SineWaveGenerator(ma_uint32 sampleRate, ma_float frequency, ma_float amplitude, ma_float amplitudeDelta)
		: m_sampleRate{ sampleRate }
		  , m_frequency{ frequency }
		  , m_amplitude{ amplitude }
		  , m_amplitudeDelta(amplitudeDelta)
	{
	}

	ma_float GetNextSample()
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
	// На сколько смещается фаза синусоидальных колебаний при переходе к следующему отсчёту
	ma_float m_phaseShift = static_cast<ma_float>(2.f * std::numbers::pi * m_frequency / m_sampleRate);
};

enum NoteType
{
	A, Ad, B, C, Cd, D, Dd, E, F, Fd, G, Gd
};

struct Note
{
	NoteType type;
	int octave;
	bool dim = false;
};

using Chord = std::vector<Note>;

class ChordGenerator
{
public:
	ChordGenerator(ma_uint32 sampleRate, int bpm, std::vector<Chord> const& chords, ma_float amplitude = 1.f)
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

} // namespace audio

int main()
{
	audio::Player player(ma_format_f32, 1);

	audio::ChordGenerator chordGenerator(player.GetSampleRate(), 60, {
		{ { audio::A, 4, true }, { audio::C, 5, true }, { audio::E, 5, true } },
		{ { audio::C, 5, true } },
		{ { audio::E, 5, true } },
	}, 0.3f);

	player.SetDataCallback([&chordGenerator](void* output, ma_uint32 frameCount) mutable {
		auto samples = std::span(static_cast<ma_float*>(output), frameCount);
		for (auto& sample : samples)
		{
			// Складываем сэмплы со всех генераторов аудио-сигнала
			sample = chordGenerator.GetNextSample();
		}
	});

	player.Start();

	std::string s;
	std::getline(std::cin, s);
}