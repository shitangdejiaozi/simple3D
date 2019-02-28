#ifndef BASICMATH
#define BASICMATH

typedef unsigned int IUINT32;

//向量，顶点相关////////////////////////////////////////////////////
typedef struct VECTOR3D_TYP
{
	union
	{
		float M[3];
		struct
		{
			float x, y, z;
		};
	};
} VECTOR3D, *VECTOR3D_PTR;

typedef struct VECTOR4D_TYP
{
	union
	{
		float M[4];
		struct
		{
			float x, y, z, w;
		};
	};
} VECTOR4D, POINT4D, *VECTOR4D_PTR;



//矩阵相关 ////////////////////////////////////////////////////////
typedef struct MATRIX4X4_TYP
{
	union
	{
		float m[4][4];

		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};
	};

} MATRIX4X4;

#endif // !BASICMATH

