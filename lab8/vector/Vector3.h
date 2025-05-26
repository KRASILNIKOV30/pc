#pragma once
#include "Vector2.h"
#include <cmath>

template <class T> class Vector3
{
public:
	T x, y, z;

	Vector3() noexcept
		: x(0), y(0), z(0)
	{
	}

	explicit Vector3(const T* p) noexcept
		: x(p[0]), y(p[1]), z(p[2])
	{
	}

	Vector3(T const& x0, T const& y0, T const& z0) noexcept
		: x(x0), y(y0), z(z0)
	{
	}

	explicit Vector3(Vector2<T> const& v, T const& z0 = 0) noexcept
		: x(v.x), y(v.y), z(z0)
	{
	}

	explicit operator T const*() const noexcept
	{
		return &x;
	}

	Vector3& operator *=(T const& scale) noexcept
	{
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	Vector3& operator /=(T const& scalar) noexcept
	{
		return *this *= (1 / scalar);
	}

	Vector3 operator /(T const& scalar) const noexcept
	{
		return *this * (1 / scalar);
	}

	Vector3 operator-(Vector3 const& v) const noexcept
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator-() const noexcept
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 operator+() const noexcept
	{
		return *this;
	}

	Vector3 operator*(Vector3 const& v) const noexcept
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 operator/(Vector3 const& v) const noexcept
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	Vector3 operator+(Vector3 const& v) const noexcept
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3& operator+=(Vector3 const& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator*=(Vector3 const& v) noexcept
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3& operator/=(Vector3 const& v) noexcept
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3& operator-=(Vector3 const& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3 operator*(T const& scale) const noexcept
	{
		return Vector3(x * scale, y * scale, z * scale);
	}

	T GetLength() const noexcept
	{
		return static_cast<T>(std::sqrt(x * x + y * y + z * z));
	}

	T GetLengthSquared() const noexcept
	{
		return x * x + y * y + z * z;
	}

	void Normalize() noexcept
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
};

template <class T> Vector3<T> operator*(T const& scalar, Vector3<T> const& v) noexcept
{
	return v * scalar;
}

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;