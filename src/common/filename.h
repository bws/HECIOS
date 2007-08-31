#ifndef FILENAME_H
#define FILENAME_H
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
#include <iostream>
#include <string>

/**
 * An absolute file or directory name with all extraneous data (slashes)
 * removed
 */
class Filename
{
public:
    
    /** Create a filename from an absolute path */
    explicit Filename(const std::string& absolutePath);

    /** Create a filename from an absolute path */
    explicit Filename(const char* absolutePathStr);

    /** @return the filename as a string */
    std::string str() const { return path_; };
    
    /** @return the number of path segments (including the root directory) */
    std::size_t getNumPathSegments() const;

    /**
     * @return a filename containing the absolute path up to segment segNum
     */
    Filename getSegment(std::size_t segNum) const;
    
private:

    /** Perform initialization tasks */
    void initialize(const std::string& absolutePath);
    
    /** Path data */
    std::string path_;
};

/** @return true if 2 filenames resolve to the same file */
inline bool operator==(const Filename& lhs, const Filename& rhs)
{
    return (lhs.str() == rhs.str());
}

/** @return stream containing representation of the filename */
inline std::ostream& operator<<(std::ostream& ost, const Filename& f)
{
    return ost << f.str();
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
