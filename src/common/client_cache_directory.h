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
#include <cstddef>
#include <iostream>
#include <map>
#include <set>
#include "basic_types.h"
#include "file_page.h"
#include "filename.h"
#include "singleton.h"
class FileDistribution;
class FileView;

class ClientCacheDirectory : public Singleton<ClientCacheDirectory>
{
public:
    /** Enable singleton construction */
    friend class Singleton<ClientCacheDirectory>;

    /** Entries stored in client caches */
    struct Entry
    {
        Filename filename;
        FilePageId pageId;
    };

    /** Client cache type */
    typedef ConnectionId ClientCache;

    /** Map of handle to client cache type */
    typedef std::multimap<Entry, ClientCache> CacheEntryToClientMap;

    /**
     * Parse the request to determine the client caches in need of an
     * invalidate
     *
     * @return the set of client caches needing an invalidate
     */
    std::set<ClientCache> getClientsNeedingInvalidate(const Filename& filename,
                                                      const FSSize& pageSize,
                                                      const FSOffset& offset,
                                                      const FSSize& dataSize,
                                                      const FileView& view,
                                                      const FileDistribution& dist) const;

    /** */
    void addClientCacheEntry(const ClientCache& clientCache,
                             const Filename& filename,
                             const FilePageId& pageId);

    /** */
    void removeClientCacheEntry(const ClientCache& clientCache,
                                const Filename& filename,
                                const FilePageId& pageId);

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
    CacheEntryToClientMap clientCacheEntries_;
};

/** @return -1, 0, or 1 if lhs is less than, equal to, or greater than rhs */
inline int compare(const ClientCacheDirectory::Entry& lhs,
                   const ClientCacheDirectory::Entry& rhs)
{
    int cmpValue = 1;
    if (lhs.pageId == rhs.pageId)
    {
        if (lhs.filename == rhs.filename)
        {
            cmpValue = 0;
        }
        else if (lhs.filename < rhs.filename)
        {
            cmpValue = -1;
        }
    }
    else if (lhs.pageId < rhs.pageId)
    {
        cmpValue = -1;
    }
    return cmpValue;
}

/** @return true if the two entries are equivalent */
inline bool operator==(const ClientCacheDirectory::Entry& lhs,
                       const ClientCacheDirectory::Entry& rhs)
{
    return (0 == compare(lhs, rhs));
}

/** @return true if the two entries are equivalent */
inline bool operator<(const ClientCacheDirectory::Entry& lhs,
                      const ClientCacheDirectory::Entry& rhs)
{
    return (-1 == compare(lhs,rhs));
}

inline std::ostream& operator<<(std::ostream& ost,
                                const ClientCacheDirectory::Entry& entry)
{
    ost << entry.filename << " " << entry.pageId;
    return ost;
}

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
