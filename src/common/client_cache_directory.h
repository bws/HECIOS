#ifndef CLIENT_CACHE_DIRECTORY_H_
#define CLIENT_CACHE_DIRECTORY_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
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
    /** Forward declaration */
    class ClientCache;
    class Entry;

    /** Enable singleton construction */
    friend class Singleton<ClientCacheDirectory>;

    /** Map of handle to client cache type */
    typedef std::multimap<Entry, ClientCache> CacheEntryToClientMap;

    /** Map of client caches to the invalid entries */
    typedef std::map<ClientCache, std::set<Entry> > InvalidationMap;

    /** Cache entry states */
    enum State {INVALID_STATE = 0, SHARED, EXCLUSIVE};

    /** Type for describing a client cache */
    struct ClientCache
    {
        int rank;
        ConnectionId connection;
    };

    /** Entries stored in client caches */
    struct Entry
    {
        Filename filename;
        FilePageId pageId;
        State state;
    };

    /**
     * Parse the request to determine the client caches in need of an
     * invalidate
     *
     * @return the set of client caches needing an invalidate
     */
    InvalidationMap getClientsNeedingInvalidate(const Filename& filename,
                                                const FSSize& pageSize,
                                                const FSOffset& offset,
                                                const FSSize& dataSize,
                                                const FileView& view,
                                                const FileDistribution& dist) const;

    /** Add a directory entry for the page */
    void addClientCacheEntry(const ClientCache& clientCache,
                             const Filename& filename,
                             const FilePageId& pageId,
                             State state);

    /** Remove the directory entry for the page */
    void removeClientCacheEntry(const ClientCache& clientCache,
                                const Filename& filename,
                                const FilePageId& pageId);

    /** Remove the directory entry for the page */
    void removeClientCacheEntryByRank(int rank,
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
inline int compare(const ClientCacheDirectory::ClientCache& lhs,
                   const ClientCacheDirectory::ClientCache& rhs)
{
    int cmpValue = 1;
    if (lhs.rank == rhs.rank)
    {
        assert(lhs.connection == rhs.connection);
        cmpValue = 0;
    }
    else if (lhs.rank < rhs.rank)
    {
        cmpValue = -1;
    }
    return cmpValue;
}

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
inline bool operator==(const ClientCacheDirectory::ClientCache& lhs,
                       const ClientCacheDirectory::ClientCache& rhs)
{
    return (0 == compare(lhs, rhs));
}

/** @return true if the two entries are equivalent */
inline bool operator<(const ClientCacheDirectory::ClientCache& lhs,
                      const ClientCacheDirectory::ClientCache& rhs)
{
    return (-1 == compare(lhs, rhs));
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
