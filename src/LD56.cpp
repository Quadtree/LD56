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

void InGameMainLoop();
void EnterInGameState();

int main(int argc, char *argv[])
{
	mainThreadId = this_thread::get_id();

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	srand((unsigned int)time(nullptr));

	wnd = SDL_CreateWindow("LD56", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE);

	EnterInGameState();

	rnd = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	testImage = LoadTexture("assets/xt3.xcf");

	AssertOnMainThread();

	cout << "main() is done" << endl;

	return 0;
}
