
#include "pfs_utils.h"

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
}

    
IPvXAddress PFSUtils::getServerIP(const FSHandle& handle) const
{
    IPvXAddress addr("192.168.0.3");
    return addr;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
