#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::cout;
using std::string;
using std::vector;

// Reads the first line from the input file and loads numbers seperated by commas
// Does not read past the first line
vector<int>* parseInput(string fileName)
{
    string inputLine;
    vector<int>* parsedContent = new vector<int>();

    std::ifstream input;
    input.open(fileName);

    if(input.is_open()) getline(input, inputLine);
    input.close();
    inputLine += ",";

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

    /*
    for (auto i = 0; i<parsedContent->size();i++)
    {
        int c = parsedContent->at(i);
        cout << c << std::endl;
    }
    */
    
    return parsedContent;
}

// Writes the content of the final vector to disk on a single line
// Overwrites the output file
void writeOutput(string fileName, vector<int>* dataList)
{
    string out = "";
    for (int data : *dataList)
    {
        out += std::to_string(data) + ", ";
    }
    std::ofstream output;
    output.open(fileName);
    output << out;
    output.close();

}