#include "GameState.h"
#include "Bacteria.h"

#define PROCESSING_BLOCK_SIZE 5

GameState::GameState() : NumActiveBacteria(0)
{
    NumActiveBacteria = 100;

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        BacteriaList[i].Position = Vector2(10 * i, 20);
        BacteriaList[i].Health = 1;
        BacteriaList[i].Faction = 0;
    }
}

void GameState::DoUpdate(GameState &nextGameState)
{
    // @TODO: Multithread me

    std::function<void(std::function<void(class GameState &)>)> queueMutation;

    for (int i = 0; i < NumActiveBacteria; i += PROCESSING_BLOCK_SIZE)
    {
        SubmitToThreadPool([]() {

        });

        BacteriaList[i].Update1(nextGameState.BacteriaList[i], queueMutation);
    }
}