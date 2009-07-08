//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
// Copyright (C) 2008 Yang Wu
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "io_trace.h"
#include <cassert>
using namespace std;

IOTrace::Record::Record(IOTrace::Operation opType,
                        int fileId,
                        std::size_t offset,
                        std::size_t length) :
    opType_(opType),
    timeStamp_(0.0),
    duration_(0.0),
    filename_(),
    fileId_(fileId),
    offset_(offset),
    length_(length),
    source_("<UNDEFINED>"),
    isCreate_(false),
    isExclusive_(false),
    isReadOnly_(false),
    isWriteOnly_(false),
    isReadWrite_(false),
    isDeleteOnClose_(false),
    isAppend_(false),
    fileExists_(true)
{
}

IOTrace::Record::Record(IOTrace::Operation opType,
                        double timeStamp,
                        double duration) :
    opType_(opType),
    timeStamp_(timeStamp),
    duration_(duration),
    filename_("<UNDEFINED>"),
    fileId_(-1),
    offset_(0),
    length_(0),
    source_("<UNDEFINED>"),
    isCreate_(false),
    isExclusive_(false),
    isReadOnly_(false),
    isWriteOnly_(false),
    isReadWrite_(false),
    isDeleteOnClose_(false),
    isAppend_(false),
    fileExists_(true)
{
}

IOTrace::~IOTrace()
{
}

void IOTrace::registerDirectory(const string& dirname, size_t numEntries)
{
    entriesByDirectory_[dirname] = numEntries;
}

void IOTrace::registerFile(const string& filename, size_t fileSize)
{
    fileSizesByName_[filename] = fileSize;
}

const FileSystemMap* IOTrace::getDirectories() const
{
    return &entriesByDirectory_;
}

const FileSystemMap* IOTrace::getFiles() const
{
    return &fileSizesByName_;
}

void IOTrace::addFilename(int fileId, std::string filename)
{
    filenamesById_[fileId] = filename;
}

std::string IOTrace::getFilename(int fileId) const
{
    map<int, string>::const_iterator iter = filenamesById_.find(fileId);
    assert(filenamesById_.end() != iter);
    return iter->second;
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
