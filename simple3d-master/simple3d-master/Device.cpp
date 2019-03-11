#include <windows.h>
#include "BasicMath.h"
#include "Device.h"


void Device_Init(device_PTR device, int width, int height, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state)
{
	device->width = width;
	device->height = height;
	device->background = color;
	device->foreground = 0;
	device->framebuffer = framebuffer;
	device->zbuffer = zbuffer;
	device->render_state = render_state;
}

void Device_Set_RenderState(device_PTR device, int reander_state)
{
	device->render_state = reander_state;
}

void Device_Clear(device_PTR device)
{
	if (device->framebuffer != NULL)
	{
		for (int y = 0; y < device->height; y++)
		{
			for (int x = 0; x < device->width;x++)
			{
				device->framebuffer[y * device->width + x] = device->background;
			}
		}
	}

	if (device->zbuffer != NULL)
	{
		memset(device->zbuffer, 0, device->width * device->height * sizeof(float));
	}
}



//向frambuffer填充像素
void Device_Draw_Pixel(device_PTR device, int x, int y, IUINT32 color)
{
	device->framebuffer[y * device->width + x] = color;
}

//划线
void Device_Draw_Line(device_PTR device, int x0, int y0, int x1, int y1, IUINT32 color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int ux = ((dx > 0) << 1) - 1;
	int uy = ((dy > 0) << 1) - 1;
	int error = 0;
	if (dx > dy)
	{
		for (int x = x0, y = y0; x <= x1; x += ux)
		{
			Device_Draw_Pixel(device, x, y, color);
			error += dy;
			if (error >= dx)
			{
				y += uy;
				error -= dx;
			}
		}
	}
	else
	{
		for (int y = y0, x = x0; y < y1; y += uy)
		{
			Device_Draw_Pixel(device, x, y, color);
			error += dx;
			if (error >= dy)
			{
				x += ux;
				error -= dy;
			}
		}
	}
}

void Reset_RENDERLIST(RENDERLIST_PTR  render_list)
{
	render_list->num_polys = 0;
}

//将多边形插入到渲染列表
int Insert_POLYF_RENDERLIST(RENDERLIST_PTR render_list, POLYF_PTR poly)
{
	if (render_list->num_polys == RENDERLIST_MAX_POLYS)
		return 0;

	int num = render_list->num_polys;
	render_list->poly_ptrs[num] =  &render_list->poly_data[num];
	memcpy((void *)&render_list->poly_data[num], (void *)poly, sizeof(POLYF));
	render_list->num_polys++;
	return 1;
}

//通用的渲染列表 变换函数
void Transform_RENDERLIST(RENDERLIST_PTR render_list, MATRIX4X4_PTR mt)
{
	for (int poly = 0; poly < render_list->num_polys; poly++)
	{
		POLYF_PTR curr_poly = render_list->poly_ptrs[poly];

		if (curr_poly == NULL || !(curr_poly->state & POLY_STATE_ACTIVE) || curr_poly->state & POLY_STATE_CLIPPED || curr_poly->state & POLY_STATE_BACKFACE)
			continue;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &curr_poly->tvlist[vertex]);
		}
	}
}

//构建局部到世界的变换矩阵
void Build_Model_To_World_Matrix4X4(VECTOR3D_PTR vpos, MATRIX4X4_PTR mt)
{
	Mat_Init_4X4(mt, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		vpos->x, vpos->y, vpos->z, 1);
}

