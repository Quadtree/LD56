#pragma once

#include <mutex>
#include "Bacteria.h"

#define MAX_BACTERIA 1024

class GameState
{
public:
    GameState();

    // this mutex is held either by the renderer, or by the game logic updater as it does an update
    std::mutex mutex;

    void DoUpdate(GameState &nextGameState);

    Bacteria BacteriaList[MAX_BACTERIA];
    int NumActiveBacteria;
};
