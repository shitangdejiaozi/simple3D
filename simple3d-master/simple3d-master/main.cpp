#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <iostream>


#define  WINDOW_CLASS_NAME "WIN3DCLASS"
#define WINDOW_TITLE "����"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1: 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0: 1)

int Game_Init(void *parms = NULL);
int Game_ShutDown(void *parms = NULL);
int Game_Main(void *parms = NULL);

HWND main_window_handle = NULL;
HINSTANCE main_instance = NULL;
HDC screen_dc = NULL; //���׵Ĵ���hdc
HBITMAP screen_hb = NULL;  //DIB,�豸�޹�λͼ
HBITMAP screen_ob = NULL;  //�ϵ�Bitmap
unsigned char *screen_fb = NULL;
long screen_pitch = 0;

//�¼�������
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
		PostQuitMessage(0); //����wm_quit��Ϣ
		return 0;
	}break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam)); //����δ�������Ϣ
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE  hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASS winclass;
	HWND hwnd;
	MSG msg;
	HDC hDc;
	LPVOID ptr;
	//����windows��
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;  //�¼��������ĺ���ָ��
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);//���
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //���ƴ��ڵı���ˢ
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;  //����

	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), WINDOW_WIDTH, WINDOW_HEIGHT, 1, 32, BI_RGB,//������DIB�Ķ�������ɫ��Ϣ
		WINDOW_WIDTH * WINDOW_HEIGHT * 4, 0, 0, 0, 0 } };

	if (!RegisterClass(&winclass))
		return 0;

	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hinstance, NULL)))
		return 0;

	main_window_handle = hwnd;
	main_instance = hinstance;

	hDc = GetDC(main_window_handle);
	screen_dc = CreateCompatibleDC(hDc); // �����Լ���DC������ʵ��˫����
	ReleaseDC(main_window_handle, hDc);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0); //����λͼ
	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb); //ѡ���ڴ�
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
	//����ʵʱ�¼�ѭ��
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//�����Ϣ����
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
	BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, screen_dc, 0, 0, SRCCOPY);//��λͼ��һ���豸�������Ƶ���һ��
	ReleaseDC(main_window_handle, hDC);
	Sleep(1);
	return 1;
}