#ifndef DEVICE
#define DEVICE

#define RENDER_STATE_WIREFARME  1  //线框模式
#define RENDER_STATE_TEXTURE    2
#define RENDER_STATE_COLOR      4


typedef struct
{
	MATRIX4X4 model; //局部到世界坐标变换
	MATRIX4X4 view;
	MATRIX4X4 projection;
	MATRIX4X4 mvp;
} transform_t;

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
} device_t;

typedef struct CAMERA_TYP
{
	int state; //相机状态
	POINT4D pos;  //相机在世界坐标系的位置
	VECTOR4D dir;  //相机的欧拉角度或者是UVN模型的注视方向

	VECTOR4D u;
	VECTOR4D v;
	VECTOR4D n;
	POINT4D target;  //uvn模型的目标位置

	float fov;  //水平和垂直方向的视野
	float near_clip_z; // 近裁剪面z值
	float far_clip_z;  //远裁剪面z值

	int viewport_width;  //屏幕/视口的大小
	int viewport_height;

	float aspect_ratio;  //宽高比

}CAMERA, *CAMERA_PTR;

void Device_Init(device_t * device, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state);
void Device_Set_RenderState(device_t * device, int reander_state);

//基本的渲染函数
void Device_Draw_Pixel(device_t * device, int x, int y, int color);
void Device_Draw_Line(device_t * device, int x0, int y0, int x1, int y1, int color);

#endif // DEVICE



