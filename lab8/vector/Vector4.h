#pragma once
#include "Vector3.h"

template <class T> class Vector4
{
public:
	T x;
	T y;
	T z;
	T w;

	Vector4() noexcept
		: x(0), y(0), z(0), w(0)
	{
	}

	explicit Vector4(const T* p) noexcept
		: x(p[0]), y(p[1]), z(p[2]), w(p[3])
	{
	}

	Vector4(T const& x0, T const& y0, T const& z0, T const& w0 = 0) noexcept
		: x(x0), y(y0), z(z0), w(w0)
	{
	}

	explicit Vector4(Vector3<T> const& v, T const& w0 = 0) noexcept
		: x(v.x), y(v.y), z(v.z), w(w0)
	{
	}

	explicit operator T const*() const noexcept
	{
		return &x;
	}

	Vector4& operator *=(T scale) noexcept
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}

	Vector4& operator /=(T const& scalar) noexcept
	{
		return *this *= 1 / scalar;
	}

	Vector4 operator /(T const& scalar) const noexcept
	{
		return *this * (1 / scalar);
	}

	Vector4 operator-(Vector4 const& v) const noexcept
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4 operator-() const noexcept
	{
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 operator+() const noexcept
	{
		return *this;
	}

	Vector4 operator*(Vector4 const& v) const noexcept
	{
		return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	Vector4 operator/(Vector4 const& v) const noexcept
	{
		return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	Vector4 operator+(Vector4 const& v) const noexcept
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4& operator+=(Vector4 const& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	Vector4& operator*=(Vector4 const& v) noexcept
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	Vector4& operator/=(Vector4 const& v) noexcept
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	Vector4& operator-=(Vector4 const& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	Vector4 operator*(T scale) const noexcept
	{
		return Vector4(x * scale, y * scale, z * scale, w * scale);
	}

	T GetLength() const noexcept
	{
		return static_cast<T>(sqrt(x * x + y * y + z * z + w * w));
	}

	explicit operator Vector3<T>() const noexcept
	{
		return Vector3<T>(x, y, z);
	}

	Vector3<T> Project() const noexcept
	{
		T invW = 1 / w;
		return Vector3<T>(x * invW, y * invW, z * invW);
	}

	void Normalize() noexcept
	{
		T invLength = 1 / GetLength();
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}
};


template <class T> Vector4<T> operator*(T const& scalar, Vector4<T> const& v) noexcept
{
	return v * scalar;
}

typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;