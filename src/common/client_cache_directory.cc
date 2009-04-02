//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
#include "client_cache_directory.h"
using namespace std;

ClientCacheDirectory::ClientCacheDirectory()
{

}

ClientCacheDirectory::~ClientCacheDirectory()
{

}

set<ClientCacheDirectory::ClientCache>
ClientCacheDirectory::getClientsNeedingInvalidate(const FSHandle& handle) const
{
    pair<HandleToClientConnectionMap::const_iterator,
         HandleToClientConnectionMap::const_iterator> range;
    range = handleToClientCache_.equal_range(handle);

    set<ClientCache> clients;
    HandleToClientConnectionMap::const_iterator iter;
    for (iter = range.first; iter != range.second; ++iter)
    {
        clients.insert(iter->second);
    }
    return clients;
}

void ClientCacheDirectory::removeClient(const FSHandle& handle, const ClientCache& client)
{
    pair<HandleToClientConnectionMap::iterator,
         HandleToClientConnectionMap::iterator> range;
    range = handleToClientCache_.equal_range(handle);

    HandleToClientConnectionMap::iterator iter = range.first;
    while (iter != range.second)
    {
        if (client == iter->second)
        {
            handleToClientCache_.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

void ClientCacheDirectory::addClient(const FSHandle& handle, const ClientCache& client)
{
    handleToClientCache_.insert(make_pair(handle, client));
}


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
