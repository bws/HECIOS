//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "client_fs_state.h"
using namespace std;

ClientFSState::ClientFSState()
    : attrCache_(MAX_ATTR_ENTRIES, MAX_ATTR_TIME),
      nameCache_(MAX_NAME_ENTRIES, MAX_NAME_TIME),
      totalNumServers_(2),
      defaultNumServers_(2),
      root_(0)
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

FSMetaData* ClientFSState::lookupAttr(FSHandle metaHandle)
{
    FSMetaData* metaData = 0;
    AttributeEntry* entry = attrCache_.lookup(metaHandle);
    if (0 != entry)
    {
        // Need to check expiration
        metaData = &(entry->data);
    }
    return metaData;
}

void ClientFSState::insertName(const string& path, FSHandle metaHandle)
{
    nameCache_.insert(path, metaHandle);
}

void ClientFSState::removeName(const string& path)
{
    nameCache_.remove(path);
}

FSHandle* ClientFSState::lookupName(const string& path)
{
    FSHandle* metaHandle = 0;
    NameEntry* entry = nameCache_.lookup(path);
    if (0 != entry)
    {
        // Need to check expiration
        metaHandle = &(entry->data);
    }
    return metaHandle;
}

FSLookupStatus ClientFSState::lookupName(const Filename& name,
                                         size_t& outNumResolvedSeg,
                                         FSHandle* outResolvedHandle)
{
    FSLookupStatus lookupStatus = SPFS_NOTFOUND;
    for (size_t i = name.getNumPathSegments() - 1; i != size_t(-1); i--)
    {
        FSHandle* handle = lookupName(name.getSegment(i).str());
        if (0 != handle)
        {
            outNumResolvedSeg = i + 1;
            *outResolvedHandle = *handle;
            lookupStatus = (i == name.getNumPathSegments() - 1) ? SPFS_FOUND : SPFS_PARTIAL;
            break;
        }
    }
    return lookupStatus;
}

/** returns true is the server has data in the given request */
bool ClientFSState::serverNotUsed(int serverNum, int dist,
                                  int count, MPIDataType dtype)
{
    return false;
}

/**
 * called during create to select servers for new file
 * randomly selects a server from 0 to S-1 where S is totalNumServers
 */
int ClientFSState::selectServer()
{
    return 0;
}

/** hashes path to a number from 0 to S-1 where S is totalNumServers */
int ClientFSState::hashPath(std::string path)
{
    return 0;
}

HandleRange ClientFSState::servers(int num)
{
    return handleRanges_[num];
}

int ClientFSState::root()
{
    return root_;
}

int ClientFSState::totalNumServers()
{
    return totalNumServers_;
}

int ClientFSState::defaultNumServers()
{
    return defaultNumServers_;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
