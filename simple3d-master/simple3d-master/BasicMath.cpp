#include "BasicMath.h"

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