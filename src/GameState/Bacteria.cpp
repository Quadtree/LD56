#include "Bacteria.h"

void Bacteria::Update1(Bacteria &nextState, class MutationQueue *queueMutation) const
{
    nextState = *this;
    nextState.Position = Vector2(Position.X, Position.Y + 1);

#if _DEBUG
    nextState.NumUpdates = NumUpdates + 1;
#endif
}