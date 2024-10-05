#include "GameState.h"
#include "Bacteria.h"

GameState::GameState() : NumActiveBacteria(0)
{
}

void GameState::DoUpdate(GameState &nextGameState)
{
    // @TODO: Multithread me

    std::function<void(std::function<void(class GameState &)>)> queueMutation;

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        BacteriaList[i].Update1(nextGameState.BacteriaList[i], queueMutation);
    }
}