
#include "file_builder.h"
#include <cassert>
#include <cstdlib>
#include "IPvXAddress.h"
#include "filename.h"
#include "simple_stripe_distribution.h"
using namespace std;

FileBuilder* FileBuilder::instance_ = 0;

FileBuilder& FileBuilder::instance()
{
    if (0 == instance_)
        instance_ = new FileBuilder();
    return *instance_;
}

void FileBuilder::clearState()
{
    delete instance_;
    instance_ = 0;
}

FileBuilder::FileBuilder()
    : nextServerNumber_(0)
{
}

int FileBuilder::registerFSServer(const HandleRange& range, bool isMetaServer)
{
    assert(nextServerNumber_ == handlesByServer_.size());
    assert(nextServerNumber_ == nextHandleByServer_.size());
    handlesByServer_.push_back(range);
    nextHandleByServer_.push_back(range.first);
    
    if (isMetaServer)
        metaServers_.push_back(nextServerNumber_);
    
    return nextServerNumber_++;
}

bool FileBuilder::fileExists(const Filename& fileName) const
{
    std::map<std::string, FSHandle>::const_iterator pos;
    pos = nameToHandleMap_.find(fileName.str());
    return (nameToHandleMap_.end() != pos);
}

vector<int> FileBuilder::getMetaServers() const
{
    return metaServers_;
}

FSMetaData* FileBuilder::getMetaData(const Filename& fileName) const
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

FSDescriptor* FileBuilder::getDescriptor(const Filename& fileName) const
{
    return 0;
}

FSHandle FileBuilder::getFirstHandle(size_t serverNumber)
{
    assert(serverNumber < nextServerNumber_);
    return handlesByServer_[serverNumber].first;
}

FSHandle FileBuilder::getNextHandle(size_t serverNumber)
{
    assert(serverNumber < nextServerNumber_);
    return nextHandleByServer_[serverNumber]++;
}

void FileBuilder::createDirectory(const Filename& dirName, int metaServer)
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

void FileBuilder::createFile(const Filename& fileName,
                             int metaServer,
                             int numServers)
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
        meta->dist = new SimpleStripeDistribution(0, numServers);

        // Construct the data handles
        cerr << "Creating file on " << numServers << endl;
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
