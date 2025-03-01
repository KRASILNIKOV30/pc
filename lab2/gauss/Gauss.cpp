#include "Gauss.h"
#include <wx/wx.h>
#include <vector>
#include <cmath>

// Функция для создания ядра Гаусса
std::vector<float> GenerateGaussianKernel(int radius, float sigma) {
	int size = 2 * radius + 1;
	std::vector<float> kernel(size);

	float sum = 0.0f;
	const float sigma2 = sigma * sigma;
	const float twoSigma2 = 2.0f * sigma2;
	const float piSigma2 = M_PI * sigma2;

	for (int x = -radius; x <= radius; ++x) {
		const float value = std::exp(-(x * x) / twoSigma2) / std::sqrt(2 * piSigma2);
		kernel[x + radius] = value;
		sum += value;
	}

	for (int x = 0; x < size; ++x) {
		kernel[x] /= sum;
	}

	return kernel;
}

// разделить блюр по горизонтали и вертикали
wxImage ApplyGaussianBlur(wxImage &img, const std::vector<float> &kernel, int radius) {
	const int width = img.GetWidth();
	const int height = img.GetHeight();

	wxImage result(width, height);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;

			for (int kx = -radius; kx <= radius; ++kx) {
				const int px = std::min(std::max(x + kx, 0), width - 1);

				const float weight = kernel[kx + radius];

				r += weight * img.GetRed(px, y);
				g += weight * img.GetGreen(px, y);
				b += weight * img.GetBlue(px, y);
			}

			result.SetRGB(x, y,
			              std::min(std::max(static_cast<int>(r), 0), 255),
			              std::min(std::max(static_cast<int>(g), 0), 255),
			              std::min(std::max(static_cast<int>(b), 0), 255));
		}
	}

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;
			for (int ky = -radius; ky <= radius; ++ky) {
				const int py = std::min(std::max(y + ky, 0), height - 1);
				const float weight = kernel[ky + radius];
				r += weight * result.GetRed(x, py);
				g += weight * result.GetGreen(x, py);
				b += weight * result.GetBlue(x, py);
			}

			result.SetRGB(x, y,
			              std::min(std::max(static_cast<int>(r), 0), 255),
			              std::min(std::max(static_cast<int>(g), 0), 255),
			              std::min(std::max(static_cast<int>(b), 0), 255));
		}
	}

	return result;
}

void GaussBlur(Args const &args) {
	wxInitializer wxInit;
	wxImage::AddHandler(new wxPNGHandler());

	if (!wxInit) {
		throw std::runtime_error("Failed to initialize wxWidgets.");
	}

	wxImage img(args.inputFileName, wxBITMAP_TYPE_PNG);
	if (!img.IsOk()) {
		throw std::runtime_error("Image loading failed.");
	}

	const int radius = args.radius;
	const auto sigma = radius / 3.29;

	auto kernel = GenerateGaussianKernel(radius, sigma);

	const auto result = ApplyGaussianBlur(img, kernel, radius);

	if (!result.SaveFile(args.outputFileName, wxBITMAP_TYPE_PNG)) {
		throw std::runtime_error("Failed to save image.");
	}
}
