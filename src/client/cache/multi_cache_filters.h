#ifndef MULTI_CACHE_FILTERS_H_
#define MULTI_CACHE_FILTERS_H_

//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
