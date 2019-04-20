#ifndef DEVICE
#define DEVICE

#define RENDER_STATE_WIREFARME  1  //线框模式
#define RENDER_STATE_TEXTURE    2
#define RENDER_STATE_COLOR      4

#define RENDERLIST_MAX_POLYS 32768
#define OBJECT_MAX_VERTICES           1024  // 
#define OBJECT_MAX_POLYS              1024 // 

#define POLY_STATE_ACTIVE             0x0001
#define POLY_STATE_CLIPPED            0x0002
#define POLY_STATE_BACKFACE           0x0004

#define POLY_ATTR_SHADE_MODE_CONSTANT 0x0020 //固定着色
#define POLY_ATTR_SHADE_MODE_FLAT     0x0040 //恒定着色
#define POLY_ATTR_SHADE_MODE_GOURAUD  0x0080 

#define POLY_ATTR_2SIDE               0x0001

#define OBJECT_STATE_ACTIVE           0x0001
#define OBJECT_STATE_VISIBLE          0x0002 
#define OBJECT_STATE_CULLED           0x0004

#define TRANSFORM_TRANS_ONLY 0
#define TRANSFORM_LOCAL_TO_TRANS 1
#define TRANSFORM_LOCAL_ONLY 2

#define LIGHT_ATTR_AMBIENT  0x0001  //环境光照
#define LIGHT_ATTR_INFINITE 0x0002  //无穷远光
#define LIGHT_ATTR_POINT    0x0003  //点光源

#define LIGHT_STATE_ON    1        //光源打开
#define LIGHT_STATE_OFF   0        //关闭
#define MAX_LIGHTS        8       //最大光源数

#define RGBABIT(r,g,b,a) ((a) + ((b) << 8) + ((g) << 16) + ((r) << 24))
#define RGBAFROMINT(color, r, g, a){*r = ((0xff << 24 & color) >> 24); *g = ((0xff << 16 & color) >> 16); *b = (0xff << 8 & color) >> 8;}
#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))

typedef struct
{
	MATRIX4X4 model; //局部到世界坐标变换
	MATRIX4X4 view;
	MATRIX4X4 projection;
	MATRIX4X4 mvp;
} transform_t;

typedef struct CAMERA_TYP
{
	int state; //相机状态
	int attr;  //相机属性
	POINT4D pos;  //相机在世界坐标系的位置
	VECTOR4D dir;  //相机的欧拉角度或者是UVN模型的注视方向

	VECTOR4D u;
	VECTOR4D v;
	VECTOR4D n;
	POINT4D target;  //uvn模型的目标位置

	float fov;  //水平和垂直方向的视野
	float near_clip_z; // 近裁剪面z值
	float far_clip_z;  //远裁剪面z值

	float viewplane_width; //视平面大小
	float viewplane_height;

	int viewport_width;  //屏幕/视口的大小
	int viewport_height;

	float view_dist; //视距
	float aspect_ratio;  //宽高比

	MATRIX4X4 view; //世界到相机矩阵
	MATRIX4X4 projection; //相机到视平面
	MATRIX4X4 screen;//视平面到屏幕
	MATRIX4X4 ps; //相机到屏幕

}CAMERA, *CAMERA_PTR;

typedef struct Device_TYP
{
	transform_t transform; //坐标变换器
	int width;
	int height;
	IUINT32 * framebuffer;
	float * zbuffer;
	int tex_width;
	int tex_height;
	float max_u;
	float max_v;
	int render_state;
	IUINT32 background;
	IUINT32 foreground;
	CAMERA camera;
} device_t, * device_PTR;

typedef struct POLY_TYP
{
	int state;
	int attr;
	int color;
	int lightcolor;
	POINT4D_PTR vertex_list;//顶点列表
	int vert[3];            //顶点索引
}POLY, *POLY_PTR;

//在光栅化阶段需要自包含的多边形面结构，不依赖于外部的顶点列表，自带顶点数据
typedef struct POLYF_TYP
{
	int state;
	int attr;
	int color;

	POINT4D vlist[3]; //原始的顶点数据
	POINT4D tvlist[3]; //变换后的顶点
}POLYF, *POLYF_PTR;

//物体结构
typedef struct OBJECT_TYP
{
	int id;
	char name[100];
	int state;
	int attr;
	int max_radius; //最大半径，用于物体剔除
	POINT4D world_pos;
	VECTOR4D dir; //局部坐标系中轴的旋转角度
	int num_vertices; //顶点数
	POINT4D vlist_local[OBJECT_MAX_VERTICES]; //顶点数组
	POINT4D vlist_trans[OBJECT_MAX_VERTICES];

	int num_polys;
	POLY plist[OBJECT_MAX_VERTICES];
}OBJECT, *OBJECT_PTR;

