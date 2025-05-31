#pragma once
#include "../gpuRunner/GPURunner.h"
#include "GaussianKernel.h"
#include <cmath>
#include <vector>
#include <wx/wx.h>

static inline const char* KERNEL_SOURCE = R"(
__kernel void gauss_blur(
    const int width,
    const int height,
    const int radius,
    __global float* gaussKernel,
    __global float4* pixels,
    __global float4* result
) {
    int idx = get_global_id(0);

	if (idx >= width * height) return;

    int x = idx % width;
    int y = idx / width;

    float4 sum = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	float weightSum = 0.0;

    for (int i = -radius; i <= radius; ++i) {
        int px = clamp(x + i, 0, width - 1);
        float weight = gaussKernel[i + radius];

	    int pidx = px + y * width;
		float4 pixel = pixels[pidx];
		float4 normalizedPixel = pixel;
		weightSum += weight;
		float4 bluredPixel = normalizedPixel * weight;
		sum += bluredPixel;
   }

   result[idx] = sum / weightSum;
}
)";

class GaussBlur
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

	void SetRadius(int radius)
	{
		m_radius = radius;
		const float sigma = static_cast<float>(radius) / 3.29f;
		m_kernel = GenerateGaussianKernel(m_radius, sigma);
	}

	wxImage Blur()
	{
		m_kernelArgs = KernelArgs();
		m_kernelArgs.argValues = { m_width, m_height, m_radius };
		m_kernelArgs.inputBuffers = {
			m_gpuRunner.GetInputBuffer(m_kernel),
			m_gpuRunner.GetInputBuffer(m_pixels)
		};
		std::vector<cl_float4> horBlur(m_pixels.size());
		m_kernelArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(horBlur);
		m_kernelArgs.globalSize = cl::NDRange(m_width * m_height);

		m_gpuRunner.Run("gauss_blur", m_kernelArgs, horBlur);

		std::swap(m_kernelArgs.argValues[0], m_kernelArgs.argValues[1]);
		auto transposed = Transpose(horBlur);

		m_kernelArgs.inputBuffers = {
			m_gpuRunner.GetInputBuffer(m_kernel),
			m_gpuRunner.GetInputBuffer(transposed)
		};

		std::vector<cl_float4> vertBlur(m_pixels.size());
		m_kernelArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(vertBlur);
		m_gpuRunner.Run("gauss_blur", m_kernelArgs, vertBlur);
		const auto result = Transpose(horBlur);

		wxImage output(m_width, m_height);
		for (int i = 0; i < horBlur.size(); ++i)
		{
			const auto x = i % m_width;
			const auto y = i / m_width;
			auto& pixel = horBlur[x + y * m_width];
			output.SetRGB(x, y,
				static_cast<unsigned char>(pixel.s[0] * 255),
				static_cast<unsigned char>(pixel.s[1] * 255),
				static_cast<unsigned char>(pixel.s[2] * 255));
		}

		return output;
	}

	std::vector<cl_float4> Transpose(std::vector<cl_float4> const& v)
	{
		std::vector<cl_float4> transposed(v.size());
		for (int i = 0; i < v.size(); ++i)
		{
			const auto x = i % m_width;
			const auto y = i / m_width;
			transposed[x * m_height + y] = v[y * m_width + x];
		}

		return transposed;
	}

private:
	KernelArgs m_kernelArgs;
	std::vector<cl_float4> m_pixels;
	int m_width;
	int m_height;
	int m_radius{};
	std::vector<float> m_kernel;
	GPURunner m_gpuRunner{ KERNEL_SOURCE };
};