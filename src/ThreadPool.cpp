#include "LD56.h"
#include <latch>

vector<thread> threadPool;
mutex primaryMutex;

queue<function<void()>> threadPoolWorkQueue;
atomic<int> ActiveThreadPoolThreads;

void BarrierCompletionFunction()
{
}

unique_ptr<latch> endOfOperationLatch;
unique_ptr<latch> startOfOperationLatch;

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
            {
                lock_guard primaryMutexGuard(primaryMutex);
                cout << this_thread::get_id() << " - " << endOfOperationLatch << ": endOfOperationLatch->arrive_and_wait()" << endl;
            }
            endOfOperationLatch->arrive_and_wait();
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

    if (!endOfOperationLatch)
    {
        {
            // lock_guard primaryMutexGuard(primaryMutex);
            cout << this_thread::get_id() << " - " << ": Creating new latch" << endl;
        }
        endOfOperationLatch = make_unique<latch>(threadPool.size() + 1);
    }

    threadPoolWorkQueue.push(func);
}

void WaitForThreadPoolToFinishAllTasks()
{
    {
        lock_guard primaryMutexGuard(primaryMutex);
        cout << this_thread::get_id() << " (main): endOfOperationLatch->arrive_and_wait()" << endl;
    }

    endOfOperationLatch->arrive_and_wait();
    endOfOperationLatch = nullptr;

    {
        // lock_guard primaryMutexGuard(primaryMutex);
        cout << this_thread::get_id() << " - " << ": Deleted old latch" << endl;
    }

    cout << "WaitForThreadPoolToFinishAllTasks completed" << endl;
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