#include <string>
#include <vector>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sys/shm.h>
#include <sys/stat.h>
#include <atomic>

using std::cout;
using std::string;
using std::vector;
using std::atomic;

vector<int>* parseInput(const string& fileName);
void writeOutput(const string& fileName, vector<int>* dataList);
void workerProcess(vector<int> workAssignment);
void allocateSharedMemory(int arraySize);

// Pointers to things in shared memory
int shmId;
void* baseShmPtr;
int* arrayAPtr;
int* arrayBPtr;
atomic<int>* counterPtr;

// Other misc information that the subprocesses need
int totalIterations;
int coreCount;

int main(int argc, char *argv[])
{
    // Checking if the correct amount of arguments was given
    if (argc != 5)
    {
        cout << "Enter four arguments: # of array elements, # of cores, input file, and output file!\n";
        return -1;
    }

    // Reading the cmd inputs
    int arraySize = std::stoi(argv[1]);
    coreCount = std::stoi(argv[2]);
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
    vector<vector<int>> workAssignment;
    for(int i=0; i<coreCount; i++) workAssignment.push_back(vector<int>());
    int currentProcess = 0;
    for(int targetIndex = 0; targetIndex < arraySize; targetIndex++)
    {
        workAssignment[currentProcess].push_back(targetIndex);
        currentProcess++;
        if(currentProcess >= coreCount) currentProcess = 0;
    }

    allocateSharedMemory(arraySize);
    totalIterations = log2(arraySize);
    for (int i = 0; i < arraySize; i++) arrayAPtr[i] = startingList->at(i);

    // Creating the worker processes
    for(int core = 0; core < coreCount; core++) if(fork() == 0 ) workerProcess(workAssignment.at(core));
    
    // Spinlock style wait for the processes to finish
    while(true) if(counterPtr->load() == coreCount * (totalIterations + 1)) break;
    
    // Reading the output and writing to file
    vector<int> output;
    int* finishedArray;
    if(totalIterations % 2 == 1) finishedArray = arrayAPtr;
    else finishedArray = arrayBPtr;
    for(int idx = 0; idx < arraySize; idx++) output.push_back(finishedArray[idx]);

    writeOutput(outputFileName, &output);

    shmdt(baseShmPtr);
    shmctl(shmId, IPC_RMID, nullptr);
}

// Reads all values from the given filename, seperated by line
vector<int>* parseInput(const string& fileName)
{
    string inputLine;
    vector<int>* parsedContent = new vector<int>();

    std::ifstream input;
    input.open(fileName);

    if(input.is_open()) {
        while (getline(input, inputLine)){
            try
            {
                if(!inputLine.empty()) parsedContent->push_back(std::stoi(inputLine));
            }
            catch(const std::exception& e)
            {
                std::cerr << "Invalid character found. Separate numbers by line!\n";
            }
        }
    }
    input.close();
    return parsedContent;
}

// Writes the content of the final array to file
void writeOutput(const string& fileName, vector<int>* dataList)
{
    string out = "";
    for (int data : *dataList)
    {
        out += std::to_string(data) + "\n";
    }
    std::ofstream output;
    output.open(fileName);
    output << out;
    output.close();
}

// Gets a shared memory segment and gets pointers to where the arrays and counter goes
void allocateSharedMemory(int arraySize)
{
    int sizeOfShm = (2 * arraySize + 1);
    shmId = shmget(IPC_PRIVATE, sizeOfShm * sizeof(int), S_IRUSR | S_IWUSR);
    baseShmPtr = shmat(shmId, nullptr, 0);
    arrayAPtr = static_cast<int *>(baseShmPtr);
    arrayBPtr = static_cast<int *>(baseShmPtr + arraySize * sizeof(int));
    counterPtr = static_cast<atomic<int> *>(baseShmPtr + 2 * (arraySize) * sizeof(int) + 1);
}

// Called by the child processes to do the calculations in parallel
void workerProcess(const vector<int> workAssignment)
{
    // Each processes tracks the iteration on its own
    for (int iteration = 0; iteration <= totalIterations; iteration++)
    {
        int twoI = exp2(iteration);
        bool AtoB = iteration % 2 == 0;
        // Loops through all the work assigned, done at least partially in parallel
        for (int arrIdx : workAssignment)
        {
            if(AtoB)
            {
                if(arrIdx < twoI) arrayBPtr[arrIdx] = arrayAPtr[arrIdx];
                else arrayBPtr[arrIdx] = arrayAPtr[arrIdx] + arrayAPtr[arrIdx - twoI];
            }
            else
            {
                if(arrIdx < twoI) arrayAPtr[arrIdx] = arrayBPtr[arrIdx];
                else arrayAPtr[arrIdx] = arrayBPtr[arrIdx] + arrayBPtr[arrIdx - twoI];
            }
        }

        counterPtr->fetch_add(1);
        while(true) if(counterPtr->load() >= (iteration + 1) * coreCount) break;
    }
    exit(0);
}

