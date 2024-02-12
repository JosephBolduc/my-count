#include <vector>
#include <string>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#include <iostream>

using std::string;

// Each subprocess has a ref to this in shared memory
// Each process can instantiate and make calls to this class which will handle shared memory and locking
class SharedMemoryManager
{
private:
    int* sourceArr;     // ptr to the array of source values
    int* destArr;       // ptr to the array of dest values, swaps with sourceArr every iteration
    pthread_mutex_t* mutexArr;    // array of mutexes to avoid clobbering writes

    bool* finishedArr;  // array to booleans to signal which processes have finished, has one extra slot at the end for signaling
    pthread_mutex_t* waitingRoomMutex;     // Synchronizes the processes waiting in the "waiting room"


public:
    int arraySize;
    int coreCount;
    int totalIterations;

    // Only called by the parent process to initialize the shared memory and set its size
    SharedMemoryManager(int arraySize, int coreCount)
    {
        this->arraySize = arraySize;
        this->coreCount = coreCount;
        totalIterations = std::log2(arraySize);
        createOrGetShared();
    }


    // Attaches and creates the shared memory segments
    // Call in every new process
     void createOrGetShared()
    {
        // A shared memory segments are allocated for the source and destination arrays and other stuff that needs to by synced
        static int sourceArrId;
        static int destArrId;
        static int mutexArrId;
        static int finishedArrId;
        static int waitingRoomMutexId;

        if(sourceArrId == 0)
        {
            // Creaes the shared memory segment for various fields, partially sourced from provided sample code
            sourceArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
            destArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
            mutexArrId = shmget(IPC_PRIVATE, arraySize * sizeof(pthread_mutex_t), S_IRUSR | S_IWUSR);
            finishedArrId = shmget(IPC_PRIVATE, (coreCount + 1) * sizeof(bool), S_IRUSR | S_IWUSR);
            waitingRoomMutexId = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), S_IRUSR | S_IWUSR);
        
            if (sourceArrId < 0 ||
                destArrId < 0 ||
                mutexArrId < 0 ||
                finishedArrId < 0 ||
                waitingRoomMutexId < 0) throw std::exception();
        }

        sourceArr = (int*)shmat(sourceArrId, NULL, 0);
        destArr = (int*)shmat(destArrId, NULL, 0);
        mutexArr = (pthread_mutex_t*)shmat(mutexArrId, NULL, 0);
        finishedArr = (bool*)shmat(destArrId, NULL, 0);
        waitingRoomMutex = static_cast<pthread_mutex_t*>( shmat(destArrId, NULL, 0));

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(waitingRoomMutex, &attr);
        for(int idx = 0; idx < arraySize; idx++) pthread_mutex_init(&mutexArr[idx], &attr);

        
    }

    // Reading from the source array with check for bounds
    int ReadFromSource(int index)
    {
        if(index >= arraySize) throw std::exception();
        return sourceArr[index];
    }

    // Adds the given value to the number at index position while handling locking
    void AddToDest(int index, int toAdd)
    {
        if(index >= arraySize) throw std::exception();
        pthread_mutex_lock(&mutexArr[index]);
        destArr[index] += toAdd;
        pthread_mutex_unlock(&mutexArr[index]);
    }

    // Method to handle writing from finished array from outside
    // Not going to bother with mutexes because each process should only be writing to its own slot
    void MarkFinished(int processSlot)
    {
        if (processSlot >= coreCount) throw std::exception();
        finishedArr[processSlot] = true;
    }

    // Acts as a waiting room for the processes when finished with their assigned work for the iteration
    // The one that finished first is responsible for checking on the rest while the others wait on the signal to go in a spinlock style wait
    void Wait()
    {
        int lockStatus = pthread_mutex_trylock(waitingRoomMutex);
        std::cout << "lock status" + std::to_string(lockStatus) + "\n";
        if(lockStatus)
        {
            std::cout << "locking sync mut!!\n";
            finishedArr[coreCount] = false;
            bool allFinished;
            do
            {
                allFinished = true;
                for (int idx = 0; idx < coreCount; idx++) allFinished = allFinished && finishedArr[idx];
            }
            while (!allFinished);

            // Every process should be done by here
            for (int idx = 0; idx < coreCount; idx++) finishedArr[idx] = false;
            swapArrays();
            finishedArr[coreCount] = true;
            pthread_mutex_unlock(waitingRoomMutex);
        }

        else
        {
            while(true) for(int idx = 0; idx < coreCount; idx++) if(finishedArr[coreCount]) return;
        }

    }


    // Swaps source and dest arrays, allowing for linear space complexity
    void swapArrays()
    {
        int* temp = sourceArr;
        sourceArr = destArr;
        destArr = sourceArr;
    }

    ~SharedMemoryManager()
    {
        shmdt(sourceArr);
        shmdt(destArr);
        shmdt(mutexArr);
    }
};