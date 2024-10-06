#include "GameState.h"
#include "Bacteria.h"
#include "../LD56.h"

#define PROCESSING_BLOCK_SIZE 20
#define USE_MULTITHREADED_UPDATE 0

GameState::GameState()
{
    Reset();
}

void GameState::DoUpdate(GameState &nextGameState, TerrainType terrain[TERRAIN_GRID_SIZE * TERRAIN_GRID_SIZE])
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
        if (BacteriaList[i].Health > 0)
            MainSpatialIndex.AddToIndex(&BacteriaList[i]);
    }

    MutationQueue mutationQueue;
    MutationQueue *mutationQueuePtr = &mutationQueue;
    auto currentBacteriaList = this->BacteriaList;
    auto nextBacteriaList = nextGameState.BacteriaList;

    const GameState *curGameStatePtr;

#if USE_MULTITHREADED_UPDATE
    for (int i = 0; i < NumActiveBacteria; i += PROCESSING_BLOCK_SIZE)
    {
        auto startPos = i;
        auto endPos = min(i + PROCESSING_BLOCK_SIZE, NumActiveBacteria);

        SubmitToThreadPool([startPos, endPos, nextBacteriaList, currentBacteriaList, mutationQueuePtr, terrain, curGameStatePtr]()
                           {
            for (int j = startPos; j < endPos; ++j)
            {
                currentBacteriaList[j].Update1(nextBacteriaList[j], curGameStatePtr, mutationQueuePtr, terrain);
            } });
    }

    WaitForThreadPoolToFinishAllTasks();
#else
    nextGameState.NumActiveBacteria = NumActiveBacteria;

    nextGameState.LivingBacteriaLastFrame = 0;

    for (int i = 0; i < NumActiveBacteria; ++i)
    {
        if (currentBacteriaList[i].Health > 0)
        {
            currentBacteriaList[i].Update1(nextBacteriaList[i], this, mutationQueuePtr, terrain);
            nextGameState.LivingBacteriaLastFrame++;
        }
        else
        {
            nextBacteriaList[i] = currentBacteriaList[i];
        }
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

        if (nextGameState.BacteriaList[i].Health > 0 && actualNumUpdates > 0 && actualNumUpdates != expectedNumUpdates)
        {
            DUMP(actualNumUpdates);
            DUMP(expectedNumUpdates);
            cout << ("NumUpdates does not match!");
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

const GridCell<Bacteria> &GameState::GetBacteriaNear(Vector2 point, float radius) const
{
    auto cellId = MainSpatialIndex.Vector2ToCellID(point);

    return MainSpatialIndex.Cells[cellId];
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
            if (bacteria.Type == BacteriaType::Gobbler)
                bacteria.Health = 20;
            if (bacteria.Type == BacteriaType::Zoomer)
                bacteria.Health = 6;
            if (bacteria.Type == BacteriaType::Spitter)
                bacteria.Health = 7;

#if _DEBUG
            bacteria.NumUpdates = 0;
#endif

            bacteria.ID = i;
            bacteria.Velocity = Vector2();
            bacteria.AttackCharge = 0;
            bacteria.NumUpdates = 0;

            if (bacteria.Type == BacteriaType::Converter)
                bacteria.AttackCharge = rand() % 1000;

            BacteriaList[i] = bacteria;

            // cout << "ADDED NEW ONE AT " << i << endl;

            return;
        }
    }
}

void GameState::Reset()
{
    NumActiveBacteria = 0;
    memset(BacteriaList, 0, sizeof(BacteriaList));
    MainSpatialIndex.ClearCells();
    memset(AttractionPoints, 0, sizeof(AttractionPoints));
    WasDeath = false;
    WasAttack = false;
}

void MutationQueue::QueueMutation(int priority, std::function<void(GameState *)> mutation)
{
#if USE_MULTITHREADED_UPDATE
    lock_guard mutationQueueMutexGuard(mutationQueueMutex);
#endif
    mutationQueues[priority].push_back(mutation);
}
