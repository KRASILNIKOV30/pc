#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "../matrix/Matrix4.h"

/************************************************************************/
/* Набор вспомогательных функций по работе с векторами и матрицами      */
/************************************************************************/

template <class T> T Sqr(T const& x) noexcept
{
	return x * x;
}

/************************************************************************/
/* Fract - взятие дробной части скаляра или координат вектора           */
/************************************************************************/
template <class T> T Fract(T const& value) noexcept
{
	return value - (T)floor(value);
}

template <class T> Vector2<T> Fract(Vector2<T> const& arg) noexcept
{
	return Vector2<T>(Fract(arg.x), Fract(arg.y));
}

template <class T> Vector3<T> Fract(Vector3<T> const& arg) noexcept
{
	return Vector3<T>(Fract(arg.x), Fract(arg.y), Fract(arg.z));
}

template <class T> Vector4<T> Fract(Vector4<T> const& arg) noexcept
{
	return Vector4<T>(Fract(arg.x), Fract(arg.y), Fract(arg.z), Fract(arg.w));
}

/***************************************************************************/
/* Step - возвращает 0, если значение параметра меньше некоторого значения */
/* и 1 в противном случае                                                  */
/****************************************************************************/
template <class T> T Step(T const& edge, T const& x) noexcept
{
	return x < edge ? 0 : 1;
}

template <class T> Vector2<T> Step(T const& edge, Vector2<T> const& vec) noexcept
{
	return Vector2<T>(Step(edge, vec.x), Step(edge, vec.y));
}

template <class T> Vector2<T> Step(Vector2<T> const& edge, Vector2<T> const& vec) noexcept
{
	return Vector2<T>(Step(edge.x, vec.x), Step(edge.y, vec.y));
}

template <class T> Vector3<T> Step(T const& edge, Vector3<T> const& vec) noexcept
{
	return Vector3<T>(Step(edge, vec.x), Step(edge, vec.y), Step(edge, vec.z));
}

template <class T> Vector3<T> Step(Vector3<T> const& edge, Vector3<T> const& vec) noexcept
{
	return Vector3<T>(Step(edge.x, vec.x), Step(edge.y, vec.y), Step(edge.z, vec.z));
}

template <class T> Vector4<T> Step(T const& edge, Vector4<T> const& vec) noexcept
{
	return Vector4<T>(Step(edge, vec.x), Step(edge, vec.y), Step(edge, vec.z), Step(edge, vec.w));
}

template <class T> Vector4<T> Step(Vector4<T> const& edge, Vector4<T> const& vec) noexcept
{
	return Vector4<T>(Step(edge.x, vec.x), Step(edge.y, vec.y), Step(edge.z, vec.z), Step(edge.w, vec.w));
}

/************************************************************************/
/* Скалярное произведение векторов                                      */
/************************************************************************/
template <class T> T Dot(Vector2<T> const& a, Vector2<T> const& b) noexcept
{
	return (a.x * b.x + a.y * b.y);
}

template <class T> inline T Dot(Vector3<T> const& a, Vector3<T> const& b) noexcept
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}


