#ifndef DATA_TYPE_PROCESSOR
#define DATA_TYPE_PROCESSOR
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <vector>
#include "file_region_set.h"
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
     * @return the number of bytes assigned to the distribution
     * @side fills the outAggregateSize with the total I/O size
     */
    static FSSize createClientFileLayoutForRead(const FSOffset& offset,
                                                const DataType& dataType,
                                                const std::size_t& count,
                                                const FileView& view,
                                                const FileDistribution& dist,
                                                const FSSize& bstreamSize,
                                                FSSize& outAggregateSize);

    /**
     * @return the number of bytes assigned to the distribution
     * @side fills the outAggregateSize with the total I/O size
     */
    static FSSize createClientFileLayoutForWrite(const FSOffset& offset,
                                                 const DataType& dataType,
                                                 const std::size_t& count,
                                                 const FileView& view,
                                                 const FileDistribution& dist,
                                                 FSSize& outAggregateSize);

    /**
     * @return the number of bytes assigned to the distribution
     * @side Fills the layout object with physical offsets and extents for
     * this server's I/O
     */
    static FSSize createServerFileLayoutForRead(const FSOffset& offset,
                                                const FSSize& dataSize,
                                                const FileView& view,
                                                const FileDistribution& dist,
                                                const FSSize& bstreamSize,
                                                DataTypeLayout& outLayout);

    /**
     * @return the number of bytes assigned to the distribution
     * @side Fills the layout object with physical offsets and extents for
     * this server's I/O
     */
    static FSSize createServerFileLayoutForWrite(const FSOffset& offset,
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

    /**
     * @return the file regions for the first requestSize bytes of the
     *   supplied MPI view.  This step 'flattens' the view into list I/O
     *   offsets and extents.
     */
    static FileRegionSet locateFileRegionSet(const FSOffset& offset,
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
                                       const FSSize& bstreamSize,
                                       bool dataExtend,
                                       FSSize& outAggregateSize);

    /** @return the number of bytes processed
     * @side Fills the layout object with physical offsets and extents for
     * this server's I/O
     */
    static FSSize processServerRequest(const FSOffset& offset,
                                       const FSSize& dataSize,
                                       const FileView& view,
                                       const FileDistribution& dist,
                                       const FSSize& bstreamSize,
                                       bool dataExtend,
                                       DataTypeLayout& outLayout);

    /** Construct the server local offset extent pairs */
    static void distributeContiguousRegion(const FSOffset& offset,
                                           const FSSize& extent,
                                           const FileDistribution& dist,
                                           const FSSize& bstreamSize,
                                           bool dataExtend,
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
