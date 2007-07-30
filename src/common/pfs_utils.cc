
#include "pfs_utils.h"
#include <cassert>
#include <cstdlib>
#include "IPvXAddress.h"
#include "pfs_types.h"
using namespace std;

PFSUtils* PFSUtils::instance_ = 0;

PFSUtils& PFSUtils::instance()
{
    if (0 == instance_)
        instance_ = new PFSUtils();
    return *instance_;
}

void PFSUtils::clearState()
{
    delete instance_;
    instance_ = 0;
}

PFSUtils::PFSUtils()
{
}

void PFSUtils::registerServerIP(IPvXAddress* ip, HandleRange range)
{
    map<HandleRange,IPvXAddress*>::const_iterator itr =
        handleIPMap_.find(range);
    
    // If duplicate exists, erase the element
    if (itr != handleIPMap_.end() && itr->second != ip)
    {
        handleIPMap_.erase(range);
    }

    // Add ip
    handleIPMap_[range] = ip;
}

IPvXAddress* PFSUtils::getServerIP(const FSHandle& handle) const
{
    // Create a HandleRange whose first and last elements are equal to the
    // handle
    HandleRange newRange;
    newRange.first = handle;
    newRange.last  = handle;
    
    // If the handle lies outside handle-ranges of map, return null ip
    IPvXAddress* ip = 0;

    // Find the first element in map whose handle-range is lesser than newRange
    map<HandleRange,IPvXAddress*>::const_iterator itr_lower =
        handleIPMap_.lower_bound(newRange);
    if (itr_lower != handleIPMap_.end())
    {
        if (handle >= itr_lower->first.first &&
            handle <= itr_lower->first.last)
        {
            ip = itr_lower->second;
        }
    }

    return ip;
}

void PFSUtils::registerRankIP(int rank, IPvXAddress* ip)
{
    ipsByRank_[rank] = ip;
}

IPvXAddress* PFSUtils::getRankIP(int rank) const
{
    IPvXAddress* addr = 0;
    map<int, IPvXAddress*>::const_iterator iter = ipsByRank_.find(rank);
    if (iter != ipsByRank_.end())
    {
        addr = iter->second;
    }
    return addr;
}

std::vector<IPvXAddress*> PFSUtils::getAllRankIP() const
{
    vector<IPvXAddress*> addrs;
    map<int, IPvXAddress*>::const_iterator iter;
    for (iter = ipsByRank_.begin(); iter != ipsByRank_.end(); ++iter)
    {
        addrs.push_back(iter->second);
    }
    return addrs;
}

void PFSUtils::parsePath(FSOpenFile* descriptor) const
{
    int size, seg;
    std::string::size_type index;
    size = descriptor->path.size();
    index = 0;
    for (seg = 0; index != std::string::npos && seg < MAXSEG; seg++)
    {
        index = descriptor->path.find_first_not_of('/', index);
        descriptor->segstart[seg] = index;
        descriptor->seglen[seg] = size - index;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
