#include "GameState.h"
#include "Bacteria.h"
#include "../LD56.h"

#define PROCESSING_BLOCK_SIZE 20
#define USE_MULTITHREADED_UPDATE 0

GameState::GameState() : NumActiveBacteria(0)
{

    // NumActiveBacteria = 4000;

    // for (int i = 0; i < NumActiveBacteria; ++i)
    // {
    //     BacteriaList[i].Position = Vector2(1 * i, 0);
    //     BacteriaList[i].Health = 1;
    //     BacteriaList[i].Faction = 0;
    //     BacteriaList[i].Type = BacteriaType::Converter;
    //     BacteriaList[i].ID = i;
    // }
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
    nextGameState.NumActiveBacteria = NumActiveBacteria;

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        currentBacteriaList[i].Update1(nextBacteriaList[i], this, mutationQueuePtr);
    }

    for (int i = 0; i < MUTATION_QUEUE_PRIORITY_LEVELS; ++i)
    {
        for (auto &it : mutationQueue.mutationQueues[i])
        {
            it(&nextGameState);
        }
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

    int prevCells[9];
    int numPrevCells = 0;

    for (int offset = 0; offset < 9; ++offset)
    {
        auto effPt = point + OFFSETS[offset] * radius;

        auto cellId = MainSpatialIndex.Vector2ToCellID(effPt);

        if (cellId == -1)
            continue;

        bool isDuplicate = false;

        for (int j = 0; j < numPrevCells; ++j)
        {
            if (prevCells[j] == cellId)
            {
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate)
            continue;

        prevCells[numPrevCells++] = cellId;

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

void GameState::AddBacteria(Bacteria bacteria)
{
    for (int i = 0; i < MAX_BACTERIA; ++i)
    {
        if (BacteriaList[i].Health <= 0 || i >= NumActiveBacteria)
        {
            NumActiveBacteria = max(NumActiveBacteria, i + 1);

            if (bacteria.Type == BacteriaType::Converter)
                bacteria.Health = 3;
            if (bacteria.Type == BacteriaType::Swarmer)
                bacteria.Health = 10;

            bacteria.ID = i;

            BacteriaList[i] = bacteria;
            return;
        }
    }
}

void MutationQueue::QueueMutation(int priority, std::function<void(GameState *)> mutation)
{
#if USE_MULTITHREADED_UPDATE
#error This will not work!
#endif
    mutationQueues[priority].push_back(mutation);
}
