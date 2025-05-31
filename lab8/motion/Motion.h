#pragma once
#include "../gpuRunner/GPURunner.h"
#include <cmath>
#include <vector>
#include <wx/wx.h>

static inline const char* KERNEL_SOURCE = R"(
__kernel void motion_blur(
    const int width,
    const int height,
    const float directionX,
    const float directionY,
    const float strength,
    const int samples,
    __global float4* pixels,
    __global float4* result
) {
    int idx = get_global_id(0);

    if (idx >= width * height) return;

    int x = idx % width;
    int y = idx / width;

    float2 direction = normalize((float2)(directionX, directionY)) * strength;
    float2 currentPos = (float2)(x, y);

    float4 sum = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    int validSamples = 0;

    for (int i = -samples/2; i <= samples/2; ++i) {
        float2 samplePos = currentPos + direction * (float)i * 0.1f;

        int sampleX = clamp((int)samplePos.x, 0, width - 1);
        int sampleY = clamp((int)samplePos.y, 0, height - 1);

        int sampleIdx = sampleY * width + sampleX;
        sum += pixels[sampleIdx];
        validSamples++;
    }

    result[idx] = sum / (float)validSamples;
}
)";

class MotionBlur
{
public:
	void SetImage(wxImage const& img)
	{
		m_width = img.GetWidth();
		m_height = img.GetHeight();

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

	void SetDirection(float directionX, float directionY)
	{
		m_directionX = directionX;
		m_directionY = directionY;
	}

	void SetStrength(float strength)
	{
		m_strength = strength;
	}

	void SetSamples(int samples)
	{
		m_samples = samples;
	}

	wxImage Blur()
	{
		KernelArgs kernelArgs;
		kernelArgs.argValues = { m_width, m_height, m_directionX, m_directionY, m_strength, m_samples };
		kernelArgs.inputBuffers = { m_gpuRunner.GetInputBuffer(m_pixels) };

		std::vector<cl_float4> result(m_pixels.size());
		kernelArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(result);
		kernelArgs.globalSize = cl::NDRange(m_width * m_height);

		m_gpuRunner.Run("motion_blur", kernelArgs, result);

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
	float m_directionX = 1.0f;
	float m_directionY = 0.0f;
	float m_strength = 0.1f;
	int m_samples = 30;
	GPURunner m_gpuRunner{ KERNEL_SOURCE };
};