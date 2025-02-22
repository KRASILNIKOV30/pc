#include "Gauss.h"
#include <wx/wx.h>

void GaussBlur(Args const& args)
{
	// Инициализация wxWidgets
	wxInitializer wxInit;
	if (!wxInit)
	{
		throw std::runtime_error("Failed to initialize wxWidgets.");
	}

	wxImage img(args.inputFileName);
	if (!img.IsOk())
	{
		throw std::runtime_error("Image loading failed.");
	}

	const auto width = img.GetWidth();
	const auto height = img.GetHeight();

	wxImage result(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			result.SetRGB(x, y, img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y));
		}
	}

	if (!result.SaveFile("copied_image.jpg", wxBITMAP_TYPE_JPEG))
	{
		throw std::runtime_error("Failed to save image.");
	}

	return 0;
}