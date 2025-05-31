#pragma once
#include "../gpuRunner/GPURunner.h"
#include <algorithm>
#include <vector>
#include <wx/wx.h>

static inline const char* KERNEL_SOURCE = R"(
__kernel void median_filter(
    const int width,
    const int height,
    const int radius,
    __global float4* pixels,
    __global float4* result
) {
    int idx = get_global_id(0);

    if (idx >= width * height) return;

    int x = idx % width;
    int y = idx / width;

    int samples = 0;
    float4 window[400]; // Максимальный размер окна (20x20)

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int px = clamp(x + dx, 0, width - 1);
            int py = clamp(y + dy, 0, height - 1);
            window[samples++] = pixels[py * width + px];
        }
    }

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < samples - 1; ++i) {
            for (int j = 0; j < samples - i - 1; ++j) {
                if (window[j][c] > window[j+1][c]) {
                    float4 temp = window[j];
                    window[j] = window[j+1];
                    window[j+1] = temp;
                }
            }
        }
    }

    result[idx] = window[samples/2];
}
)";

class MedianFilter
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
	}

	wxImage Apply()
	{
		KernelArgs kernelArgs;
		kernelArgs.argValues = { m_width, m_height, m_radius };
		kernelArgs.inputBuffers = { m_gpuRunner.GetInputBuffer(m_pixels) };

		std::vector<cl_float4> result(m_pixels.size());
		kernelArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(result);
		kernelArgs.globalSize = cl::NDRange(m_width * m_height);

		m_gpuRunner.Run("median_filter", kernelArgs, result);

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
	int m_radius = 1;
	GPURunner m_gpuRunner{ KERNEL_SOURCE };
};