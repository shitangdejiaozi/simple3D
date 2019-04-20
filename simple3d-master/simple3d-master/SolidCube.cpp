//#include <SDL.h>
//#include <iostream>
//#include "BasicMath.h"
//#include "Device.h"
//using namespace std;
//
//#define WINDOW_TITLE "sample3D"
//const int SCREEN_WIDHT = 640;
//const int SCREEN_HEIGHT = 480;
//
//bool isViewdist_normalize = false;  //视距是否为1，视平面是否归一化
//bool isMergePs = true;   //是否合并透视和屏幕变换
//CAMERA camera;
//RENDERLIST rend_list;
//POINT4D cam_pos = { 0, 0,0, 1 };
//VECTOR4D cam_dir = { 0, 0, 0, 1 };
//
//OBJECT cube;
//POINT3D cube_pos = { 0, 0, 100 };
//void GameInit()
//{
//
//	cube.id = 0;
//	cube.state = OBJECT_STATE_ACTIVE | OBJECT_STATE_VISIBLE;
//	cube.attr = 0;
//	cube.world_pos = { 0, 0, 0, 1 };
//	cube.num_vertices = 8;
//
//	POINT3D temp_verts[8] =
//	{ {10, 10, 10},
//	{10, 10, -10},
//	{-10, 10, -10},
//	{-10, 10, 10},
//	{10, -10, 10},
//	{-10, -10, 10},
//	{-10, -10, -10},
//	{10, -10, -10} };
//	for (int vertex = 0; vertex < 8; vertex++)
//	{
//		cube.vlist_local[vertex].x = temp_verts[vertex].x;
//		cube.vlist_local[vertex].y = temp_verts[vertex].y;
//		cube.vlist_local[vertex].z = temp_verts[vertex].z;
//		cube.vlist_local[vertex].w = 1;
//
//	}
//
//	cube.num_polys = 12;
//	int temp_poly_indices[36] =
//	{
//		0, 1, 2, 0, 2, 3,
//		0, 7, 1, 0, 4, 7,
//		1, 7, 6, 1, 6, 2,
//		2, 6, 5, 3, 2, 5,
//		0, 5, 4, 0, 3, 5,
//		5, 6, 7, 4, 5, 7
//	};
//
//	for (int tri = 0; tri < 12; tri++)
//	{
//		cube.plist[tri].state = POLY_STATE_ACTIVE;
//		cube.plist[tri].attr = 0;
//		cube.plist[tri].color = RGBABIT(0, 255, 255, 0);
//		cube.plist[tri].vertex_list = cube.vlist_local;
//
//		cube.plist[tri].vert[0] = temp_poly_indices[tri * 3 + 0];
//		cube.plist[tri].vert[1] = temp_poly_indices[tri * 3 + 1];
//		cube.plist[tri].vert[2] = temp_poly_indices[tri * 3 + 2];
//
//	}
//
//	Init_Camera(&camera, 0, &cam_pos, &cam_dir, NULL, 50.0, 100.0, 90, SCREEN_WIDHT, SCREEN_HEIGHT, isViewdist_normalize);
//
//
//}
//
//int main(int argc, char *argv[])
//{
//	SDL_Window * window = NULL;
//	SDL_Renderer * Renderer = NULL;
//	SDL_Surface * screenSurface = NULL;
//
//	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0))
//	{
//		printf("cont initi");
//		return 0;
//	}
//
//	//创建窗口
//	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//		SCREEN_WIDHT, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//	if (window == NULL)
//	{
//		printf("windows count create");
//		return 0;
//	}
//
//	Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //创建窗口的渲染器，使用硬件加速
//
//	if (Renderer == NULL)
//	{
//		printf("renderer cont b created");
//		return 0;
//	}
//	//初始化render color
//	SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
//
//	device_t device;
//	//缓存
//	IUINT32 * framebuffer = (IUINT32 *)malloc(SCREEN_WIDHT * SCREEN_HEIGHT * sizeof(IUINT32));
//	float * zbuffer = (float *)malloc(SCREEN_WIDHT * SCREEN_HEIGHT * sizeof(float));
//	Device_Init(&device, SCREEN_WIDHT, SCREEN_HEIGHT, 0x55555555, framebuffer, zbuffer, RENDER_STATE_WIREFARME);
//	GameInit();
//	while (1)
//	{
//		SDL_Event e;
//		if (SDL_PollEvent(&e))
//		{
//			if (e.type == SDL_QUIT)
//			{
//				break;
//			}
//		}
//
//		//清空屏幕
//		SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
//		SDL_RenderClear(Renderer);
//		Device_Clear(&device);
//
//		//Device_Draw_Line(&device, 0, 0, 200, 200, 0x123457);
//		Reset_RENDERLIST(&rend_list);
//		Reset_OBJECT(&cube);
//
//		MATRIX4X4 rotation;
//		Build_XYZ_Rotation_Matrix(0, 30.0, 0, &rotation);
//		Transform_OBJECT(&cube, &rotation, TRANSFORM_LOCAL_TO_TRANS);
//
//		Insert_OBJECT_RENDERLIST(&rend_list, &cube, false);
//		cout << "model to world" << endl;
//		MATRIX4X4 mt;
//		Build_Model_To_World_Matrix4X4(&cube_pos, &mt);
//		Transform_RENDERLIST(&rend_list, &mt, TRANSFORM_TRANS_ONLY);
//
//		//开启背面消除
//		Remove_Backface_RENDERLIST(&rend_list, &camera);
//
//		cout << "world to camera" << endl;
//		Build_World_To_Camera_Matrix_Euler(&camera);
//		Transform_RENDERLIST(&rend_list, &camera.view, TRANSFORM_TRANS_ONLY);
//		cout << "camera to screen" << endl;
//		if (isViewdist_normalize)
//		{
//			Camera_To_Perspective_Renderlist(&rend_list, &camera); //视距为1， fov =90，默认使用分开的计算，如果为其他情况，用合并格式
//			Perspective_To_Screen_Renderlist(&rend_list, &camera);
//		}
//		else
//		{
//			if (isMergePs)
//			{
//				Camera_To_Screen_Renderlist(&rend_list, &camera);
//			}
//			else
//			{
//				Build_Camera_To_Screen_Matrix(&camera);
//				Transform_RENDERLIST(&rend_list, &camera.ps, TRANSFORM_TRANS_ONLY);
//
//				Convert_From_Homogeneous4D_Renderlist(&rend_list);
//			}
//
//
//		}
//
//		Draw_Renderlist_Solid(&rend_list, &device);
//		//Draw_Renderlist_Wire(&rend_list, &device);
//
//		//根据framebuffer填充rendercolor
//		for (int y = 0; y < SCREEN_HEIGHT; y++)
//		{
//			for (int x = 0; x < SCREEN_WIDHT; x++)
//			{
//				IUINT32 color = framebuffer[y * SCREEN_WIDHT + x];//color采用8.8.8.a格式
//				SDL_SetRenderDrawColor(Renderer, (0xff << 24 & color) >> 24,(0xff << 16 & color) >> 16, (0xff << 8 & color) >> 8, 0xff & color);
//				SDL_RenderDrawPoint(Renderer, x, y);
//				//SDL_RenderDrawLine(Renderer, 0, 0, 200, 200);
//			}
//		}
//		//更新屏幕
//		SDL_RenderPresent(Renderer);
//	}
//
//	free(framebuffer);
//	free(zbuffer);
//
//	SDL_DestroyRenderer(Renderer);
//	SDL_DestroyWindow(window);
//	Renderer = NULL;
//	SDL_Quit();
//	return 1;
//}