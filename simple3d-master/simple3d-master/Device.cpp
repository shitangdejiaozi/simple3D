#include <windows.h>
#include "BasicMath.h"
#include "Device.h"


void Device_Init(device_t * device, IUINT32 color, IUINT32 * framebuffer, float * zbuffer, int render_state)
{
	device->background = color;
	device->foreground = 0;
	device->framebuffer = framebuffer;
	device->zbuffer = zbuffer;
	device->render_state = render_state;
}

void Device_Set_RenderState(device_t * device, int reander_state)
{

}