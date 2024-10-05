#include "LD56.h"
#include "Util.h"
#include "GameState/GameState.h"

thread gameUpdateThread;
Uint64 ticksHandledByGameStateUpdates;

extern SDL_Renderer *rnd;
extern SDL_Window *wnd;

shared_ptr<SDL_Texture> testImage;

GameState gameStates[2];
int currentGameState = 0;
int nextGameStateToUpdate = 1;

bool showingDebugInfo = false;
int currentFPS = 0;

void UpdateWorldState()
{
    lock_guard gameStateLock(gameStates[nextGameStateToUpdate].mutex);

    gameStates[currentGameState].DoUpdate(gameStates[nextGameStateToUpdate]);

    nextGameStateToUpdate = currentGameState;
    currentGameState = 1 - currentGameState;
}

void GameUpdateThread()
{
    auto ticksPerGameUpdate = SDL_GetPerformanceFrequency() / 60;

    Uint64 lastSecond = 0;
    auto updatesLastSecond = 0;

    ticksHandledByGameStateUpdates = SDL_GetPerformanceCounter();

    while (true)
    {
        if (SDL_GetPerformanceCounter() > ticksHandledByGameStateUpdates)
        {
            ticksHandledByGameStateUpdates += ticksPerGameUpdate;

            auto startTime = GetTimeAsDouble();
            UpdateWorldState();
            auto endTime = GetTimeAsDouble();
            // cout << "World update time: " << (int)((endTime - startTime) * 1000) << "ms" << endl;

            updatesLastSecond++;
        }
        else
        {
            SDL_Delay(0);
        }

        auto currentSecond = SDL_GetTicks64() / 1000;
        if (currentSecond != lastSecond)
        {
            cout << "Game Update FPS=" << updatesLastSecond << endl;
            currentFPS = updatesLastSecond;
            updatesLastSecond = 0;
            lastSecond = currentSecond;
        }
    }
}

void InGameMainLoop()
{
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        if (evt.type == SDL_KEYDOWN)
        {
            if (evt.key.keysym.sym == SDLK_F3)
            {
                cout << "F3 pressed" << endl;
            }
        }
    }

    SDL_SetRenderDrawColor(rnd, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rnd);

    {
        auto gameStateBeingRendered = currentGameState;

        lock_guard rendererLockGuard(gameStates[gameStateBeingRendered].mutex);

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = true;
#endif

        for (auto i = 0; i < gameStates[gameStateBeingRendered].NumActiveBacteria; ++i)
        {
#if _DEBUG
            if (gameStates[gameStateBeingRendered].BeingUpdated)
            {
                throw "Game state is also being updated!!!";
            }
#endif
            SDL_FRect trg;
            trg.x = gameStates[gameStateBeingRendered].BacteriaList[i].Position.X;
            trg.y = gameStates[gameStateBeingRendered].BacteriaList[i].Position.Y;
            trg.w = 32;
            trg.h = 32;

            SDL_RenderCopyExF(rnd, testImage.get(), nullptr, &trg, 20, nullptr, SDL_FLIP_NONE);
        }

#if _DEBUG
        gameStates[gameStateBeingRendered].BeingRendered = false;
#endif
    }

    SDL_RenderPresent(rnd);
}

void EnterInGameState()
{
    emscripten_cancel_main_loop();

    cout << "About to call emscripten_set_main_loop" << endl;
    emscripten_set_main_loop(InGameMainLoop, 0, 0);

    gameUpdateThread = thread(GameUpdateThread);

    testImage = LoadTexture("assets/xt3.xcf");
}