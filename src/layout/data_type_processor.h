#ifndef DATA_TYPE_PROCESSOR
#define DATA_TYPE_PROCESSOR
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

#include <cstddef>
#include <vector>
#include "pfs_types.h"
class DataTypeLayout;
class FileDistribution;

/** Data type processor to determine which file portions are alloted
 *  to each I/O node for a give MPI data type and file distribution
 *  scheme
 */
class DataTypeProcessor
{
public:

    /** @return the number of bytes processed */
    static int createFileLayoutForClient(const FSOffset& offset,
                                         const FSDataType& dataType,
                                         const std::size_t& count,
                                         const FileDistribution& dist,
                                         const std::size_t& maxBytesToProcess,
                                         DataTypeLayout& layout);

    /** @return the number of bytes processed */
    static int createFileLayoutForServer(const FSOffset& offset,
                                         const FSDataType& dataType,
                                         const std::size_t& count,
                                         const FileDistribution& dist,
                                         const std::size_t& maxBytesToProcess,
                                         DataTypeLayout& layout);

private:

    /** @return the number of bytes processed */
    static int processClientRequest(const FSOffset& offset,
                                    const FSDataType& dataType,
                                    const std::size_t& count,
                                    const FileDistribution& dist,
                                    const std::size_t& maxBytesToProcess,
                                    DataTypeLayout& layout);
    
    /** @return the number of bytes processed */
    static int processServerRequest(const FSOffset& offset,
                                    const FSDataType& dataType,
                                    const std::size_t& count,
                                    const FileDistribution& dist,
                                    const std::size_t& maxBytesToProcess,
                                    DataTypeLayout& layout);

    /** @return the number of bytes processed */
    static int processContiguousClientRegion(
        const FSOffset& offset,
        const FSSize& extent,
        const FileDistribution& dist,
        const std::size_t& maxBytesToProcess,
        DataTypeLayout& layout);
    
    /** @return the number of bytes processed */
    static int processContiguousServerRegion(
        const FSOffset& offset,
        const FSSize& extent,
        const FileDistribution& dist,
        const std::size_t& maxBytesToProcess,
        DataTypeLayout& layout);
    
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
