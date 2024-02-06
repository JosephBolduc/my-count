#include <vector>
#include <string>
#include <math.h>

using std::vector;
using std::string;




// Each subprocess has a ref to this in shared memory
class SharedMemoryManager
{
private:
    //queue<int> workQueue;	// maintains every 
public:
    vector<int> listA;		// The two lists to be used, so linear space usage 
    vector<int> listB;
    int AtoB = true;		// which list to write to, alternates every iteration
	int iteration = 0;
	int totalIterations;


    SharedMemoryManager(vector<int>* input)
	{
		listA = *input;
		totalIterations = std::log2(listA.size());
	}

    
    ~SharedMemoryManager()
    {
        // TODO release any shared resources
        // or do that from main
        
    }
};