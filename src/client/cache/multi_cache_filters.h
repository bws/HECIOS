#ifndef MULTI_CACHE_FILTERS_H_
#define MULTI_CACHE_FILTERS_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "multi_cache.h"

/** Functor for finding all pages for a cached file */
class FilePageFilter : public MultiCache::FilterFunctor
{
public:
    /** Constructor */
    FilePageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file */
    virtual bool filter(const MultiCache::Key& key,
                        const MultiCache::Page& pageId,
                        bool isDirty) const
    {
        return (key.filename == filename_);
    }

private:
    Filename filename_;
};

/** Functor for finding dirty pages for a cached file */
class DirtyPageFilter : public MultiCache::FilterFunctor
{
public:
    /** Constructor */
    DirtyPageFilter(const Filename& filename) : filename_(filename) {};

    /** @return true if the page belongs to this file and is dirty */
    virtual bool filter(const MultiCache::Key& key,
                        const MultiCache::Page& value,
                        bool isDirty) const
    {
        return ((key.filename == filename_) && isDirty);
    }

private:
    Filename filename_;
};

#endif /* MULTI_CACHE_FILTERS_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
