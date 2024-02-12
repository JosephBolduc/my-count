#include <vector>
#include <math.h>
#include <stdlib.h>
#include "MemoryManager.h"

using std::vector;

// What the child processes are doing
// Basically mirrors the psudocode definition of the algorithm from Wikipedia because MemoryManager.h does most of the heavy lifting
// Takes a vector of array indicies to work on and a pointer to the shared memory object from main
void workerProcess(const vector<int> *workAssignment, MemoryManager *shared)
{
    // Each processes tracks the iteration on its own
    for (int iteration = 0; iteration <= shared->totalIterations; iteration++)
    {
        int twoI = exp2(iteration);
        // Loops through all the work assigned, done at least partially in parallel
        for (int workIdx = 0; workIdx < workAssignment->size(); workIdx++)
        {
            int arrIdx = workAssignment->at(workIdx);
            if (arrIdx < twoI)
                shared->setDest(arrIdx, shared->atSrc(arrIdx));
            else
                shared->setDest(arrIdx, shared->atSrc(arrIdx) + shared->atSrc(arrIdx - twoI));
        }
        shared->Wait();
    }

    // Signals to main that it's done
    *(shared->finishedCounter) = -1;
    shared->Deallocate(false);
    exit(0);
}