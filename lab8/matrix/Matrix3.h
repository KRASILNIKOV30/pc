#pragma once
#include "../vector/Vector3.h"

#include <cassert>

template <class T> class Matrix3
{
public:
	Matrix3() noexcept
	{
		LoadIdentity();
	}

	explicit Matrix3(T const* src) noexcept
	{
		memcpy(&mat[0][0], src, sizeof(mat));
	}

	Matrix3(
		T const& v00, T const& v10, T const& v20, // Элементы 0 столбца
		T const& v01, T const& v11, T const& v21, // Элементы 1 столбца
		T const& v02, T const& v12, T const& v22 // Элементы 2 столбца
		) noexcept
	{
		mat[0][0] = v00;
		mat[0][1] = v10;
		mat[0][2] = v20;
		mat[1][0] = v01;
		mat[1][1] = v11;
		mat[1][2] = v21;
		mat[2][0] = v02;
		mat[2][1] = v12;
		mat[2][2] = v22;
	}

	T GetDeterminant() const noexcept
	{
		return
			mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) -
			mat[1][0] * (mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2]) +
			mat[2][0] * (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]);
	}

	void LoadIdentity() noexcept
	{
		mat[0][1] = mat[0][2] = mat[1][0] = mat[1][2] = mat[2][0] = mat[2][1] = 0;
		mat[0][0] = mat[1][1] = mat[2][2] = 1;
	}

	Vector3<T> GetRow(unsigned i) const noexcept
	{
		assert(i < 3);
		return
			Vector3<T>(
				mat[0][i],
				mat[1][i],
				mat[2][i]
				);
	}

	Vector3<T> GetColumn(unsigned i) const noexcept
	{
		assert(i < 3);
		return Vector3<T>(mat[i]);
	}

	void SetRow(unsigned i, const Vector3<T>& v) noexcept
	{
		assert(i < 3);
		mat[0][i] = v.x;
		mat[1][i] = v.y;
		mat[2][i] = v.z;
	}

	void SetColumn(unsigned i, const Vector3<T>& v) noexcept
	{
		assert(i < 3);
		mat[i][0] = v.x;
		mat[i][1] = v.y;
		mat[i][2] = v.z;
	}

	union
	{
		T mat[3][3];

		struct
		{
			T a00, a10, a20;
			T a01, a11, a21;
			T a02, a12, a22;
		};
	};
};

typedef Matrix3<double> Matrix3d;
typedef Matrix3<float> Matrix3f;