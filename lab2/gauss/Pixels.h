#pragma once
#include <functional>
#include <vector>
#include <wx/image.h>

struct Pixel
{
	float r;
	float g;
	float b;

	Pixel& operator+=(Pixel const& p)
	{
		r += p.r;
		g += p.g;
		b += p.b;
		return *this;
	}
};

inline Pixel operator*(Pixel const& p, float c)
{
	return {
		p.r * c,
		p.g * c,
		p.b * c,
	};
}

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
				static_cast<unsigned char>(r),
				static_cast<unsigned char>(g),
				static_cast<unsigned char>(b));
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
					static_cast<float>(img.GetRed(x, y)),
					static_cast<float>(img.GetGreen(x, y)),
					static_cast<float>(img.GetBlue(x, y)),
				});
			}
		}
	}

private:
	int m_width;
	int m_height;
	std::vector<Pixel> m_data;
};
