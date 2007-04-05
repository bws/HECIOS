#include <list>
#include <vector>
#include <ext/hash_map>
#include "client_fs_state.h"
using namespace std;

ClientFSState::ClientFSState()
    : attrCache_(MAX_ATTR_ENTRIES, MAX_ATTR_TIME),
      dirCache_(MAX_ATTR_ENTRIES, MAX_ATTR_TIME)
{
    totalNumServers = 64;
    defaultNumServers = 16;
    root = 0;
}

void ClientFSState::insertAttr(FSHandle metaHandle, FSMetaData metaData)
{
    attrCache_.insert(metaHandle, metaData);
}

void ClientFSState::removeAttr(FSHandle metaHandle)
{
    attrCache_.remove(metaHandle);
}

FSMetaData ClientFSState::lookupAttr(FSHandle metaHandle)
{
    FSMetaData metaData;
    attrCache_.lookup(metaHandle);
    return metaData;
}

void ClientFSState::insertDir(std::string path, FSHandle metaHandle)
{
    dirCache_.insert(path, metaHandle);
}

void ClientFSState::removeDir(std::string path)
{
    dirCache_.remove(path);
}

FSHandle ClientFSState::lookupDir(std::string path)
{
    FSHandle metaHandle;
    dirCache_.lookup(path);
    return metaHandle;
}

/** returns true is the server has data in the given request */
bool serverNotUsed(int serverNum, int dist, int count, MPIDataType dtype)
{
    return false;
}
                                                                                
/** called during create to select servers for new file */
/** randomly selects a server from 0 to S-1 where S is totalNumServers */
int fsSelectServer()
{
    return 0;
}
                                                                                
/** hashes path to a number from 0 to S-1 where S is totalNumServers */
int fsHashPath(std::string path)
{
    return 0;
}

HandleRange ClientFSState::fsServers(int num)
{
    return handleRanges_[num];
}

int ClientFSState::fsTotalNumServers()
{
    return totalNumServers;
}

int ClientFSState::fsDefaultNumServers()
{
    return defaultNumServers;
}

int ClientFSState::fsRoot()
{
    return root;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
