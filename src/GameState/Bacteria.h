#pragma once

#include "Vector2.h"
#include <functional>

class Bacteria
{
public:
    Vector2 Position;
    Vector2 Velocity;
    uint8_t Faction;
    uint8_t Health;

#if _DEBUG
    uint64_t NumUpdates;
#endif

    void Update1(Bacteria &nextState, class MutationQueue *queueMutation) const;

    // void Update1(const Bacteria &currentState, Bacteria &nextState, std::function<void(int)> queueMutation);
};
