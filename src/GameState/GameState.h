#pragma once

#include <mutex>
#include "Bacteria.h"

#define MAX_BACTERIA 5000

class MutationQueue
{
    void QueueMutation(std::function<void()> mutation);
};

class GameState
{
public:
    GameState();

    // this mutex is held either by the renderer, or by the game logic updater as it does an update
    std::mutex mutex;

    void DoUpdate(GameState &nextGameState);

    Bacteria BacteriaList[MAX_BACTERIA];
    int NumActiveBacteria;

    std::vector<Bacteria *> &GetBacteriaNear(Vector2 point, float radius);

#if _DEBUG
    bool BeingRendered;
    bool BeingUpdated;
#endif
};
