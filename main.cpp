#include <iostream>
#include <string>
#include <vector>
#include <unistd.h> 
#include <mutex>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fileManager.h>
#include <SharedMemoryManager.h>

using std::string;
using std::cout;
using std::vector;

// Fwd declares for file writing functions and other stuff
vector<int>* parseInput(string fileName);
void writeOutput(string fileName, vector<int>* data);

void workerProcessLoop();

int main(int argc, char* argv[])
{
    // Checking if the correct amount of arguments was given
    if(argc != 5) 
    {
        cout << "Enter four arguments: # of array elements, # of cores, input file, and output file!\n";
        return -1;
    }


    int arraySize = std::stoi(argv[1]);
    int coreCount = std::stoi(argv[2]);
    string inputFileName = argv[3];
    string outputFileName = argv[4];

    if(arraySize < 1)
    {
        cout << "Enter a number of array elements of at least 1\n";
        return -1;
    }

    if(coreCount < 1)
    {
        cout << "Enter a core count of at least 1\n";
        return -1;
    }

    auto startingList = parseInput(inputFileName);

    if(startingList->size() != arraySize)
    {
        cout << "The given array size does not match the input file!!\n";
        exit(-1);
    }

	// Initialize via shared memory
	SharedMemoryManager shared(startingList);
	

    writeOutput(outputFileName, startingList);

    key_t shmKey = ftok("countShm", 41);
    int shmId = shmget(shmKey, 1024, 0666 | IPC_CREAT);
    


    // Creating the child processes
    for (int i = 0; i < coreCount; i++)
    {
        // The children are sent off to the workerProcessLoop mines, never to be seen again
        if(fork() == 0) workerProcessLoop();
    }

    cout << "Pid of the process is " << getpid() << std::endl;
}


void workerProcessLoop()
{
    //TODO add the loop for the child processes. In the meantime, suicide
    cout << "ending from inside child function\n";
    exit(1);
}