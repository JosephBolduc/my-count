    #include <vector>
#include <math.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>

using std::vector;

// Each subprocess has a ref to this in shared memory
// Each process canmake calls to this class which will handle shared memory and locking
class MemoryManager
{
private:
    int *srcArr;                   // ptr to the array of source values
    int *destArr;                  // ptr to the array of dest values, swaps with sourceArr every iteration
    pthread_mutex_t *counterMutex; // Mutex for the fake counting semaphore being used

    int sourceArrId;
    int destArrId;
    int finCtrId;
    int finCtrMutexId;


public:
    int arraySize;
    int coreCount;
    int totalIterations;
    int *finishedCounter; // The fake counting semaphore being used

    // Only to called by the parent process to initialize the shared memory and set its size and starting array
    MemoryManager(int coreCount, vector<int> *startingValues, int size)
    {
        arraySize = size;
        this->coreCount = coreCount;
        totalIterations = log2(arraySize);

        // A shared memory segments are allocated for the source and destination arrays and other stuff that needs to by synced
        int sourceArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
        int destArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
        int finCtrId = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);
        int finCtrMutexId = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), S_IRUSR | S_IWUSR);

        // Checking if memory was allocated correctly
        if (sourceArrId < 0 ||
            destArrId < 0 ||
            finCtrId < 0 ||
            finCtrMutexId < 0)
            throw std::exception();

        // Attaching the shared memory segments
        srcArr = (int *)shmat(sourceArrId, NULL, 0);
        destArr = (int *)shmat(destArrId, NULL, 0);
        finishedCounter = (int *)shmat(finCtrId, NULL, 0);
        counterMutex = (pthread_mutex_t *)shmat(finCtrMutexId, NULL, 0);

        // Initializing the mutex
        pthread_mutexattr_t mutexattr;
        pthread_mutexattr_init(&mutexattr);
        pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(counterMutex, &mutexattr);
        pthread_mutexattr_destroy(&mutexattr);

        // Loading starting array into place
        for (int idx = 0; idx < arraySize; idx++)
            srcArr[idx] = startingValues->at(idx);
    }

    // Reading from the source array with check for bounds
    int atSrc(int index)
    {
        if (index >= arraySize || index < 0)
            throw std::exception();
        return srcArr[index];
    }

    // Adds the given value to the number at index position w/ bounds check
    void setDest(int index, int value)
    {
        if (index >= arraySize || index < 0)
            throw std::exception();
        destArr[index] = value;
    }

    // Acts as a waiting room for the processes when finished with their assigned work for the iteration
    void Wait()
    {
        // Checks if all other processes have passed through
        pthread_mutex_lock(counterMutex);
        if(*finishedCounter == -1)
        {
            pthread_mutex_unlock(counterMutex);
            exit(0);
        }


        if (*finishedCounter == coreCount - 1)
        {
            swapArrays();
            *finishedCounter = 0;
            pthread_mutex_unlock(counterMutex);
            return;
        }

        (*finishedCounter) += 1;
        pthread_mutex_unlock(counterMutex);
        // Spinlock style wait for the other processes
        while (true)
        {
            if (*finishedCounter == -1) exit(0);        
            if (*finishedCounter == 0) return;
        }
    }

    // Swaps arrays to keep linear space complexity
    void swapArrays()
    {
        for (int i = 0; i < arraySize; i++)
            srcArr[i] = destArr[i];
    }

    void Deallocate(bool del)
    {
        shmdt(srcArr);
        shmdt(destArr);
        shmdt(counterMutex);
        shmdt(finishedCounter);
        if(!del) return;
        shmctl(sourceArrId, IPC_RMID, NULL);
        shmctl(destArrId, IPC_RMID, NULL);
        shmctl(finCtrId, IPC_RMID, NULL);
        shmctl(finCtrMutexId, IPC_RMID, NULL);
    }
};