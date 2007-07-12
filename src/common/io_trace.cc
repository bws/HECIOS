
#include "io_trace.h"
#include <cassert>
using namespace std;

void IOTrace::addFilename(int fileId, std::string filename)
{
    filenamesById_[fileId] = filename;
}
    
std::string IOTrace::getFilename(int fileId) const
{
    map<int, string>::const_iterator iter = filenamesById_.find(fileId);
    return iter->second;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
