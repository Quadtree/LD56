#include "LD56.h"

vector<thread> threadPool;
mutex primaryMutex;

queue<function<void()>> threadPoolWorkQueue;

int DetermineNumberOfProcessors()
{
    return 16;
}

void ThreadPoolEntryPoint()
{
    bool hasWorkItem;
    function<void()> workItem;

    while (true)
    {
        hasWorkItem = false;

        {
            lock_guard primaryMutexGuard(primaryMutex);

            if (threadPoolWorkQueue.size() > 0)
            {
                workItem = move(threadPoolWorkQueue.pop());
                hasWorkItem = true;
            }
        }

        if (hasWorkItem)
        {
            workItem();
        }
        else
        {
            SDL_Delay(1);
        }
    }
}

void SubmitToThreadPool(function<void()> func)
{
    lock_guard primaryMutexGuard(primaryMutex);

    if (threadPool.size() == 0)
    {
        auto numProc = DetermineNumberOfProcessors();

        cout << "Initializing threadpool to size " << numProc << endl;

        for (auto i = 0; i < numProc; ++i)
        {
            threadPool.push_back(thread(ThreadPoolEntryPoint));
        }
    }
}