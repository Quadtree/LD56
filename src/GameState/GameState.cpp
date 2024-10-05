#include "GameState.h"
#include "Bacteria.h"
#include "../LD56.h"

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

    MutationQueue mutationQueue;
    MutationQueue *mutationQueuePtr = &mutationQueue;
    auto currentBacteriaList = this->BacteriaList;
    auto nextBacteriaList = nextGameState.BacteriaList;

    for (int i = 0; i < NumActiveBacteria; i += PROCESSING_BLOCK_SIZE)
    {
        auto startPos = i;
        auto endPos = min(i + PROCESSING_BLOCK_SIZE, NumActiveBacteria);

        SubmitToThreadPool([startPos, endPos, nextBacteriaList, currentBacteriaList, mutationQueuePtr]()
                           {
            for (int j = startPos; j < endPos; ++j)
            {
                currentBacteriaList[j].Update1(nextBacteriaList[j], mutationQueuePtr);
            } });
    }

    WaitForThreadPoolToFinishAllTasks();
}