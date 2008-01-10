//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#include <list>
#include <vector>
#include <ext/hash_map>
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
