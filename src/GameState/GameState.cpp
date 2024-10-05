#include "GameState.h"
#include "Bacteria.h"
#include "../LD56.h"

#define PROCESSING_BLOCK_SIZE 5
#define USE_MULTITHREADED_UPDATE 1

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
#if _DEBUG
    nextGameState.BeingUpdated = true;

    if (nextGameState.BeingRendered)
        throw "Cannot also be rendered!";

        // ExpectThreadPoolToBeEmpty();
#endif

    MutationQueue mutationQueue;
    MutationQueue *mutationQueuePtr = &mutationQueue;
    auto currentBacteriaList = this->BacteriaList;
    auto nextBacteriaList = nextGameState.BacteriaList;

#if USE_MULTITHREADED_UPDATE
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
#else

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        currentBacteriaList[i].Update1(nextBacteriaList[i], mutationQueuePtr);
    }
#endif

#if _DEBUG
    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        auto actualNumUpdates = nextGameState.BacteriaList[i].NumUpdates;
        auto expectedNumUpdates = BacteriaList[i].NumUpdates + 1;

        if (actualNumUpdates != expectedNumUpdates)
        {
            DUMP(actualNumUpdates);
            DUMP(expectedNumUpdates);
            RAISE_ERROR("NumUpdates does not match!");
        }
    }

    if (nextGameState.BeingRendered)
        throw "Cannot also be rendered!";

    nextGameState.BeingUpdated = false;
#endif
}