//多边形列表，
typedef struct RENDERLIST_TYP
{
	int state;
	int attr;

	POLYF_PTR poly_ptrs[RENDERLIST_MAX_POLYS]; //索引列表
	POLYF poly_data[RENDERLIST_MAX_POLYS];     //数据列表
	int num_polys; //多边形数

}RENDERLIST, *RENDERLIST_PTR;

//颜色格式
typedef struct RGBA_TYP
{
	union
	{
		int rgba;
		struct {
			unsigned char a, b, g, r;
		};
	};
}RGBA, *RGBA_PTR;

typedef struct LIGHT_TYP
{
	int state;
	int attr;
	int id;

	RGBA c_ambient; //环境光强度
	RGBA c_diffuse; //散射光强度
	RGBA c_specular; //镜面反射光强度
	POINT4D pos;
	VECTOR4D dir;
	float kc, kl, kq; //点光源的衰减因子

}LIGHT, *LIGHT_PTR;


extern LIGHT lights[MAX_LIGHTS];  // lights in system
extern int num_lights;              // current number of lights

void Device_Init(device_PTR device, int width, int height, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state);
void Device_Set_RenderState(device_PTR device, int reander_state);
void Device_Clear(device_PTR device);

//基本的渲染函数
void Device_Draw_Pixel(device_PTR device, int x, int y, IUINT32 color);
void Device_Draw_Line(device_PTR device, int x0, int y0, int x1, int y1, IUINT32 color);
void Device_Draw_Triangle(device_PTR device, int x1, int y1, int x2, int y2, int x3, int y3, IUINT32 color);
void Device_Draw_Top_Tri(device_PTR device, int x1, int y1, int x2, int y2, int x3, int y3, IUINT32 color);
void Device_Draw_Bottom_Tri(device_PTR device, int x1, int y1, int x2, int y2, int x3, int y3, IUINT32 color);

void GetRGBFromInt(int color, int *r, int *g, int *b);
//renderlist
void Reset_RENDERLIST(RENDERLIST_PTR  render_list);
int Insert_POLYF_RENDERLIST(RENDERLIST_PTR render_list, POLYF_PTR poly);
int Insert_POLY_RENDERLIST(RENDERLIST_PTR render_list, POLY_PTR poly, bool lighting = false);
int Insert_OBJECT_RENDERLIST(RENDERLIST_PTR render_list, OBJECT_PTR obj, bool insert_local, bool lighting = false);
void Transform_RENDERLIST(RENDERLIST_PTR render_list, MATRIX4X4_PTR mt, int coord_select);
void Remove_Backface_RENDERLIST(RENDERLIST_PTR render_list, CAMERA_PTR cam);

//object
void Reset_OBJECT(OBJECT_PTR obj);
void Transform_OBJECT(OBJECT_PTR obj, MATRIX4X4_PTR mt, int coord_select);
void Init_Camera(CAMERA_PTR cam, int cam_attr, POINT4D_PTR cam_pos, VECTOR4D_PTR cam_dir, POINT4D_PTR cam_target, float near_clip_z, float far_clip_z, float fov, float  viewport_width, float viewport_height, bool isNormalize);
void Remove_Backface_OBJECT(OBJECT_PTR obj, CAMERA_PTR cam);

//坐标变换
void Build_Model_To_World_Matrix4X4(VECTOR3D_PTR vpos, MATRIX4X4_PTR mt);
void Build_World_To_Camera_Matrix_Euler(CAMERA_PTR cam);
void Build_World_To_Camera_Matrix_UVN(CAMERA_PTR cam);
void Build_Camera_To_Perspective_Matrix(CAMERA_PTR cam);
void Build_Perspective_To_Screen_Matrix(CAMERA_PTR cam);
void Build_Camera_To_Screen_Matrix(CAMERA_PTR cam);
void Convert_From_Homogeneous4D_Renderlist(RENDERLIST_PTR render_list);
void Camera_To_Perspective_Renderlist(RENDERLIST_PTR render_list, CAMERA_PTR cam);
void Perspective_To_Screen_Renderlist(RENDERLIST_PTR render_list, CAMERA_PTR cam);
void Camera_To_Screen_Renderlist(RENDERLIST_PTR render_list, CAMERA_PTR cam);

void Build_XYZ_Rotation_Matrix(float a, float b, float c, MATRIX4X4_PTR mt);
void Draw_Renderlist_Wire(RENDERLIST_PTR render_list, device_PTR device);
void Draw_Renderlist_Solid(RENDERLIST_PTR render_list, device_PTR device);

//Light
void Reset_Lights_LIGHT();
int Init_Light_LIGHT(
	int index,
	int _state,
	int _attr,
	RGBA ambient, //环境光强度
	RGBA diffuse,
	RGBA specular,
	POINT4D_PTR pos, //光源位置
	VECTOR4D_PTR dir, //光源方向
	float kc, float kl, float kq
);
int Light_Object_World(OBJECT_PTR obj, CAMERA_PTR cam, LIGHT_PTR lights, int max_lights);
#endif // DEVICE



