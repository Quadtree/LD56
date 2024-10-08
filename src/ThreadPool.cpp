#include "LD56.h"
#include <latch>
#include <barrier>
#include "Util.h"

vector<thread> threadPool;
mutex primaryMutex;

queue<function<void()>> threadPoolWorkQueue;

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
    // return 2;

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
            workItem();
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

    if (threadPool.size() == 0)
    {
        threadPoolControllerThreadId = this_thread::get_id();
        hasDeterminedMainThreadId = true;

        numProc = max(DetermineNumberOfProcessors() - 1, 1);

        DESC_LINE(endOfOperationLatch = make_unique<barrier<>>(numProc + 1));
        DESC_LINE(startOfOperationLatch = make_unique<barrier<>>(numProc + 1));

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
    TIME_COMMAND(startOfOperationLatch->arrive_and_wait());

    TIME_COMMAND(endOfOperationLatch->arrive_and_wait());
}
