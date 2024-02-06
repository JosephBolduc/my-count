#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h> 
#include <mutex>
#include <sys/ipc.h>
#include <sys/shm.h>

using std::string;
using std::cout;
using std::vector;

vector<int>* parseInput(string fileName);
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

    
    

    parseInput(inputFileName);

    key_t shmKey = ftok("countShm", 41);
    int shmId = shmget(shmKey, 1024, 0666 | IPC_CREAT);


    // Creating the child processes
    for (int i = 0; i < coreCount; i++)
    {
        if(fork() == 0) workerProcessLoop();
    }
    



    int fCheck = fork();

    cout << "Pid of the process is " << getpid() << std::endl;
    


}

// Reads the first line from the input file and loads numbers seperated by commas
// Does not read past the first line
vector<int>* parseInput(string fileName)
{
    string inputLine;
    vector<int>* parsedContent = new vector<int>();

    std::ifstream input;
    input.open(fileName);

    if(input.is_open()) getline(input, inputLine);
    
    
    string parseBuffer = "";
    for(size_t idx = 0; idx < inputLine.length(); idx++)
    {
        char current = inputLine[idx];
        if(current == ' ' || current == ',')
        {
            try 
            { 
                if(parseBuffer != "") parsedContent->push_back(std::stoi(parseBuffer)); 
            }
            catch(const std::exception& e)
            {
                std::cerr << "Invalid character found. Seperate numbers using only spaces or commas!\n";
            }
            parseBuffer = "";
        }
        else parseBuffer += current;
    }
    

    for (auto i = 0; i<parsedContent->size();i++)
    {
        int c = parsedContent->at(i);
        cout << c << std::endl;
    }
    
    
    return parsedContent;
}

void workerProcessLoop()
{

}