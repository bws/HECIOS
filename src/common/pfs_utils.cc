
#include "pfs_utils.h"
using namespace std;

PFSUtils* PFSUtils::instance_ = 0;

PFSUtils& PFSUtils::instance()
{
    if (0 == instance_)
        instance_ = new PFSUtils();
    return *instance_;
}

PFSUtils::PFSUtils()
{
}

void PFSUtils::registerServerIP(const IPvXAddress& ip, HandleRange range)
{
    map< HandleRange , IPvXAddress >::const_iterator itr = handleIPMap.find(range);
    
    // If duplicate exists, erase the element
    if ( itr != handleIPMap.end() && itr->second != ip)
    {
        handleIPMap.erase(range);
    }

    // Add ip
    handleIPMap[range] = ip;
}

    
IPvXAddress PFSUtils::getServerIP(const FSHandle& handle) const
{
    // Create a HandleRange whose first and last elements are equal to the
    // parameter handle
    HandleRange newRange;
    newRange.first = handle;
    newRange.last  = handle;
    
    // If the handle lies outside handle-ranges stored in map, return "0.0.0.0"
    IPvXAddress ipaddr("0.0.0.0");

    // Find the first element in map whose handle-range is lesser than newRange
    map< HandleRange , IPvXAddress >::const_iterator itr_lower =
        handleIPMap.lower_bound(newRange);
    if(itr_lower != handleIPMap.end())
    {
        if( handle >= itr_lower->first.first &&
            handle <= itr_lower->first.last)
        {
            return itr_lower->second;
        }
    }

    return ipaddr;
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
