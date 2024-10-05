#include "GameState.h"
#include "Bacteria.h"
#include "../LD56.h"

#define PROCESSING_BLOCK_SIZE 20
#define USE_MULTITHREADED_UPDATE 0

GameState::GameState() : NumActiveBacteria(0)
{
    NumActiveBacteria = 4000;

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        BacteriaList[i].Position = Vector2(1 * i, 0);
        BacteriaList[i].Health = 1;
        BacteriaList[i].Faction = 0;
        BacteriaList[i].Type = BacteriaType::Converter;
        BacteriaList[i].ID = i;
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

    MainSpatialIndex.ClearCells();

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        MainSpatialIndex.AddToIndex(&BacteriaList[i]);
    }

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
        currentBacteriaList[i].Update1(nextBacteriaList[i], this, mutationQueuePtr);
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

thread_local vector<const Bacteria *> bacteriaNearVector;

const Vector2 OFFSETS[] = {
    Vector2(0, 0),

    Vector2(-1, 0),
    Vector2(1, 0),
    Vector2(0, 1),
    Vector2(0, -1),

    Vector2(-1, -1),
    Vector2(1, 1),
    Vector2(-1, 1),
    Vector2(-1, -1),
};

vector<const Bacteria *> &
GameState::GetBacteriaNear(Vector2 point, float radius) const
{
    bacteriaNearVector.resize(0);

    vector<int> prevCells;

    for (int offset = 0; offset < 9; ++offset)
    {
        auto cellId = MainSpatialIndex.Vector2ToCellID(point);

        if (cellId == -1 || find(prevCells.begin(), prevCells.end(), cellId) != prevCells.end())
            continue;

        prevCells.push_back(cellId);

        for (int i = 0; i < MainSpatialIndex.Cells[cellId].NumInCell; ++i)
        {
            if (point.DistToSquared(MainSpatialIndex.Cells[cellId].List[i]->Position) <= radius)
            {
                bacteriaNearVector.push_back(MainSpatialIndex.Cells[cellId].List[i]);
            }
        }
    }

    return bacteriaNearVector;
}
