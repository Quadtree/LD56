#include "LD56.h"
#include <barrier>

vector<thread> threadPool;
mutex primaryMutex;

queue<function<void()>> threadPoolWorkQueue;
atomic<int> ActiveThreadPoolThreads;

void BarrierCompletionFunction()
{
}

unique_ptr<barrier<void()>> endOfOperationBarrier;

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
            endOfOperationBarrier->arrive_and_wait();
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

        endOfOperationBarrier = make_unique<barrier>(threadPool.size() + 1, &BarrierCompletionFunction);
    }

    threadPoolWorkQueue.push(func);
}

void WaitForThreadPoolToFinishAllTasks()
{
    endOfOperationBarrier->arrive_and_wait();

    // cout << "WaitForThreadPoolToFinishAllTasks completed" << endl;
}

void ExpectThreadPoolToBeEmpty()
{
    if (ActiveThreadPoolThreads != 0)
    {
        RAISE_ERROR("ActiveThreadPoolThreads should be zero");
    }

    lock_guard primaryMutexGuard(primaryMutex);

    if (threadPoolWorkQueue.size() != 0)
    {
        RAISE_ERROR("threadPoolWorkQueue.size() should be zero");
    }
}