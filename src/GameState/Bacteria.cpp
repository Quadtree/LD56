#include "Bacteria.h"

void Bacteria::Update1(Bacteria &nextState, class MutationQueueTarget *queueMutation) const
{
    nextState = *this;
    nextState.Position = Vector2(Position.X, Position.Y + 1);
}