#pragma once
#include "../gpuRunner/GPURunner.h"
#include <cmath>
#include <vector>
#include <wx/wx.h>

static inline const char* KERNEL_SOURCE = R"(
__kernel void radial_blur(
    const int width,
    const int height,
    const float centerX,
    const float centerY,
    const float strength,
    const int samples,
    __global float4* pixels,
    __global float4* result
) {
    int idx = get_global_id(0);

    if (idx >= width * height) return;

    int x = idx % width;
    int y = idx / width;

    float2 center = (float2)(centerX, centerY);
    float2 currentPos = (float2)(x, y);
    float2 direction = currentPos - center;
    float distance = length(direction);

    if (distance < 0.5f) {
        result[idx] = pixels[idx];
        return;
    }

    direction = normalize(direction) * strength;

    float4 sum = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < samples; ++i) {
        float t = (float)i / (float)(samples - 1);
        float2 samplePos = currentPos + direction * t;

        int sampleX = clamp((int)samplePos.x, 0, width - 1);
        int sampleY = clamp((int)samplePos.y, 0, height - 1);

        int sampleIdx = sampleY * width + sampleX;
        sum += pixels[sampleIdx];
    }

    result[idx] = sum / (float)samples;
}
)";

class RadialBlur
{
public:
	void SetImage(wxImage const& img)
	{
		m_width = img.GetWidth();
		m_height = img.GetHeight();

		m_centerX = m_width / 2;
		m_centerY = m_height / 2;

		m_pixels.resize(m_width * m_height);
		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				m_pixels[y * m_width + x] = {
					img.GetRed(x, y) / 255.0f,
					img.GetGreen(x, y) / 255.0f,
					img.GetBlue(x, y) / 255.0f,
					1.0f
				};
			}
		}
	}

	void SetStrength(float strength)
	{
		m_strength = strength;
	}

	wxImage Blur()
	{
		KernelArgs kernelArgs;
		kernelArgs.argValues = { m_width, m_height, m_centerX, m_centerY, m_strength, m_samples };
		kernelArgs.inputBuffers = { m_gpuRunner.GetInputBuffer(m_pixels) };

		std::vector<cl_float4> result(m_pixels.size());
		kernelArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(result);
		kernelArgs.globalSize = cl::NDRange(m_width * m_height);

		m_gpuRunner.Run("radial_blur", kernelArgs, result);

		wxImage output(m_width, m_height);
		for (int i = 0; i < result.size(); ++i)
		{
			const auto x = i % m_width;
			const auto y = i / m_width;
			auto& pixel = result[x + y * m_width];
			output.SetRGB(x, y,
				static_cast<unsigned char>(std::clamp(pixel.s[0], 0.0f, 1.0f) * 255),
				static_cast<unsigned char>(std::clamp(pixel.s[1], 0.0f, 1.0f) * 255),
				static_cast<unsigned char>(std::clamp(pixel.s[2], 0.0f, 1.0f) * 255));
		}

		return output;
	}

private:
	std::vector<cl_float4> m_pixels;
	int m_width;
	int m_height;
	float m_centerX;
	float m_centerY;
	float m_strength = 0.1f;
	int m_samples = 10;
	GPURunner m_gpuRunner{ KERNEL_SOURCE };
};