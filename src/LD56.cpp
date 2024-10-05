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

SDL_Texture *testImage;

thread altThread;

void MainLoop()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
	}

	SDL_SetRenderDrawColor(rnd, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rnd);

	SDL_FRect trg;
	trg.x = 20;
	trg.y = 20;
	trg.w = 32;
	trg.h = 32;

	SDL_RenderCopyExF(rnd, testImage, nullptr, &trg, 20, nullptr, SDL_FLIP_NONE);

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

	auto testSurf = IMG_Load("assets/xt3.xcf");
	DUMP(testSurf);
	testImage = SDL_CreateTextureFromSurface(rnd, testSurf);
	DUMP(testImage);

	cout << "main() is done" << endl;

	return 0;
}
