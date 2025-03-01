#include "Gauss.h"

#include "GaussianKernel.h"
#include "Pixels.h"
#include <cmath>
#include <wx/wx.h>

// разделить блюр по горизонтали и вертикали (исправлено)
void ApplyGaussianBlur(const Pixels& pixels, const std::vector<float>& kernel, const int radius, const int start, const int end, Pixels& result)
{
	const auto width = pixels.GetWidth();
	const auto height = pixels.GetHeight();
	Pixels horizontalBlur(width, height);
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
		horizontalBlur.Set(x, y, newPixel);
	}

	for (int i = start; i < end; i++)
	{
		const int x = i % width;
		const int y = i / width;
		Pixel newPixel{};
		for (int ky = -radius; ky <= radius; ++ky)
		{
			const int py = std::min(std::max(y + ky, 0), height - 1);
			const float weight = kernel[ky + radius];
			newPixel += horizontalBlur.Get(x, py) * weight;
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
	Pixels result(width, height);
	ApplyGaussianBlur(pixels, kernel, radius, 0, width * height, result);

	return result.GetImage();
}

void GaussBlur(Args const& args)
{
	wxInitializer wxInit;
	wxImage::AddHandler(new wxPNGHandler());

	if (!wxInit)
	{
		throw std::runtime_error("Failed to initialize wxWidgets.");
	}

	const wxImage img(args.inputFileName, wxBITMAP_TYPE_PNG);
	if (!img.IsOk())
	{
		throw std::runtime_error("Image loading failed.");
	}

	const auto result = BlurParallel(img, args.radius, args.threadsNum);

	if (!result.SaveFile(args.outputFileName, wxBITMAP_TYPE_PNG))
	{
		throw std::runtime_error("Failed to save image.");
	}
}
