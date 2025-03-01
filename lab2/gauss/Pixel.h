#pragma once

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