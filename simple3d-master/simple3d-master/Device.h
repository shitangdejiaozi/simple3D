#ifndef DEVICE
#define DEVICE

#define RENDER_STATE_WIREFARME  1  //�߿�ģʽ
#define RENDER_STATE_TEXTURE    2
#define RENDER_STATE_COLOR      4


typedef struct
{
	MATRIX4X4 model; //�ֲ�����������任
	MATRIX4X4 view;
	MATRIX4X4 projection;
	MATRIX4X4 mvp;
} transform_t;

typedef struct Device_TYP
{
	transform_t transform; //����任��
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
	int state; //���״̬
	POINT4D pos;  //�������������ϵ��λ��
	VECTOR4D dir;  //�����ŷ���ǶȻ�����UVNģ�͵�ע�ӷ���

	VECTOR4D u;
	VECTOR4D v;
	VECTOR4D n;
	POINT4D target;  //uvnģ�͵�Ŀ��λ��

	float fov;  //ˮƽ�ʹ�ֱ�������Ұ
	float near_clip_z; // ���ü���zֵ
	float far_clip_z;  //Զ�ü���zֵ

	int viewport_width;  //��Ļ/�ӿڵĴ�С
	int viewport_height;

	float aspect_ratio;  //��߱�

}CAMERA, *CAMERA_PTR;

void Device_Init(device_t * device, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state);
void Device_Set_RenderState(device_t * device, int reander_state);

//��������Ⱦ����
void Device_Draw_Pixel(device_t * device, int x, int y, int color);
void Device_Draw_Line(device_t * device, int x0, int y0, int x1, int y1, int color);

#endif // DEVICE



