
#include "pfs_utils.h"
#include <cassert>
#include <cstdlib>
#include "IPvXAddress.h"
#include "filename.h"
using namespace std;

PFSUtils* PFSUtils::instance_ = 0;

PFSUtils& PFSUtils::instance()
{
    if (0 == instance_)
        instance_ = new PFSUtils();
    return *instance_;
}

void PFSUtils::clearState()
{
    delete instance_;
    instance_ = 0;
}

PFSUtils::PFSUtils()
    : nextServerNumber_(0)
{
}

void PFSUtils::registerServerIP(IPvXAddress* ip, HandleRange range)
{
    map<HandleRange,IPvXAddress*>::const_iterator itr =
        handleIPMap_.find(range);
    
    // If duplicate exists, erase the element
    if (itr != handleIPMap_.end() && itr->second != ip)
    {
        handleIPMap_.erase(range);
    }

    // Add ip
    handleIPMap_[range] = ip;
}

IPvXAddress* PFSUtils::getServerIP(const FSHandle& handle) 
{
    // Create a HandleRange whose first and last elements are equal to the
    // handle
    HandleRange newRange;
    newRange.first = handle;
    newRange.last  = handle;
    
    // If the handle lies outside handle-ranges of map, return null ip
    IPvXAddress* ip = 0;

    // Find the first element in map whose handle-range is lesser than newRange
    map<HandleRange,IPvXAddress*>::const_iterator itr_lower =
        handleIPMap_.lower_bound(newRange);
    if (itr_lower != handleIPMap_.end())
    {
        if (handle >= itr_lower->first.first &&
            handle <= itr_lower->first.last)
        {
            ip = itr_lower->second;
        }
    }
    return ip;
}

void PFSUtils::parsePath(FSOpenFile* descriptor) const
{
    int size, seg;
    std::string::size_type index;
    size = descriptor->path.size();
    index = 0;
    for (seg = 0; index != std::string::npos && seg < MAXSEG; seg++)
    {
        index = descriptor->path.find_first_not_of('/', index);
        descriptor->segstart[seg] = index;
        descriptor->seglen[seg] = size - index;
    }
}

vector<int> PFSUtils::getMetaServers() const
{
    return metaServers_;
}

bool PFSUtils::fileExists(const Filename& fileName) const
{
    std::map<std::string, FSHandle>::const_iterator pos;
    pos = nameToHandleMap_.find(fileName.str());
    return (nameToHandleMap_.end() != pos);
}

FSMetaData* PFSUtils::getMetaData(const Filename& fileName) const
{
    FSMetaData* md = 0;
    std::map<std::string, FSHandle>::const_iterator p1;
    p1 = nameToHandleMap_.find(fileName.str());
    if (nameToHandleMap_.end() != p1)
    {
        std::map<FSHandle, FSMetaData*>::const_iterator p2;
        p2 = handleToMetaMap_.find(p1->second);
        md = p2->second;
    }
    
    return md;
}

FSOpenFile* PFSUtils::getDescriptor(const Filename& fileName) const
{
    return 0;
}

FSHandle PFSUtils::getFirstHandle(size_t serverNumber)
{
    assert(serverNumber < nextServerNumber_);
    return handlesByServer_[serverNumber].first;
}

FSHandle PFSUtils::getNextHandle(size_t serverNumber)
{
    assert(serverNumber < nextServerNumber_);
    return nextHandleByServer_[serverNumber]++;
}

int PFSUtils::registerFSServer(const HandleRange& range, bool isMetaServer)
{
    assert(nextServerNumber_ == handlesByServer_.size());
    assert(nextServerNumber_ == nextHandleByServer_.size());
    handlesByServer_.push_back(range);
    nextHandleByServer_.push_back(range.first);
    
    if (isMetaServer)
        metaServers_.push_back(nextServerNumber_);
    
    return nextServerNumber_++;
}

void PFSUtils::createDirectory(const Filename& dirName, int metaServer)
{
    if (!fileExists(dirName))
    {
        // Create implied parent directories recursively
        size_t numSegs = dirName.getNumPathSegments();
        if (1 < numSegs)
            createDirectory(dirName.getSegment(numSegs - 2), metaServer);

        // Create the MetaData for the directory
        FSMetaData* meta = new FSMetaData();
        meta->mode = 777;
        meta->owner = 0;
        meta->group = 0;
        meta->nlinks = 0;
        meta->size = 0;
        meta->handle = getNextHandle(metaServer);
        meta->dist = 0;

        // Record bookkeeping information
        nameToHandleMap_[dirName.str()] = meta->handle;
        handleToMetaMap_[meta->handle] = meta;
    }
}

void PFSUtils::createFile(const Filename& fileName, int metaServer, int numServers)
{
    if (!fileExists(fileName))
    {
        // Create implied parent directories recursively
        size_t numSegs = fileName.getNumPathSegments();
        if (1 < numSegs)
            createDirectory(fileName.getSegment(numSegs - 2), metaServer);
        
        // Create the MetaData for the file
        FSMetaData* meta = new FSMetaData();
        meta->mode = 777;
        meta->owner = 0;
        meta->group = 0;
        meta->nlinks = 0;
        meta->size = 0;
        meta->handle = getNextHandle(metaServer);
        meta->dist = 0;

        // Construct the data handles
        int firstServer = rand() % nextServerNumber_;
        vector<FSHandle> dataHandles;
        for (size_t i = 0; i < nextServerNumber_; i++)
        {
            int serverNum = (firstServer + i) % nextServerNumber_;
            dataHandles.push_back(getNextHandle(serverNum));
        }
        meta->dataHandles = dataHandles;

        // Record bookeeping information
        nameToHandleMap_[fileName.str()] = meta->handle;
        handleToMetaMap_[meta->handle] = meta;
    }
    else
    {
        cerr << "Cannot create " << fileName << ": file exists" << endl;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
