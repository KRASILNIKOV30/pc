#pragma once
#include "Device.h"

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