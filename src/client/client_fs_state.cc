#include <list>
#include <vector>
#include <ext/hash_map>
#include "client_fs_state.h"
using namespace std;

ClientFSState::ClientFSState()
    : attrCache_(MAX_ATTR_ENTRIES, MAX_ATTR_TIME),
      dirCache_(MAX_ATTR_ENTRIES, MAX_ATTR_TIME)
{
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

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
