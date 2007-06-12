
#include "io_trace.h"
#include <cassert>
#include "pfs_types.h"
using namespace std;

void IOTrace::setDescriptor(int fileId, FSOpenFile* descriptor)
{
    descriptorById_[fileId] = descriptor;
}

FSOpenFile* IOTrace::getDescriptor(int fileId) const
{
    FSOpenFile* descriptor = 0;
    map<int, FSOpenFile*>::const_iterator iter = descriptorById_.find(fileId);
    if (iter != descriptorById_.end())
    {
        descriptor = iter->second;
    }
    return descriptor;
}
