#ifndef CLIENT_CACHE_DIRECTORY_H_
#define CLIENT_CACHE_DIRECTORY_H_

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
#include <map>
#include <set>
#include "basic_types.h"
#include "singleton.h"

class ClientCacheDirectory : public Singleton<ClientCacheDirectory>
{
public:
    /** Enable singleton construction */
    friend class Singleton<ClientCacheDirectory>;

    /** Client cache type */
    typedef ConnectionId ClientCache;

    /** Map of handle to client cache type */
    typedef std::multimap<FSHandle, ClientCache> HandleToClientConnectionMap;

    std::set<ClientCache> getClientsNeedingInvalidate(const FSHandle& handle) const;

    void removeClient(const FSHandle& handle, const ConnectionId& connId);

    void addClient(const FSHandle& handle, const ConnectionId& connId);

private:
    /** Private constructor */
    ClientCacheDirectory();

    /** Private destructor */
    ~ClientCacheDirectory();

    /** Hidden copy constructor */
    ClientCacheDirectory(const ClientCacheDirectory& other);

    /** Hidden assignment operator */
    ClientCacheDirectory& operator=(const ClientCacheDirectory& other);

    /** Map of handles to client caches */
    HandleToClientConnectionMap handleToClientCache_;
};


#endif /* CLIENT_CACHE_DIRECTORY_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
