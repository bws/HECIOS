
#include "filename.h"
#include <cassert>
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
