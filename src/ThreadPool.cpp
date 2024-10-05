#include "LD56.h"
#include <latch>
#include <barrier>

vector<thread> threadPool;
mutex primaryMutex;

queue<function<void()>> threadPoolWorkQueue;
atomic<int> ActiveThreadPoolThreads;

thread::id threadPoolControllerThreadId;
bool hasDeterminedMainThreadId = false;

int numProc = -1;

void BarrierCompletionFunction()
{
}

// #define VERBOSE_LOGGING

#if VERBOSE_LOGGING
mutex msgMutex;

#define PRINT_MSG(x)                                               \
    {                                                              \
        lock_guard msgMutexGuard(msgMutex);                        \
        if (this_thread::get_id() != threadPoolControllerThreadId) \
            cout << this_thread::get_id();                         \
        else                                                       \
            cout << "MAIN";                                        \
        cout << ": " << x << endl;                                 \
    }

#else
#define PRINT_MSG(x)
#endif

#define DESC_LINE(x)                       \
    {                                      \
        PRINT_MSG(__LINE__ << ": " << #x); \
        x;                                 \
    }

unique_ptr<barrier<>> endOfOperationLatch;
unique_ptr<barrier<>> startOfOperationLatch;

int DetermineNumberOfProcessors()
{
    return 8;

    return EM_ASM_INT({
        return navigator.hardwareConcurrency;
    });
}

void ThreadPoolEntryPoint()
{
    bool hasWorkItem;
    function<void()> workItem;

    DESC_LINE(startOfOperationLatch->arrive_and_wait());

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
            DESC_LINE(endOfOperationLatch->arrive_and_wait());
            DESC_LINE(startOfOperationLatch->arrive_and_wait());
        }
    }
}

void SubmitToThreadPool(function<void()> func)
{
    lock_guard primaryMutexGuard(primaryMutex);

    if (!hasDeterminedMainThreadId)
    {
        threadPoolControllerThreadId = this_thread::get_id();
        hasDeterminedMainThreadId = true;
    }

    if (numProc == -1)
        numProc = max(DetermineNumberOfProcessors() - 1, 1);

    if (!endOfOperationLatch)
    {
        DESC_LINE(endOfOperationLatch = make_unique<barrier<>>(numProc + 1));
        DESC_LINE(startOfOperationLatch = make_unique<barrier<>>(numProc + 1));
    }

    if (threadPool.size() == 0)
    {
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
    DESC_LINE(startOfOperationLatch->arrive_and_wait());

    DESC_LINE(endOfOperationLatch->arrive_and_wait());
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