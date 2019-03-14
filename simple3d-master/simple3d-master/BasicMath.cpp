#include "BasicMath.h"
#include <math.h>

void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR result)
{
	for (int col = 0; col < 4; col++)
	{
		int sum = 0;
		for (int row = 0; row < 4; row++)
		{
			sum += (va->M[row] * mb->M[row][col]);
		}
		result->M[col] = sum;
	}
}

void Mat_Init_4X4(MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02; ma->M03 = m03;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12; ma->M13 = m13;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22; ma->M23 = m23;
	ma->M30 = m30; ma->M31 = m31; ma->M32 = m32; ma->M33 = m33;
}

void Mat_Mul_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR  mb, MATRIX4X4_PTR result)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			int sum = 0;

			for (int i = 0; i < 4; i++)
			{
				sum += (ma->M[row][i] * mb->M[i][col]);
			}

			result->M[row][col] = sum;
		}
	}
}

void Vector4D_Copy(VECTOR4D_PTR dst, VECTOR4D_PTR src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->w = src->w;
}
void Vector4D_Init_XYZ(VECTOR4D_PTR vec, float x, float y, float z)
{
	vec->x = x;
	vec->y = y;
	vec->z = z;
	vec->w = 1.0;
}

void Vector4D_Init(VECTOR4D_PTR dst, VECTOR4D_PTR src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
	dst->w = src->w;

}

void Vector4D_Build(VECTOR4D_PTR src, VECTOR4D_PTR dst, VECTOR4D_PTR result)
{
	result->x = dst->x - src->x;
	result->y = dst->y - src->y;
	result->z = dst->z - src->z;
	result->w = 1;
}

void Vector4D_Cross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR result)
{
	result->x = ((va->y * vb->z) - (va->z * vb->y));
	result->y = -((va->x * vb->z) - (va->z * vb->x));
	result->z = ((va->x * vb->y) - (va->y * vb->x));
	result->w = 1;
	
}

void Vector4D_Normalize(VECTOR4D_PTR va)
{
	// compute length
	float length = sqrtf(va->x*va->x + va->y*va->y + va->z*va->z);

	float length_inv = 1.0 / length;

	// compute normalized version of vector
	va->x *= length_inv;
	va->y *= length_inv;
	va->z *= length_inv;
	va->w = 1;
}
void Vector4D_Div_By_W(VECTOR4D_PTR va)
{
	va->x = va->x / va->w;
	va->y = va->y / va->w;
	va->z = va->z / va->w;
	va->w = 1;
}
