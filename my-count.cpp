#include <string>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "fileParser.h"
#include "workerProcess.h"

using std::cout;
using std::string;
using std::vector;

void cleanup() {system("pkill -f my-count");}
void handler(int signo) {exit(-1);}

int main(int argc, char *argv[])
{
    atexit(cleanup);
    signal(SIGTERM, handler);

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

    // Validating arguments
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

    if(coreCount > arraySize)
    {
        cout << "Ignoring extra " << coreCount - arraySize << " cores!\n";
        coreCount = arraySize;
    }

    vector<int> *startingList = parseInput(inputFileName);

    if (startingList->size() < arraySize)
    {
        cout << "The input file does not have the given number of elements!!\n";
        exit(-1);
    }

    // Assigning work to each process by indexes of the array they will handle
    vector<vector<int>*> workAssignment;
    for(int i=0; i<coreCount; i++) workAssignment.push_back(new vector<int>());
    int currentProcess = 0;
    for(int targetIndex = 0; targetIndex < arraySize; targetIndex++)
    {
        workAssignment[currentProcess]->push_back(targetIndex);
        currentProcess++;
        if(currentProcess >= coreCount) currentProcess = 0;
    }

    // Creating the memory manager and worker processes
    MemoryManager* shared = new MemoryManager(coreCount, startingList, arraySize);
    for(int core = 0; core < coreCount; core++) if(fork() != 0 ) workerProcess(workAssignment.at(core), shared);
    
    // Spinlock style wait for the processes to finish
    while(true) if (*(shared->finishedCounter) == -1) break;
    
    // Reading the output and writing to file
    vector<int> output;
    for(int i=0; i<arraySize; i++) output.push_back(shared->atSrc(i));

    shared->Deallocate(true);

    writeOutput(outputFileName, &output);
}