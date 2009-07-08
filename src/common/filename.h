#ifndef FILENAME_H
#define FILENAME_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <iostream>
#include <string>
#include "basic_types.h"

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

    /** Create a filename from a unique file handle */
    explicit Filename(const FSHandle& handle);

    /** @return the filename as a string */
    std::string str() const { return path_; };

    /** @return the filename as a char* */
    const char* c_str() const { return path_.c_str(); };

    /** @return the number of path segments (including the root directory) */
    std::size_t getNumPathSegments() const;

    /**
     * @return a filename containing the absolute path up to segment segNum
     */
    Filename getSegment(std::size_t segNum) const;

    /**
     * @return the filename of the parent directory.  Signal an error if this
     *  is the root directory (/)
     */
    Filename getParent() const;

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

/** @return true if lhs is aphabetically < rhs */
inline bool operator<(const Filename& lhs, const Filename& rhs)
{
    return (lhs.str() < rhs.str());
}

/** @return stream containing representation of the filename */
inline std::ostream& operator<<(std::ostream& ost, const Filename& f)
{
    return ost << f.str();
}

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
