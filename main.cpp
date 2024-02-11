#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <mutex>
#include <math.h>

#include "fileManager.h"
#include "SharedMemoryManager.h"

using std::cout;
using std::string;
using std::vector;

// Fwd declares for file writing functions and other stuff
void workerProcessLoop(SharedMemoryManager* shared, vector<int>* work);

int main(int argc, char *argv[])
{
    // Checking if the correct amount of arguments was given
    if (argc != 5)
    {
        cout << "Enter four arguments: # of array elements, # of cores, input file, and output file!\n";
        return -1;
    }

    // This style of inputs is probably at least a little vulnurable to buffer overflows so please don't hurt me
    int arraySize = std::stoi(argv[1]);
    int coreCount = std::stoi(argv[2]);
    string inputFileName = argv[3];
    string outputFileName = argv[4];

    if (arraySize < 1)
    {
        cout << "Enter a number of array elements of at least 1\n";
        exit(-1);
    }

    if (coreCount < 1)
    {
        cout << "Enter a core count of at least 1\n";
        exit(-1);
    }

    vector<int> *startingList = parseInput(inputFileName);

    if (startingList->size() != arraySize)
    {
        cout << "The given array size does not match the input file!!\n";
        exit(-1);
    }

    SharedMemoryManager* shared = new SharedMemoryManager(startingList->size());

    // Assigning work to each process by indexes of the array they will handle
    vector<vector<int>*> workAssignment;
    for(int i=0; i<coreCount; i++) workAssignment.push_back(new vector<int>());
    int currentProcess = 0;
    for(int targetIndex = 0; targetIndex < startingList->size(); targetIndex++)
    {
        workAssignment[currentProcess]->push_back(targetIndex);
        currentProcess++;
        if(currentProcess >= coreCount) currentProcess = 0;
    }

    // Creating the child processes
    for (int core = 0; core < coreCount; core++)
    {
        // The children are sent off to the workerProcessLoop mines, never to be seen again
        if (fork() == 0) 
        {
            workerProcessLoop(shared, workAssignment[core]);
        }
    }


    vector<int> output;
    for(int idx = 0; idx < arraySize; idx++)
    {
        output.push_back(shared->ReadFromSource(idx));
    }

    cout << "Pid of the process is " << getpid() << std::endl;
}

void workerProcessLoop(SharedMemoryManager* shared, vector<int>* workAssigned)
{
    string asdna = "work assigned: ";
    for(int i=0; i<workAssigned->size(); i++) {asdna += std::to_string(workAssigned->at(i)) + " ";}
    cout << asdna << std::endl;

    // TODO add the loop for the child processes. In the meantime, suicide
    // Tracks each iteration on its own, synchonizing through the shared object
    for (int iteration = 0; iteration <= shared->totalIterations; iteration++)
    {
        // Does work for each array index assigned
        for(int workIdx = 0; workIdx < workAssigned->size(); workIdx++)
        {
            int arrIdx = workAssigned->at(workIdx);
            if(arrIdx >= std::exp2(iteration))
            {
                int toAdd = shared->ReadFromSource(arrIdx - std::exp2(iteration));
                shared->AddToDest(arrIdx, toAdd);
            }





        }



        if (true)
        {
            cout << "ending from inside child function\n";
            delete shared;
            exit(0);
        }
    }
}
