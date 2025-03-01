#pragma once
#include "Pixel.h"

#include <functional>
#include <vector>
#include <wx/image.h>

class Pixels
{
public:
	explicit Pixels(wxImage const& img)
		: m_width(img.GetWidth())
		  , m_height(img.GetHeight())
	{
		InitPixels(img);
	}

	Pixels(const int width, const int height)
		: m_width(width)
		  , m_height(height)
	{
		m_data.resize(width * height);
	}

	void Set(const int x, const int y, Pixel const& pixel)
	{
		m_data[y * m_width + x] = pixel;
	}

	[[nodiscard]] Pixel Get(const int x, const int y) const
	{
		return m_data[y * m_width + x];
	}

	wxImage GetImage() const
	{
		wxImage img(m_width, m_height);
		for (int i = 0; i < m_width * m_height; i++)
		{
			const auto [r, g, b] = m_data[i];
			img.SetRGB(i % m_width, i / m_width,
				DenormalizeColor(r),
				DenormalizeColor(g),
				DenormalizeColor(b));
		}

		return img;
	}

	[[nodiscard]] int GetWidth() const
	{
		return m_width;
	}

	[[nodiscard]] int GetHeight() const
	{
		return m_height;
	}

private:
	void InitPixels(wxImage const& img)
	{
		m_data.reserve(m_width * m_height);
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				m_data.emplace_back(Pixel{
					NormalizeColor(img.GetRed(x, y)),
					NormalizeColor(img.GetGreen(x, y)),
					NormalizeColor(img.GetBlue(x, y)),
				});
			}
		}
	}

	static float NormalizeColor(const unsigned char c)
	{
		return std::pow(static_cast<float>(c) / 255, 2.2);
	}

	static unsigned char DenormalizeColor(const float c)
	{
		return static_cast<unsigned char>(std::pow(c, 1.0 / 2.2) * 255);
	}

private:
	int m_width;
	int m_height;
	std::vector<Pixel> m_data;
};
