#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <mutex>

#include "fileManager.h"
#include "SharedMemoryManager.h"

using std::cout;
using std::string;
using std::vector;

// Fwd declares for file writing functions and other stuff
void workerProcessLoop(SharedMemoryManager*);

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

    writeOutput(outputFileName, startingList);

    SharedMemoryManager* shared = new SharedMemoryManager(startingList->size());

    // Creating the child processes
    for (int i = 0; i < coreCount; i++)
    {
        // The children are sent off to the workerProcessLoop mines, never to be seen again
        if (fork() == 0) workerProcessLoop(shared);
        
    }

    cout << "Pid of the process is " << getpid() << std::endl;
}

void workerProcessLoop(SharedMemoryManager* shared)
{
    // TODO add the loop for the child processes. In the meantime, suicide
    for (;;)
    {

        if (true)
        {
            shared->AddToDest(0, 1);
            cout << "ending from inside child function\n";
            delete shared;
            exit(1);
        }
    }
}
