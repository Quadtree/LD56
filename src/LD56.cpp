// LD56.cpp : Defines the entry point for the application.
//

#include "LD56.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <thread>

using namespace std;

SDL_Renderer *rnd;
SDL_Window *wnd;

thread altThread;

void MainLoop()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
	}

	SDL_RenderPresent(rnd);
}

void AltThreadEntryPoint()
{
	cout << "some other thread" << endl;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	// TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	srand((unsigned int)time(nullptr));

	wnd = SDL_CreateWindow("LD56", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE);

	cout << "About to call emscripten_set_main_loop" << endl;
	emscripten_set_main_loop(MainLoop, 0, 0);

	rnd = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	altThread = thread(AltThreadEntryPoint);

	cout << "main() is done" << endl;

	return 0;
}
