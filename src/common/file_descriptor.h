#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "basic_types.h"
#include "comm_man.h"
#include "file_view.h"
#include "filename.h"
#include "pfs_types.h"
class FileView;

/**
 * A parallel file system file descriptor
 */
class FileDescriptor
{
public:
    /**
     * Constructor - the default view is set to simply MPI_BYTE, the file
     * pointer is set to 0
     */
    FileDescriptor(const Filename& name, const FSMetaData& metaData);

    /** Destructor */
    ~FileDescriptor();

    /** @return the communicator used during file open */
    Communicator getCommunicator() const { return communicator_; };

    /** @return the file's name */
    Filename getFilename() const { return filename_; };

    /** @return the current position of the file pointer */
    FSOffset getFilePointer() const { return filePtr_; };

    /** @return the file view set for this file */
    FileView getFileView() const { return fileView_; };

    /** @return the meta data handle for this file */
    FSHandle getHandle() const { return metaData_.handle; };

    /** @return the descriptor's metadata */
    const FSMetaData* getMetaData() const { return &metaData_; };

    /** @return the number of parent handles resolved */
    std::size_t getNumParentHandles() const;

    /** @return the resolved handle for the filename path segment */
    FSHandle getParentHandle(std::size_t segmentIdx) const;

    /** Set the open communicator */
    void setCommunicator(const Communicator& communicator);

    /** Set the file view */
    void setFileView(const FileView& fileView);

    /** Set the file pointer to a value */
    void setFilePointer(FSOffset filePtr);

    /** Add inc to the file pointer */
    void moveFilePointer(FSOffset inc);

    /** @return true if the file descriptors are equivalent */
    bool operator==(const FileDescriptor& other) const;

private:
    /** Copy constructor hidden */
    FileDescriptor(const FileDescriptor& other);

    /** Assignement operator hidden */
    FileDescriptor& operator=(const FileDescriptor& other);

    /** Communicator used to open file */
    Communicator communicator_;

    /** File name */
    Filename filename_;

    /** File meta data */
    FSMetaData metaData_;

    /** File view */
    FileView fileView_;

    /** File pointer */
    FSOffset filePtr_;

    /** Handles for the parent directories */
    std::vector<FSHandle> parentHandles_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
