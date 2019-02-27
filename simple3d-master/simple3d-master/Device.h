#ifndef DEVICE
#define DEVICE

#define RENDER_STATE_WIREFARME  1  //�߿�ģʽ
#define RENDER_STATE_TEXTURE    2
#define RENDER_STATE_COLOR      4


typedef struct
{
	matrix_t world; //�ֲ�����������任
	matrix_t view;
	matrix_t projection;
	matrix_t transfrom;   
	float w, h;
} transform_t;

typedef struct Device_TYP
{
	transform_t transform; //����任��
	int width;
	int height;
	IUINT32 ** framebuffer;
	float ** zbuffer;
	IUINT32 ** texture;
	int tex_width;
	int tex_height;
	float max_u;
	float max_v;
	int render_state;
	IUINT32 background;
	IUINT32 foreground;
} device_t;

#endif // DEVICE



