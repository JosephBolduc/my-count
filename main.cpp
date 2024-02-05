#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h> 

using std::string;
using std::cout;
using std::vector;

vector<int>* parseInput(string fileName);

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

    parseInput(inputFileName);

    int* syncNumber = new int(0);

    int fCheck = fork();

    cout << "Pid of the process is " << getpid() << std::endl;
    


    cout << "hello world" << std::endl;
}

// Reads the first line from the input file and loads numbers seperated by commas
// Does not read past the first line
vector<int>* parseInput(string fileName)
{
    string inputLine;
    vector<int>* parsedContent = new vector<int>();

    std::ifstream input;
    input.open(fileName);

    getline(input, inputLine);
    if(input.is_open())
    {

        input >> inputLine;
    }
    
    string parseBuffer = "";
    for(unsigned int idx = 0; idx < inputLine.length(); idx++)
    {
        cout << inputLine[idx];
    }
    cout << "\n";
    
    return parsedContent;
}