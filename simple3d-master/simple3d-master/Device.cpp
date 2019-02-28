#include <windows.h>
#include "BasicMath.h"
#include "Device.h"


void Device_Init(device_t * device, int width, int height, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state)
{
	device->width = width;
	device->height = height;
	device->background = color;
	device->foreground = 0;
	device->framebuffer = framebuffer;
	device->zbuffer = zbuffer;
	device->render_state = render_state;
}

void Device_Set_RenderState(device_t * device, int reander_state)
{
	device->render_state = reander_state;
}

//ÏòframbufferÌî³äÏñËØ
void Device_Draw_Pixel(device_t * device, int x, int y, IUINT32 color)
{
	device->framebuffer[y * device->width + x] = color;
}

//»®Ïß
void Device_Draw_Line(device_t * device, int x0, int y0, int x1, int y1, IUINT32 color)
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
