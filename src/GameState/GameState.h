#pragma once

#include <mutex>
#include "Bacteria.h"
#include "SpatialIndex.h"

#define MAX_BACTERIA 5000

class MutationQueue
{
    void QueueMutation(std::function<void()> mutation);
};

struct AttractionPoint
{
    BacteriaType Type;
    int Faction;
    Vector2 Location;
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

    SpatialIndex<5, 500, Bacteria> MainSpatialIndex;

    std::vector<const Bacteria *> &GetBacteriaNear(Vector2 point, float radius) const;

    AttractionPoint AttractionPoints[4];

    void AddBacteria(Bacteria bacteria);

#if _DEBUG
    bool BeingRendered;
    bool BeingUpdated;
#endif
};
