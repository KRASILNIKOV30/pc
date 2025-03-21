#pragma once
#include "ErrorCategory.h"
#include <functional>

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