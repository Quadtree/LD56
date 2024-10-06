// LD56.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <memory>
#include <thread>

using namespace std;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#define DUMP(x) cout << #x << "=" << x << endl;

#define RAISE_ERROR(x)                                                          \
    {                                                                           \
        cout << "ERROR (" << __FILE__ << ":" << __LINE__ << "): " << x << endl; \
        exit(2);                                                                \
    }

#define SQUARE(x) (x * x)

void SubmitToThreadPool(function<void()> func);
void WaitForThreadPoolToFinishAllTasks();
void ExpectThreadPoolToBeEmpty();

#define TERRAIN_GRID_SIZE 512

enum class TerrainType : uint8_t
{
    Invalid,
    Clear,
    Rough,
    Obstructed,
    Max,
};

void EnterTitleScreenState();
void EnterMainMenuState();
TTF_Font *GetFont(int ptSize);
void CallTearDownFunction();
void EnterMessageScreen(string message, function<void()> next);
void EnterInGameState(int levelNumber);

#define CHEATS_ENABLED 1

#define UPDATES_PER_SECOND 10

#define NUM_LEVELS 5