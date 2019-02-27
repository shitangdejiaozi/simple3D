#include <SDL.h>
#include <iostream>

#define WINDOW_TITLE "sample3D"
const int SCREEN_WIDHT = 640;
const int SCREEN_HEIGHT = 480;



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

	//创建窗口
	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDHT, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("windows count create");
		return 0;
	}
	
	Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //创建窗口的渲染器，使用硬件加速
	
	if (Renderer == NULL)
	{
		printf("renderer cont b created");
		return 0;
	}
	//初始化render color
	SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF); 

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

		SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(Renderer);

		//根据framebuffer填充rendercolor

		//更新屏幕
		SDL_RenderPresent(Renderer);
	}
	
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(window);
	Renderer = NULL;
	SDL_Quit();
	return 1;
}