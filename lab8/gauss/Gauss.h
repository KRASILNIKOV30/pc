#pragma once
#include "../gpuRunner/GPURunner.h"
#include "GaussianKernel.h"
#include <cmath>
#include <vector>
#include <wx/wx.h>

inline const char* BLUR_KERNEL_SOURCE = R"(
__kernel void gauss_blur(
    const int width,
    const int height,
    const int radius,
    __constant float* gaussKernel,
    __global float4* pixels,
    __global float4* result
) {
    int i = get_global_id(0);

    int x = i % width;
    int y = i / width;

    float4 sum = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = -radius; i <= radius; ++i) {
        int px = clamp(x + i, 0, width - 1);
        float weight = gaussKernel[i + radius];
		float4 pixel = pixels[px + y * width];
		float4 normalizedPixel = pow(pixel, 2.2f);
		float4 bluredPixel = normalizedPixel * weight;
        sum += pow(bluredPixel, 1.0f / 2.2f);
    }

    result[i] = sum;
}
)";

class GaussBlur : public GPURunner
{
public:
	~GaussBlur()
	{
	}

	GaussBlur(wxImage const& img)
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

		m_kernelArgs.argValues = std::vector<ArgValue>{
			{ sizeof(int), &m_width },
			{ sizeof(int), &m_height },
			{ sizeof(int), &m_radius }
		};
		m_kernelArgs.inputBuffers = {
			GetInputBuffer(m_kernel),
			GetInputBuffer(m_pixels)
		};
		m_kernelArgs.outputBuffer = GetOutputBuffer(m_pixels);
		m_kernelArgs.globalSize = cl::NDRange(m_width * m_height);
	}

	wxImage Blur()
	{
		std::vector<cl_float4> vertBlur(m_pixels.size());
		Run("gauss_blur", vertBlur);

		std::swap(m_kernelArgs.argValues[0].value, m_kernelArgs.argValues[1].value);
		std::vector<cl_float4> transposed(vertBlur.size());
		for (int i = 0; i < vertBlur.size(); ++i)
		{
			const auto x = i % m_width;
			const auto y = i / m_width;
			transposed[x * m_height + y] = vertBlur[y * m_width + x];
		}

		m_kernelArgs.inputBuffers = {
			GetInputBuffer(m_kernel),
			GetInputBuffer(transposed)
		};

		std::vector<cl_float4> horBlur(m_pixels.size());
		Run("gauss_blur", horBlur);

		wxImage output(m_width, m_height);
		for (int i = 0; i < vertBlur.size(); ++i)
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

private:
	[[nodiscard]] const char* GetKernelSource() const override
	{
		return BLUR_KERNEL_SOURCE;
	}

	KernelArgs& GetKernelArgs() override
	{
		return m_kernelArgs;
	}

private:
	KernelArgs m_kernelArgs;
	std::vector<cl_float4> m_pixels;
	int m_width;
	int m_height;
	int m_radius{};
	std::vector<float> m_kernel;
};