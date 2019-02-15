#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <iostream>


#define  WINDOW_CLASS_NAME "WIN3DCLASS"
#define WINDOW_TITLE "窗口"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1: 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0: 1)

int Game_Init(void *parms = NULL);
int Game_ShutDown(void *parms = NULL);
int Game_Main(void *parms = NULL);

HWND main_window_handle = NULL;
HINSTANCE main_instance = NULL;
HDC screen_dc = NULL; //配套的窗口hdc
HBITMAP screen_hb = NULL;  //DIB,设备无关位图
HBITMAP screen_ob = NULL;  //老的Bitmap
unsigned char *screen_fb = NULL;
long screen_pitch = 0;

//事件处理函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_CREATE:
	{
		return 0;
	}break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0); //发送wm_quit消息
		return 0;
	}break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam)); //传递未处理的消息
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE  hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASS winclass;
	HWND hwnd;
	MSG msg;
	HDC hDc;
	LPVOID ptr;
	//创建windows类
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;  //事件处理程序的函数指针
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);//光标
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //绘制窗口的背景刷
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;  //类名

	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), WINDOW_WIDTH, WINDOW_HEIGHT, 1, 32, BI_RGB,//定义了DIB的度量和颜色信息
		WINDOW_WIDTH * WINDOW_HEIGHT * 4, 0, 0, 0, 0 } };

	if (!RegisterClass(&winclass))
		return 0;

	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hinstance, NULL)))
		return 0;

	main_window_handle = hwnd;
	main_instance = hinstance;

	hDc = GetDC(main_window_handle);
	screen_dc = CreateCompatibleDC(hDc); // 建立自己的DC，来用实现双缓冲
	ReleaseDC(main_window_handle, hDc);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0); //创建位图
	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb); //选入内存
	screen_fb = (unsigned char *)ptr;
	screen_pitch = WINDOW_WIDTH * 4;

	RECT window_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRectEx(&window_rect, GetWindowStyle(main_window_handle), GetMenu(main_window_handle) != NULL,
		GetWindowExStyle(main_window_handle));
	// now resize the window with a call to MoveWindow()
	MoveWindow(main_window_handle,
		0, // x position
		0, // y position
		window_rect.right - window_rect.left, // width
		window_rect.bottom - window_rect.top, // height
		FALSE);
	
	ShowWindow(main_window_handle, SW_SHOW);

	Game_Init();
	//构建实时事件循环
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//检测消息队列
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Game_Main();
	}
	Game_ShutDown();
	return (msg.wParam);
}

int Game_Init(void *parms)
{
	memset(screen_fb, 0, WINDOW_WIDTH * WINDOW_HEIGHT * 4);

	return 1;
}

int Game_ShutDown(void *parms)
{
	if (screen_dc)
	{
		if (screen_ob)
		{
			SelectObject(screen_dc, screen_ob);
			screen_ob = NULL;
		}
	}
	if (screen_hb)
	{
		DeleteObject(screen_hb);
		screen_hb = NULL;
	}
	if (main_window_handle)
	{
		CloseWindow(main_window_handle);
		main_window_handle = NULL;
	}
	return 1;
}

int Game_Main(void *parms)
{
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}

	HDC hDC = GetDC(main_window_handle);
	BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, screen_dc, 0, 0, SRCCOPY);//将位图从一个设备环境复制到另一个
	ReleaseDC(main_window_handle, hDC);
	Sleep(1);
	return 1;
}