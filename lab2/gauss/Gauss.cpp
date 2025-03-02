#include "Gauss.h"
#include "GaussianKernel.h"
#include "Parallel.h"
#include "Pixels.h"
#include "../../lib/timer/Timer.h"
#include <cmath>
#include <wx/wx.h>

void ApplyHorizontalBlur(const Pixels& pixels, const std::vector<float>& kernel, const int radius, const int start, const int end, Pixels& result)
{
	const auto width = pixels.GetWidth();
	for (int i = start; i < end; i++)
	{
		const int x = i % width;
		const int y = i / width;
		Pixel newPixel{};
		for (int kx = -radius; kx <= radius; ++kx)
		{
			const int px = std::min(std::max(x + kx, 0), width - 1);
			const float weight = kernel[kx + radius];
			newPixel += pixels.Get(px, y) * weight;
		}
		result.Set(x, y, newPixel);
	}
}

wxImage BlurParallel(wxImage const& img, const int radius, const int threadsNum)
{
	const auto sigma = radius / 3.29;
	const auto kernel = GenerateGaussianKernel(radius, sigma);

	const Pixels pixels(img);
	const int width = pixels.GetWidth();
	const int height = pixels.GetHeight();

	Pixels horizontalBlur(width, height);
	ComputeParallel(width * height, threadsNum, [&](const size_t start, const size_t end) {
		ApplyHorizontalBlur(pixels, kernel, radius, start, end, horizontalBlur);
	});
	Pixels result(width, height);
	horizontalBlur.Transpose();
	result.Transpose();
	ComputeParallel(width * height, threadsNum, [&](const size_t start, const size_t end) {
		ApplyHorizontalBlur(horizontalBlur, kernel, radius, start, end, result);
	});
	result.Transpose();

	return result.GetImage();
}

void GaussBlur(Args const& args)
{
	wxInitializer wxInit;
	wxImage::AddHandler(new wxJPEGHandler());

	if (!wxInit)
	{
		throw std::runtime_error("Failed to initialize wxWidgets.");
	}

	const wxImage img(args.inputFileName, wxBITMAP_TYPE_JPEG);
	if (!img.IsOk())
	{
		throw std::runtime_error("Image loading failed.");
	}

	wxImage result;
	for (int i = 1; i <= 20; ++i)
	{
		MeasureTime(std::cout, "Gaussian blur with " + std::to_string(i) + " threads", [&] {
			result = BlurParallel(img, args.radius, i);
		});
	}

	if (!result.SaveFile(args.outputFileName, wxBITMAP_TYPE_JPEG))
	{
		throw std::runtime_error("Failed to save image.");
	}
}
