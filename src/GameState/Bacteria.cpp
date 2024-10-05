#include "Bacteria.h"

void Bacteria::Update1(Bacteria &nextState, std::function<void(std::function<void(class GameState &)>)> &queueMutation) const
{
    nextState = *this;
    nextState.Position = Vector2(Position.X, Position.Y + 1);
}