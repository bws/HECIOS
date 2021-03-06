#ifndef CLIENT_FS_UTILS_H
#define CLIENT_FS_UTILS_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <string>
#include <vector>
#include <omnetpp.h>
#include "lru_timeout_cache.h"
#include "mpi_proto_m.h"
#include "pfs_types.h"

/**
 * Singleton object for accessing client-side file system utility
 * functions
 */
class ClientFSState
{
public:

    /** Attribute Cache Type */
    typedef LRUTimeoutCache<FSHandle, FSMetaData> AttributeCache;

    /** Attribute Cache Entry Type */
    typedef AttributeCache::EntryType AttributeEntry;

    /** Name Cache Type */
    typedef LRUTimeoutCache<std::string, FSHandle> NameCache;

    /** Name Cache Entry Type */
    typedef NameCache::EntryType NameEntry;

    /** The maximum number of entries in the attribute cache */
    static const int MAX_ATTR_ENTRIES = 100;

    /** The maximum time an entry may reside in the attribute cache */
    static const double MAX_ATTR_TIME = 100.0;

    /** The maximum number of entries in the name cache */
    static const int MAX_NAME_ENTRIES = 100;

    /** The maximum time an entry may reside in the name cache */
    static const double MAX_NAME_TIME = 100.0;

    /** Default constructor */
    ClientFSState();

    /** Add an entry to the attribute cache */
    void insertAttr(FSHandle metaHandle, FSMetaData metaData);

    /** Remove an entry from the attribute cache */
    void removeAttr(FSHandle metaHandle);

    /** @return the meta data for the handle */
    FSMetaData* lookupAttr(FSHandle metaHandle);

    /** Add an entry to the name cache */
    void insertName(const std::string& path, FSHandle metaHandle);

    /** Remove an entry from the directory cache */
    void removeName(const std::string& path);

    /** @return the meta data handle for the directory */
    FSHandle* lookupName(const std::string& path);

    /**
     * Perform a lookup on name and fill out the number of resolved filename
     * segments and the resolved handle (null if no item was portion of the
     * name was resolved.
     *
     * @return the lookup status for the cache of name
     */
    FSLookupStatus lookupName(const Filename& name,
                              size_t& outNumResolvedSegs,
                              FSHandle* outResolvedHandle);

    /** I have no idea what this function is for (BWS) */
    bool serverNotUsed(int serverNum, int dist, int count, MPIDataType dtype);

    /** called during create to select servers for new file */
    /** randomly selects a server from 0 to S-1 where S is totalNumServers */
    int selectServer();

    /** hashes path to a number from 0 to S-1 where S is totalNumServers */
    int hashPath(std::string path);

    /** access function for HandleRange_ vector */
    HandleRange servers(int num);

    /** access function for defaultNumServers */
    int defaultNumServers();

private:
    /** Copy constructor disabled */
    ClientFSState(const ClientFSState& orig);

    /** access function for root */
    int root();

    /** access function for totalNumServers */
    int totalNumServers();

    /** Attribute cache */
    AttributeCache attrCache_;

    /** Name cache */
    NameCache nameCache_;

    /** Server handle ranges */
    std::vector<HandleRange> handleRanges_;

    /** number of servers in file system */
    int totalNumServers_;

    /** default number of servers for 1 file */
    int defaultNumServers_;

    /** server with root directory */
    int root_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