template <class T> inline T Dot(Vector4<T> const& a, Vector4<T> const& b) noexcept
{
	return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

/************************************************************************/
/* Min - поиск минимума                                                 */
/************************************************************************/
template <class T> T Min(T const& x, T const& y) noexcept
{
	return x < y ? x : y;
}

template <class T> inline Vector2<T> Min(Vector2<T> const& vec, T const& scalar) noexcept
{
	return Vector2<T>(Min(vec.x, scalar), Min(vec.y, scalar));
};

template <class T> inline Vector3<T> Min(Vector3<T> const& vec, T const& scalar) noexcept
{
	return Vector3<T>(Min(vec.x, scalar), Min(vec.y, scalar), Min(vec.z, scalar));
};


template <class T> inline Vector4<T> Min(Vector4<T> const& vec, T const& scalar) noexcept
{
	return Vector4<T>(Min(vec.x, scalar), Min(vec.y, scalar), Min(vec.z, scalar), Min(vec.w, scalar));
};

template <class T> Vector3<T> Min(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}


/************************************************************************/
/* Max - поиск максимума                                                */
/************************************************************************/
template <class T> T Max(T const& x, T const& y) noexcept
{
	return x > y ? x : y;
}

template <class T> inline Vector2<T> Max(Vector2<T> const& vec, T const& scalar) noexcept
{
	return Vector2<T>(Max(vec.x, scalar), Max(vec.y, scalar));
};

template <class T> inline Vector3<T> Max(Vector3<T> const& vec, T const& scalar) noexcept
{
	return Vector3<T>(Max(vec.x, scalar), Max(vec.y, scalar), Max(vec.z, scalar));
};


template <class T> inline Vector4<T> Max(Vector4<T> const& vec, T const& scalar) noexcept
{
	return Vector4<T>(Max(vec.x, scalar), Max(vec.y, scalar), Max(vec.z, scalar), Max(vec.w, scalar));
};

template <class T> inline Vector3<T> Max(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}


/************************************************************************/
/* Clamp - приведение скаляра или вектора к заданному диапазону         */
/************************************************************************/
template <class T> inline Vector2<T> Clamp(Vector2<T> const& vec, T const& minValue, T const& maxValue) noexcept
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T> inline Vector2<T> Clamp(Vector2<T> const& vec, Vector2<T> const& minValue, Vector2<T> const& maxValue) noexcept
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T> inline Vector3<T> Clamp(Vector3<T> const& vec, T const& minValue, T const& maxValue) noexcept
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T> inline Vector3<T> Clamp(Vector3<T> const& vec, Vector3<T> const& minValue, Vector3<T> const& maxValue) noexcept
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T> inline Vector4<T> Clamp(Vector4<T> const& vec, T const& minValue, T const& maxValue) noexcept
{
	return Max(Min(vec, maxValue), minValue);
}

template <class T> inline Vector4<T> Clamp(Vector4<T> const& vec, Vector4<T> const& minValue, Vector4<T> const& maxValue) noexcept
{
	return Max(Min(vec, maxValue), minValue);
}

/************************************************************************/
/* Нормализация векторов                                                */
/************************************************************************/
template <class T> inline Vector2<T> Normalize(Vector2<T> const& vec) noexcept
{
	Vector2<T> result(vec);
	result.Normalize();
	return result;
}

template <class T> inline Vector3<T> Normalize(Vector3<T> const& vec) noexcept
{
	Vector3<T> result(vec);
	result.Normalize();
	return result;
}

template <class T> inline Vector4<T> Normalize(Vector4<T> const& vec) noexcept
{
	Vector4<T> result(vec);
	result.Normalize();
	return result;
}

/************************************************************************/
/* Векторное произведение векторов                                      */
/************************************************************************/
template <class T> Vector3<T> Cross(Vector3<T> const& a, Vector3<T> const& b) noexcept
{
	/*
	Векторное произведение векторов A и B равно определителю матрицы:
	      |i  j  k |
	AxB = |Ax Ay Az|
	      |Bx By Bz|
	*/
	return Vector3<T>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);

}

/************************************************************************/
/* Трансформация векторов (произведение матрицы и вектора)              */
/************************************************************************/
template <class T> Vector4<T> operator*(Matrix4<T> const& mat, Vector4<T> const& vec) noexcept
{
	return {
		mat.a00 * vec.x + mat.a01 * vec.y + mat.a02 * vec.z + mat.a03 * vec.w,
		mat.a10 * vec.x + mat.a11 * vec.y + mat.a12 * vec.z + mat.a13 * vec.w,
		mat.a20 * vec.x + mat.a21 * vec.y + mat.a22 * vec.z + mat.a23 * vec.w,
		mat.a30 * vec.x + mat.a31 * vec.y + mat.a32 * vec.z + mat.a33 * vec.w
	};
}

template <class T> Vector3<T> operator*(Matrix3<T> const& mat, Vector3<T> const& vec) noexcept
{
	return {
		mat.a00 * vec.x + mat.a01 * vec.y + mat.a02 * vec.z,
		mat.a10 * vec.x + mat.a11 * vec.y + mat.a12 * vec.z,
		mat.a20 * vec.x + mat.a21 * vec.y + mat.a22 * vec.z
	};
}

