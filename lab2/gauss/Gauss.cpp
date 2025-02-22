#include "Gauss.h"
#include <wx/wx.h>
#include <vector>
#include <cmath>

// Функция для создания ядра Гаусса
std::vector<std::vector<float>> GenerateGaussianKernel(int radius, float sigma)
{
	int size = 2 * radius + 1;
	std::vector kernel(size, std::vector<float>(size));

	float sum = 0.0f;
	const float sigma2 = sigma * sigma;
	const float twoSigma2 = 2.0f * sigma2;
	const float piSigma2 = M_PI * sigma2;

	for (int y = -radius; y <= radius; ++y)
	{
		for (int x = -radius; x <= radius; ++x)
		{
			float value = std::exp(-(x * x + y * y) / twoSigma2) / piSigma2;
			kernel[y + radius][x + radius] = value;
			sum += value;
		}
	}

	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			kernel[y][x] /= sum;
		}
	}

	return kernel;
}

// разделить блюр по горизонтали и вертикали
wxImage ApplyGaussianBlur(wxImage& img, const std::vector<std::vector<float>>& kernel, int radius)
{
	const int width = img.GetWidth();
	const int height = img.GetHeight();

	wxImage result(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;

			for (int ky = -radius; ky <= radius; ++ky)
			{
				for (int kx = -radius; kx <= radius; ++kx)
				{
					const int px = std::min(std::max(x + kx, 0), width - 1);
					const int py = std::min(std::max(y + ky, 0), height - 1);

					const float weight = kernel[ky + radius][kx + radius];

					// r += std::pow(weight * std::pow(static_cast<float>(img.GetRed(px, py)) / 255.0, 2.2), 1.0 / 2.2) * 255;
					// g += std::pow(weight * std::pow(static_cast<float>(img.GetGreen(px, py)) / 255.0, 2.2), 1.0 / 2.2) * 255;
					// b += std::pow(weight * std::pow(static_cast<float>(img.GetBlue(px, py)) / 255.0, 2.2), 1.0 / 2.2) * 255;
					r += weight * img.GetRed(px, py);
					g += weight * img.GetGreen(px, py);
					b += weight * img.GetBlue(px, py);
				}
			}

			result.SetRGB(x, y,
				std::min(std::max(static_cast<int>(r), 0), 255),
				std::min(std::max(static_cast<int>(g), 0), 255),
				std::min(std::max(static_cast<int>(b), 0), 255));
		}
	}

	return result;
}

void GaussBlur(Args const& args)
{
	wxInitializer wxInit;
	wxImage::AddHandler(new wxPNGHandler());

	if (!wxInit)
	{
		throw std::runtime_error("Failed to initialize wxWidgets.");
	}

	wxImage img(args.inputFileName, wxBITMAP_TYPE_PNG);
	if (!img.IsOk())
	{
		throw std::runtime_error("Image loading failed.");
	}

	const int radius = args.radius;
	const auto sigma = radius / 3.29;

	auto kernel = GenerateGaussianKernel(radius, sigma);

	const auto result = ApplyGaussianBlur(img, kernel, radius);

	if (!result.SaveFile(args.outputFileName, wxBITMAP_TYPE_PNG))
	{
		throw std::runtime_error("Failed to save image.");
	}
}
