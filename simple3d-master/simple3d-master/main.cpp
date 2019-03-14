#include <SDL.h>
#include <iostream>
#include "BasicMath.h"
#include "Device.h"
using namespace std;

#define WINDOW_TITLE "sample3D"
const int SCREEN_WIDHT = 640;
const int SCREEN_HEIGHT = 480;

bool isViewdist_normalize = false;  //�Ӿ��Ƿ�Ϊ1����ƽ���Ƿ��һ��
bool isMergePs = true;   //�Ƿ�ϲ�͸�Ӻ���Ļ�任
CAMERA camera;
RENDERLIST rend_list;
POINT4D cam_pos = { 0, 0, -100, 1 };
VECTOR4D cam_dir = { 0, 0, 0, 1 };
POLYF poly1;
POINT3D poly_pos = { 0, 0, 100};

void GameInit()
{
	poly1.state = POLY_STATE_ACTIVE;
	poly1.attr = 0;
	poly1.color = RGB32BIT(0, 0, 255, 255);

	poly1.vlist[0].x = 0;
	poly1.vlist[0].y = 50;
	poly1.vlist[0].z = 0;
	poly1.vlist[0].w = 1;

	poly1.vlist[1].x = 50;
	poly1.vlist[1].y = -50;
	poly1.vlist[1].z = 0;
	poly1.vlist[1].w = 1;

	poly1.vlist[2].x = -50;
	poly1.vlist[2].y = -50;
	poly1.vlist[2].z = 0;
	poly1.vlist[2].w = 1;

	Init_Camera(&camera, 0, &cam_pos, &cam_dir, NULL, 50.0, 100.0, 90, SCREEN_WIDHT, SCREEN_HEIGHT, isViewdist_normalize);


}

int main(int argc, char *argv[])
{
	SDL_Window * window = NULL;
	SDL_Renderer * Renderer = NULL;
	SDL_Surface * screenSurface = NULL;

	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0))
	{
		printf("cont initi");
		return 0;
	}

	//��������
	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDHT, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("windows count create");
		return 0;
	}
	
	Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //�������ڵ���Ⱦ����ʹ��Ӳ������
	
	if (Renderer == NULL)
	{
		printf("renderer cont b created");
		return 0;
	}
	//��ʼ��render color
	SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF); 

	device_t device;
	//����
	IUINT32 * framebuffer = (IUINT32 *)malloc(SCREEN_WIDHT * SCREEN_HEIGHT * sizeof(IUINT32));
	float * zbuffer = (float *)malloc(SCREEN_WIDHT * SCREEN_HEIGHT * sizeof(float));
	Device_Init(&device, SCREEN_WIDHT, SCREEN_HEIGHT, 0x55555555, framebuffer, zbuffer, RENDER_STATE_WIREFARME);
	GameInit();
	while (1)
	{
		SDL_Event e;
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				break;
			}
		}

		//�����Ļ
		SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(Renderer);
		Device_Clear(&device);

		//Device_Draw_Line(&device, 0, 0, 200, 200, 0x123457);
		Reset_RENDERLIST(&rend_list);
		Insert_POLYF_RENDERLIST(&rend_list, &poly1);
		MATRIX4X4 mt;
		cout << "model to world" << endl;
		Build_Model_To_World_Matrix4X4(&poly_pos, &mt);
		Transform_RENDERLIST(&rend_list, &mt, TRANSFORM_LOCAL_TO_TRANS);

		cout << "world to camera" << endl;
		Build_World_To_Camera_Matrix_Euler(&camera);
		Transform_RENDERLIST(&rend_list, &camera.view, TRANSFORM_TRANS_ONLY);
		cout << "camera to screen" << endl;
		if (isViewdist_normalize)
		{
			Camera_To_Perspective_Renderlist(&rend_list, &camera); //�Ӿ�Ϊ1�� fov =90��Ĭ��ʹ�÷ֿ��ļ��㣬���Ϊ����������úϲ���ʽ
			Perspective_To_Screen_Renderlist(&rend_list, &camera);
		}
		else
		{
			if (isMergePs)
			{
				Camera_To_Screen_Renderlist(&rend_list, &camera); 
			}
			else
			{
				Build_Camera_To_Screen_Matrix(&camera);
				Transform_RENDERLIST(&rend_list, &camera.ps, TRANSFORM_TRANS_ONLY);

				Convert_From_Homogeneous4D_Renderlist(&rend_list);
			}
			
			
		}
		
		
		Draw_Renderlist_Wire(&rend_list, &device);

		//����framebuffer���rendercolor
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			for (int x = 0; x < SCREEN_WIDHT; x++)
			{
				IUINT32 color = framebuffer[y * SCREEN_WIDHT + x];
				SDL_SetRenderDrawColor(Renderer, (0xff << 16 & color) >> 16, (0xff << 8 & color) >> 8, 0xff & color, (0xff << 24 & color) >> 24);
				SDL_RenderDrawPoint(Renderer, x, y);
				//SDL_RenderDrawLine(Renderer, 0, 0, 200, 200);
			}
		}
		//������Ļ
		SDL_RenderPresent(Renderer);
	}
	
	free(framebuffer);
	free(zbuffer);

	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(window);
	Renderer = NULL;
	SDL_Quit();
	return 1;
}