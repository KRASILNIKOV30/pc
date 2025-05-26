#pragma once

template <class T> class Vector2
{
public:
	Vector2() noexcept
		: x(0), y(0)
	{
	}

	explicit Vector2(const T* p) noexcept
		: x(p[0]), y(p[1])
	{
	}

	Vector2(T const& x0, T const& y0) noexcept
		: x(x0), y(y0)
	{
	}

	explicit operator T const*() const noexcept
	{
		return &x;
	}

	Vector2& operator *=(T const& scale) noexcept
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	Vector2& operator /=(T const& scalar) noexcept
	{
		return *this *= (1 / scalar);
	}

	Vector2 operator /(T const& scalar) const noexcept
	{
		return *this * (1 / scalar);
	}

	Vector2 operator-(Vector2 const& v) const noexcept
	{
		return CVector2(x - v.x, y - v.y);
	}

	Vector2 operator-() const noexcept
	{
		return CVector2(-x, -y);
	}

	Vector2 operator+() const noexcept
	{
		return *this;
	}

	Vector2 operator*(Vector2 const& v) const noexcept
	{
		return Vector2(x * v.x, y * v.y);
	}

	Vector2 operator/(Vector2 const& v) const noexcept
	{
		return Vector2(x / v.x, y / v.y);
	}

	Vector2 operator+(Vector2 const& v) const noexcept
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2& operator+=(Vector2 const& v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2& operator*=(Vector2 const& v) noexcept
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vector2& operator/=(Vector2 const& v) noexcept
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector2& operator-=(Vector2 const& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2 operator*(T const& scale) const noexcept
	{
		return Vector2(x * scale, y * scale);
	}

	T GetLength() const noexcept
	{
		return static_cast<T>(sqrt(x * x + y * y));
	}

	void Normalize() noexcept
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
	}

	T x;
	T y;
};

template <class T> Vector2<T> operator*(T const& scalar, Vector2<T> const& v) noexcept
{
	return v * scalar;
}

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;