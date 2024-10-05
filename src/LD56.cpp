// LD56.cpp : Defines the entry point for the application.
//

#include "LD56.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <thread>

#include "Util.h"

using namespace std;

SDL_Renderer *rnd;
SDL_Window *wnd;

shared_ptr<SDL_Texture> testImage;

thread gameUpdateThread;

thread::id mainThreadId;

Uint64 ticksHandledByGameStateUpdates;

void UpdateWorldState()
{
	cout << "UPDATE world state! " << ticksHandledByGameStateUpdates << endl;
}

void GameUpdateThread()
{
	auto ticksPerGameUpdate = SDL_GetPerformanceFrequency() / 60;

	Uint64 lastSecond = 0;
	auto updatesLastSecond = 0;

	while (true)
	{
		if (SDL_GetPerformanceCounter() > ticksHandledByGameStateUpdates)
		{
			ticksHandledByGameStateUpdates += ticksPerGameUpdate;
			UpdateWorldState();

			updatesLastSecond++;
		}
		else
		{
			SDL_Delay(1);
		}

		auto currentSecond = SDL_GetTicks64() / 1000;
		if (currentSecond != lastSecond)
		{
			cout << "Game Update FPS=" << updatesLastSecond << endl;
			updatesLastSecond = 0;
			lastSecond = currentSecond;
		}
	}
}

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

	SDL_RenderCopyExF(rnd, testImage.get(), nullptr, &trg, 20, nullptr, SDL_FLIP_NONE);

	SDL_RenderPresent(rnd);
}

void AltThreadEntryPoint()
{
	cout << "some other thread" << endl;
}

int main(int argc, char *argv[])
{
	mainThreadId = this_thread::get_id();

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	srand((unsigned int)time(nullptr));

	wnd = SDL_CreateWindow("LD56", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE);

	cout << "About to call emscripten_set_main_loop" << endl;
	emscripten_set_main_loop(MainLoop, 0, 0);

	rnd = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	gameUpdateThread = thread(GameUpdateThread);

	testImage = LoadTexture("assets/xt3.xcf");

	AssertOnMainThread();

	cout << "main() is done" << endl;

	return 0;
}
