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
class DataType;
class DataTypeLayout;
class FileDistribution;
class FileView;

/** Data type processor to determine which file portions are alloted
 *  to each I/O node for a give MPI data type and file distribution
 *  scheme
 */
class DataTypeProcessor
{
public:

    /**
     * @return the number of bytes processed
     * @side fills the outAggregateSize with the total I/O size
     */
    static FSSize createFileLayoutForClient(const FSOffset& offset,
                                            const DataType& dataType,
                                            const std::size_t& count,
                                            const FileView& view,
                                            const FileDistribution& dist,
                                            FSSize& outAggregateSize);

    /**
     * @return the number of bytes processed
     * @side Fills the layout object with physical offsets and extents for
     * this server's I/O
     */
    static FSSize createFileLayoutForServer(const FSOffset& offset,
                                            const FSSize& dataSize,
                                            const FileView& view,
                                            const FileDistribution& dist,
                                            DataTypeLayout& outLayout);
    
    /** 
     * @return the file regions for the first requestSize bytes of the
     *   supplied MPI view.  This step 'flattens' the view into list I/O
     *   offsets and extents. 
     */
    static std::vector<FileRegion> locateFileRegions(const FSOffset& offset,
                                                     const FSSize& dataSize,
                                                     const FileView& view);

private:

    /**
     * @return the number of bytes processed
     * @side fills the outAggregateSize with the total I/O size
     */
    static FSSize processClientRequest(const FSOffset& offset,
                                       const DataType& dataType,
                                       const std::size_t& count,
                                       const FileView& view,
                                       const FileDistribution& dist,
                                       FSSize& outAggregateSize);
    
    /** @return the number of bytes processed
     * @side Fills the layout object with physical offsets and extents for
     * this server's I/O
     */
    static FSSize processServerRequest(const FSOffset& offset,
                                       const FSSize& dataSize,
                                       const FileView& view,
                                       const FileDistribution& dist,
                                       DataTypeLayout& outLayout);

    /** Construct the server local offset extent pairs */
    static void distributeContiguousRegion(const FSOffset& offset,
                                           const FSSize& extent,
                                           const FileDistribution& dist,
                                           DataTypeLayout& outLayout);    
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
