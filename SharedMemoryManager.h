#include <vector>
#include <string>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <mutex>

using std::string;
using std::mutex;

// Each subprocess has a ref to this in shared memory
// Each process can instantiate and make calls to this class which will handle shared memory and locking
class SharedMemoryManager
{
private:
    int* sourceArr;     // ptr to the array of source values
    int* destArr;       // ptr to the array of dest values, swaps with sourceArr every iteration
    mutex* mutexArr;    // array of mutexes to avoid clobbering writes
    bool* finishedArr;  // array to booleans to signal which array indices have finished, could be done by process to save memory but whatever


public:
    int arraySize;
    int iteration = 0;
    int totalIterations;

    // Only called by the parent process to initialize the shared memory and set its size
    SharedMemoryManager(int arraySize)
    {
        this->arraySize = arraySize;
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

        if(sourceArrId == 0)
        {
            // Creaes the shared memory segment for various fields, partially sourced from provided sample code
            sourceArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
            destArrId = shmget(IPC_PRIVATE, arraySize * sizeof(int), S_IRUSR | S_IWUSR);
            mutexArrId = shmget(IPC_PRIVATE, arraySize * sizeof(std::mutex), S_IRUSR | S_IWUSR);
        
            if (sourceArrId < 0 || destArrId < 0) throw std::exception();
        }

        sourceArr = (int*)shmat(sourceArrId, NULL, 0);
        destArr = (int*)shmat(destArrId, NULL, 0);
        mutexArr = (std::mutex*)shmat(mutexArrId, NULL, 0);

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
        mutexArr[index].lock();
        destArr[index] += toAdd;
        mutexArr[index].unlock();
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