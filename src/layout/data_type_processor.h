#ifndef REQUEST_PROCESSOR
#define REQUEST_PROCESSOR

#include <cstddef>
#include <vector>
#include "pfs_types.h"
class FileDistribution;

/** A file layout */
struct FileLayout
{
    std::vector<FSOffset> offsets;
    std::vector<FSSize> extents;
};

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
                                         FileLayout& layout);

    /** @return the number of bytes processed */
    static int createFileLayoutForServer(const FSOffset& offset,
                                         const FSDataType& dataType,
                                         const std::size_t& count,
                                         const FileDistribution& dist,
                                         const std::size_t& maxBytesToProcess,
                                         FileLayout& layout);

private:

    /** @return the number of bytes processed */
    static int processClientRequest(const FSOffset& offset,
                                    const FSDataType& dataType,
                                    const std::size_t& count,
                                    const FileDistribution& dist,
                                    const std::size_t& maxBytesToProcess,
                                    FileLayout& layout);
    
    /** @return the number of bytes processed */
    static int processServerRequest(const FSOffset& offset,
                                    const FSDataType& dataType,
                                    const std::size_t& count,
                                    const FileDistribution& dist,
                                    const std::size_t& maxBytesToProcess,
                                    FileLayout& layout);

    /** @return the number of bytes processed */
    static int processContiguousClientRegion(
        const FSOffset& offset,
        const FSSize& extent,
        const FileDistribution& dist,
        const std::size_t& maxBytesToProcess,
        FileLayout& layout);
    
    /** @return the number of bytes processed */
    static int processContiguousServerRegion(
        const FSOffset& offset,
        const FSSize& extent,
        const FileDistribution& dist,
        const std::size_t& maxBytesToProcess,
        FileLayout& layout);
    
};

#endif
