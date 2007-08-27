#ifndef FILENAME_H
#define FILENAME_H

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

    /** Return the filename as a string */
    std::string str() const { return path_; };
    
    /** @return the number of path segments (including the root directory) */
    std::size_t getNumPathSegments() const;

    /** @return the absolute path segment */
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
