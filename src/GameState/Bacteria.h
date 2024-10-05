#pragma once

#include "Vector2.h"
#include <functional>

class Bacteria
{
    Vector2 Position;
    Vector2 Velocity;
    uint8_t Faction;
    uint8_t Health;

    void Update1(const Bacteria &currentState, Bacteria &nextState, std::function<void(std::function<void(class GameState &)>)> queueMutation) const;

    // void Update1(const Bacteria &currentState, Bacteria &nextState, std::function<void(int)> queueMutation);
};
