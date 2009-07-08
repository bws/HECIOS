#ifndef MOCK_STORAGE_LAYOUT_MANAGER_H
#define MOCK_STORAGE_LAYOUT_MANAGER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include "storage_layout_manager.h"
class Filename;

class MockStorageLayoutManager : public StorageLayoutManagerIFace
{
protected:
    /** Add a directory to a server's native file system */
    virtual void addDirectoryToFS(std::size_t serverNumber,
                                  const Filename& dirName) {};

    /** Add a file to a server's native file system */
    virtual void addFileToFS(std::size_t serverNumber,
                         const Filename& filename,
                         FSSize size) {};
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
