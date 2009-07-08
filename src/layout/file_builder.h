#ifndef FILE_BUILDER_H
#define FILE_BUILDER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <map>
#include <vector>
#include "io_trace.h"
#include "pfs_types.h"
#include "singleton.h"
class FileDescriptor;
class Filename;
class StorageLayoutManagerIFace;

/** Builder functions for creating pre-existing parallel file system files */
class FileBuilder : public Singleton<FileBuilder>
{
public:
    /** Allow singleton construction */
    friend class Singleton<FileBuilder>;

    /** Default size for a PFS directory */
    static const std::size_t DEFAULT_DIRECTORY_SIZE = 8192;

    /** Default size for a PFS directory */
    static const std::size_t DEFAULT_BSTREAM_SIZE = 1073741824;

    /** Singleton accessor */
    //static FileBuilder& instance();

    /** Clear all state information */
    //static void clearState();

    /** Set the default meta data size */
    void setDefaultMetaDataSize(std::size_t metaDataSize);

    /** @return the server id for the registering server */
    int registerFSServer(const HandleRange& range, bool isMetaServer);

    /** @return true if the filename exists within the PFS */
    bool fileExists(const Filename& fileName) const;

    /** @return a list of Metadata Server numbers */
    std::vector<int> getMetaServers() const;

    /** @return the number of data servers */
    size_t getNumDataServers() const {return nextServerNumber_;};

    /** @return Metadata for a PFS file */
    FSMetaData* getMetaData(const FSHandle& handle) const;

    /** @return Metadata for a PFS file */
    FSMetaData* getMetaData(const Filename& fileName) const;

    /** @return Descriptor for a PFS file */
    FileDescriptor* getDescriptor(const Filename& fileName) const;

    /** @return the first handle for the server */
    FSHandle getFirstHandle(size_t serverNumber);

    /** @return the next handle for server */
    FSHandle getNextHandle(size_t serverNumber);

    /** Create the named directory in the file system */
    void createDirectory(const Filename& dirName,
                         int metaServer,
                         StorageLayoutManagerIFace& layoutManager);

    /** Create the named file in the file system */
    void createFile(const Filename& fileName,
                    FSSize fileSize,
                    int metaServer,
                    int numDataServers,
                    StorageLayoutManagerIFace& layoutManager);

    /** @return the number of data objects for a metadata handle */
    size_t getNumDataObjects(const FSHandle& metaHandle) const;

    /** @return the list of blocks for a file handle */
    std::vector<int> getDiskBlocks(const FSHandle& dataHandle) const;

    /** Populate the file system with the files described in the IOTrace */
    void populateFileSystem(const FileSystemMap& traceFS);

    /**
     * Populate the file system with the directories and files described
     * in the IOTrace
     */
    void populateFileSystem(const FileSystemMap& traceDirs,
                            const FileSystemMap& traceFiles);

private:
    /** Default constructor */
    FileBuilder();

    /** Destructor */
    ~FileBuilder();

    /** Disabled copy constructor */
    FileBuilder(const FileBuilder& other);

    /** Disabled assignment operator */
    FileBuilder& operator=(const FileBuilder& other);

    /** Size of metadata entries */
    std::size_t defaultMetaDataSize_;

    /** Next serer number to assign */
    std::size_t nextServerNumber_;

    std::map<std::string, FSHandle> nameToHandleMap_;

    std::map<FSHandle, FSMetaData*> handleToMetaMap_;

    std::vector<HandleRange> handlesByServer_;

    std::vector<int> metaServers_;

    std::vector<FSHandle> nextHandleByServer_;
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
