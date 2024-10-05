#include "LD56.h"

vector<thread> threadPool;
mutex primaryMutex;

queue<function<void()>> threadPoolWorkQueue;
volatile atomic<int> ActiveThreadPoolThreads;

int DetermineNumberOfProcessors()
{
    return EM_ASM_INT({
        return navigator.hardwareConcurrency;
    });
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
                workItem = threadPoolWorkQueue.front();
                threadPoolWorkQueue.pop();
                hasWorkItem = true;
            }
        }

        if (hasWorkItem)
        {
            ActiveThreadPoolThreads++;
            workItem();
            ActiveThreadPoolThreads--;
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
        auto numProc = max(DetermineNumberOfProcessors() - 1, 1);

        cout << "Initializing threadpool to size " << numProc << endl;

        for (auto i = 0; i < numProc; ++i)
        {
            threadPool.push_back(thread(ThreadPoolEntryPoint));
        }
    }

    threadPoolWorkQueue.push(func);
}

void WaitForThreadPoolToFinishAllTasks()
{
    while (true)
    {
        if (ActiveThreadPoolThreads <= 0)
        {
            lock_guard primaryMutexGuard(primaryMutex);

            if (threadPoolWorkQueue.size() <= 0)
                return;
        }

        SDL_Delay(0);
    }
}