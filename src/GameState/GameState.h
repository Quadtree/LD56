#pragma once

#include <mutex>

#define MAX_BACTERIA 1024

class GameState
{
public:
    // this mutex is held either by the renderer, or by the game logic updater as it does an update
    std::mutex mutex;

    void DoUpdate(GameState &nextGameState);

    Bacteria BacteriaList[MAX_BACTERIA];
};
