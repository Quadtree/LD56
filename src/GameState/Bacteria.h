#pragma once

#include "Vector2.h"

class Bacteria
{
    Vector2 Position;
    Vector2 Velocity;
    uint8_t Faction;
    uint8_t Health;

    void Update1(const Bacteria &currentState, Bacteria &nextState, function<void(function < void(GameState &))> queueMutation);
};
