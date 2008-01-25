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
    source_("<UNDEFINED>")
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
    source_("<UNDEFINED>")
{
}

IOTrace::~IOTrace()
{
}

void IOTrace::registerFile(const string& filename, size_t fileSize)
{
    fileSizesByName_[filename] = fileSize;
}

IOTrace::FileSystemMap::const_iterator IOTrace::getFiles() const
{
    return fileSizesByName_.begin();
}

void IOTrace::addFilename(int fileId, std::string filename)
{
    filenamesById_[fileId] = filename;
}
    
std::string IOTrace::getFilename(int fileId) const
{
    map<int, string>::const_iterator iter = filenamesById_.find(fileId);
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
