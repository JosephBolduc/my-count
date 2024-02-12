#include <vector>
#include <string>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#include <iostream>

using std::string;
using std::vector;

// Each subprocess has a ref to this in shared memory
// Each process can instantiate and make calls to this class which will handle shared memory and locking
class SharedMemoryManager
{
private:
    int* sourceArr;     // ptr to the array of source values
    int* destArr;       // ptr to the array of dest values, swaps with sourceArr every iteration
    pthread_mutex_t* counterMutex; 

    

public:
    int arraySize;
    int coreCount;
    int totalIterations;
    int* finishedCounter;  // value to signal how many processess have finished, also set to negative value to signal finished to main
    pthread_mutex_t* waitingRoomMutex;     // Avoids clobbering writes to the counter in the "waiting room"

    // Only called by the parent process to initialize the shared memory and set its size
    SharedMemoryManager(int arraySize, int coreCount, vector<int>* startingValues)
    {
        this->arraySize = arraySize;
        this->coreCount = coreCount;
        totalIterations = std::log2(arraySize);
        createOrGetShared();

        for(int idx = 0; idx < arraySize; idx++)
        {
            sourceArr[idx] = startingValues->at(idx);
            destArr[idx] = startingValues->at(idx);
        }

    }


    // Attaches and creates the shared memory segments
    // Call in every new process
     void createOrGetShared()
    {
        // A shared memory segments are allocated for the source and destination arrays and other stuff that needs to by synced
        static int sourceArrId;
        static int destArrId;
        static int counterMutexId;
        static int finishedCounterId;
        static int waitingRoomMutexId;

        if(sourceArrId == 0)
        {
            // Creaes the shared memory segment for various fields, partially sourced from provided sample code
            sourceArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
            destArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
            counterMutexId = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), S_IRUSR | S_IWUSR);
            finishedCounterId = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);
            waitingRoomMutexId = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), S_IRUSR | S_IWUSR);
        
            if (sourceArrId < 0 ||
                destArrId < 0 ||
                counterMutexId < 0 ||
                finishedCounterId < 0 ||
                waitingRoomMutexId < 0) throw std::exception();
        }

        sourceArr = (int*)shmat(sourceArrId, NULL, 0);
        destArr = (int*)shmat(destArrId, NULL, 0);
        counterMutex = (pthread_mutex_t*)shmat(counterMutexId, NULL, 0);
        finishedCounter = (int*)shmat(finishedCounterId, NULL, 0);
        waitingRoomMutex = (pthread_mutex_t*)shmat(waitingRoomMutexId, NULL, 0);

        pthread_mutexattr_t mutexattr;
        pthread_mutexattr_init(&mutexattr);
        pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(waitingRoomMutex, &mutexattr);
        pthread_mutex_init(counterMutex, &mutexattr);

        pthread_mutexattr_destroy(&mutexattr);
    }

    // Reading from the source array with check for bounds
    int ReadFromSource(int index)
    {
        if(index >= arraySize) throw std::exception();
        return sourceArr[index];
    }

    // Adds the given value to the number at index position w/ bounds check
    void AddToDest(int index, int toAdd)
    {
        std::cout << "adding " + std::to_string(toAdd) + " to index " + std::to_string(index) + "\n";
        if(index >= arraySize) throw std::exception();
        destArr[index] = sourceArr[index] + toAdd;
    }

    // Acts as a waiting room for the processes when finished with their assigned work for the iteration
    void Wait()
    {
        // avoid writing over the counter
        pthread_mutex_lock(counterMutex);
        if(*finishedCounter == coreCount - 1) 
        {
            std::cout << "moving to next iteration\n";
            swapArrays();
            pthread_mutex_unlock(counterMutex);
            *finishedCounter = 0;
            return;
        }
        (*finishedCounter)++;
        pthread_mutex_unlock(counterMutex);
        

        while(*finishedCounter > 0) continue;

    }


    // Doesn't actually swap arrays anymore but copies dest into source
    void swapArrays()
    {
        std::cout << "arrays swapped!\n";
        for(int i = 0; i<arraySize; i++) sourceArr[i] = destArr[i];
    }

    ~SharedMemoryManager()
    {
        shmdt(sourceArr);
        shmdt(destArr);
        shmdt(counterMutex);
        shmdt(finishedCounter);
        shmdt(waitingRoomMutex);
    }
};