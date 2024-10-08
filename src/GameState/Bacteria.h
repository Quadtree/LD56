#pragma once

#include "Vector2.h"
#include <functional>

enum class BacteriaType : uint8_t
{
    Invalid,
    Converter,
    Swarmer,
    Gobbler,
    Zoomer,
    Spitter,
    Max,
};

class Bacteria
{
public:
    Vector2 Position;
    Vector2 Velocity;
    int8_t Faction;
    int8_t Health;
    BacteriaType Type;
    uint16_t ID;
    int16_t AttackCharge;

#if _DEBUG
    uint64_t NumUpdates;
#endif

    void Update1(Bacteria &nextState, const class GameState *curGameState, class MutationQueue *queueMutation, TerrainType terrain[TERRAIN_GRID_SIZE * TERRAIN_GRID_SIZE]) const;

    void Render(class SDL_Renderer *rnd, class Camera &camera, double elapsedTime, double timeSinceLastGameUpdate) const;

    // void Update1(const Bacteria &currentState, Bacteria &nextState, std::function<void(int)> queueMutation);
};
