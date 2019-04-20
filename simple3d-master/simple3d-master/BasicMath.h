#ifndef BASICMATH
#define BASICMATH

typedef unsigned int IUINT32;
#define PI         ((float)3.141592654f)
#define DEG_TO_RAD(ang) ((ang)*PI/180.0)  //角度转弧度

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
} VECTOR3D, *VECTOR3D_PTR, POINT3D, *POINT3D_PTR;

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
} VECTOR4D, POINT4D, *VECTOR4D_PTR , *POINT4D_PTR;



//矩阵相关 ////////////////////////////////////////////////////////
typedef struct MATRIX4X4_TYP
{
	union
	{
		float M[4][4];

		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};
	};

} MATRIX4X4, * MATRIX4X4_PTR;


void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR result);
void Mat_Init_4X4(MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33);

void Mat_Mul_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR  mb, MATRIX4X4_PTR result);

void Vector4D_Copy(VECTOR4D_PTR dst, VECTOR4D_PTR src);
void Vector4D_Init_XYZ(VECTOR4D_PTR vec, float x, float y, float z);
void Vector4D_Init(VECTOR4D_PTR dst, VECTOR4D_PTR src);
void Vector4D_Build(VECTOR4D_PTR src, VECTOR4D_PTR dst, VECTOR4D_PTR result);
void Vector4D_Cross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR result);
float Vector4D_Dot(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void Vector4D_Normalize(VECTOR4D_PTR va);
void Vector4D_Div_By_W(VECTOR4D_PTR va);
float Vector4D_Length(VECTOR4D_PTR va);
#endif // !BASICMATH

