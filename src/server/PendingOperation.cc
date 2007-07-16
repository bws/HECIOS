#include <list>
#include <string.h>
using namespace std;

class PendingOperation
{

public:
    bool isPending;
    bool isComplete;
    std::string op;

    PendingOperation(bool isPending, bool isComplete, std::string op);
    // ~PendingOperation();
};

PendingOperation::PendingOperation(bool isPending, bool isComplete, std::string op) // Constructor
{
   isPending = isPending;
   isComplete = isComplete;
   op = op;
}



