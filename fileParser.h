#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::cout;
using std::string;
using std::vector;

// Reads all values from the given filename, seperated by line
vector<int>* parseInput(string fileName)
{
    string inputLine;
    vector<int>* parsedContent = new vector<int>();

    std::ifstream input;
    input.open(fileName);

    if(input.is_open()) {
        while (getline(input, inputLine)){
            try
            {
                if(inputLine != "") parsedContent->push_back(std::stoi(inputLine));
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
void writeOutput(string fileName, vector<int>* dataList)
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