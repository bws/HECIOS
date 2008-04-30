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

#include "pfs_utils.h"
#include <cassert>
#include <cstdlib>
#include "IPvXAddress.h"
#include "pfs_types.h"
using namespace std;

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

void PFSUtils::registerRankConnectionDescriptor(int rank,
                                                const ConnectionDescriptor& cd)
{
    connectionsByRank_[rank] = cd;
}

PFSUtils::ConnectionDescriptor
PFSUtils::getRankConnectionDescriptor(int rank) const
{
    ConnectionDescriptor cp(0,0);
    map<int, ConnectionDescriptor>::const_iterator iter =
        connectionsByRank_.find(rank);
    assert(iter != connectionsByRank_.end());
    return iter->second;
}

IPvXAddress* PFSUtils::getRankIP(int rank) const
{
    IPvXAddress* addr = 0;
    map<int, ConnectionDescriptor>::const_iterator iter =
        connectionsByRank_.find(rank);
    if (iter != connectionsByRank_.end())
    {
        addr = iter->second.first;
    }
    return addr;
}

std::vector<IPvXAddress*> PFSUtils::getAllRankIP() const
{
    vector<IPvXAddress*> addrs;
    map<int, ConnectionDescriptor>::const_iterator iter, end;
    end = connectionsByRank_.end();
    for (iter = connectionsByRank_.begin(); iter != end; ++iter)
    {
        addrs.push_back(iter->second.first);
    }
    return addrs;
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
