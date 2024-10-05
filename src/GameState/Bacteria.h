#pragma once

#include "Vector2.h"
#include <functional>

enum class BateriaType : uint8_t
{
    Invalid,
    Converter,
    Swarmer,
    Gobbler,
    Zoomer,
    Spitter,
}

class Bacteria
{
public:
    Vector2 Position;
    Vector2 Velocity;
    uint8_t Faction;
    uint8_t Health;
    BateriaType Type;

#if _DEBUG
    uint64_t NumUpdates;
#endif

    void Update1(Bacteria &nextState, class MutationQueue *queueMutation) const;

    void Render(SDL_Renderer* rnd, Camera& camera);

    // void Update1(const Bacteria &currentState, Bacteria &nextState, std::function<void(int)> queueMutation);
};
