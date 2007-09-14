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

#include "filename.h"
#include <cassert>
#include <sstream>
using namespace std;

Filename::Filename(const string& absolutePath)
{
    assert('/' == absolutePath[0]);
    initialize(absolutePath);
}

Filename::Filename(const char* absolutePathStr)
{
    assert('/' == absolutePathStr[0]);
    string absolutePath(absolutePathStr);
    initialize(absolutePath);
}

Filename::Filename(const FSHandle& handle)
{
    // Prepend "/" to the handles string representation to create a filename
    ostringstream s;
    s << "/" << handle;
    initialize(s.str());
}

size_t Filename::getNumPathSegments() const
{
    size_t numSegs = 1;

    if (1 != path_.size())
    {
        // Add segments if the path is more than just the root directory
        //  (be sure and skip trailing slashes)
        bool foundSlash = true;
        for (size_t i = 1; i < path_.size(); i++)
        {
            if (foundSlash && '/' != path_[i])
            {
                foundSlash = false;
                numSegs++;
            }
            else if ('/' == path_[i])
            {
                foundSlash = true;
            }
        }
    }
    return numSegs;
}

Filename Filename::getSegment(size_t segIdx) const
{
    string segment = path_;
    if (0 == segIdx)
        segment = path_.substr(0, 1);
    else
    {
        size_t segCnt = 1;
        bool foundSlash = true;
        for (size_t i = 1; i < path_.size(); i++)
        {
            if ((segCnt - 1) == segIdx &&
                '/' == path_[i])
            {
                segment = path_.substr(0, i);
                break;
            }
            else if (foundSlash && '/' != path_[i])
            {
                foundSlash = false;
                ++segCnt;
            }
            else if ('/' == path_[i])
            {
                foundSlash = true;
            }
        }
    }
    return Filename(segment);
}

void Filename::initialize(const string& absolutePath)
{
    // Remove extra slashes from the path (incl. trailing slashes)
    bool foundSlash = false;
    path_.reserve(absolutePath.size());
    for (string::size_type i = 0; i < absolutePath.size(); i++)
    {
        if (!foundSlash && '/' != absolutePath[i])
        {
            path_.push_back(absolutePath[i]);
        }
        else if (foundSlash && '/' != absolutePath[i])
        {
            foundSlash = false;
            path_.push_back('/');
            path_.push_back(absolutePath[i]);
        }
        else if ('/' == absolutePath[i])
        {
            foundSlash = true;
        }
    }

    // If the path was only slashes (i.e. the root dir), set it by hand
    if (0 == path_.size())
        path_.push_back('/');
}
