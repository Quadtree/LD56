#include "GameState.h"

GameState::GameState() : NumActiveBacteria(0)
{
}

void GameState::DoUpdate(GameState &nextGameState)
{
    // @TODO: Multithread me

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        Bacteria[i].Update1(nextGameState.Bacteria[i]);
    }
}