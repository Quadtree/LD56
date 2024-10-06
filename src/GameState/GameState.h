#pragma once

#include <mutex>
#include "Bacteria.h"
#include "SpatialIndex.h"

#define MAX_BACTERIA 5000
#define MUTATION_QUEUE_PRIORITY_LEVELS 2

class MutationQueue
{
public:
    // mutations need to happen in order in order to avoid some very odd situations
    // such as bacteria getting hit twice in one turn, but the first one kills it
    // then another bacteria replaces it
    // and the second hit hits the new one
    // thus, priority is a set of phases
    // 0 - Creation
    // 1 - Damage
    void QueueMutation(int priority, std::function<void(GameState *)> mutation);

    vector<std::function<void(GameState *)>> mutationQueues[MUTATION_QUEUE_PRIORITY_LEVELS];

    mutex mutationQueueMutex;
};

struct AttractionPoint
{
    bool Types[(int)BacteriaType::Max];
    Vector2 Location;
};

class GameState
{
public:
    GameState();

    // this mutex is held either by the renderer, or by the game logic updater as it does an update
    std::mutex mutex;

    void DoUpdate(GameState &nextGameState, TerrainType terrain[TERRAIN_GRID_SIZE * TERRAIN_GRID_SIZE]);

    Bacteria BacteriaList[MAX_BACTERIA];
    int NumActiveBacteria;

    SpatialIndex<5, 500, Bacteria> MainSpatialIndex;

    std::vector<const Bacteria *> &GetBacteriaNear(Vector2 point, float radius) const;

    AttractionPoint AttractionPoints[4];

    void AddBacteria(Bacteria bacteria);

    void Reset();

    bool WasDeath;
    bool WasAttack;

    int LivingBacteriaLastFrame;

#if _DEBUG
    bool BeingRendered;
    bool BeingUpdated;
#endif
};
