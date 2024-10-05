#include "LD56.h"

thread gameUpdateThread;
Uint64 ticksHandledByGameStateUpdates;

extern SDL_Renderer *rnd;
extern SDL_Window *wnd;

void UpdateWorldState()
{
    // cout << "UPDATE world state! " << ticksHandledByGameStateUpdates << endl;
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

void InGameMainLoop()
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

void EnterInGameState()
{
    cout << "About to call emscripten_set_main_loop" << endl;
    emscripten_set_main_loop(InGameMainLoop, 0, 0);

    gameUpdateThread = thread(GameUpdateThread);
}