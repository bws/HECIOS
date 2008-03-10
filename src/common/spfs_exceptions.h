#ifndef SPFS_EXCEPTIONS_H
#define SPFS_EXCEPTIONS_H
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

#include <stdexcept>

/**
 * Exception raised when an invalid key is used to perform a lookup into
 * a key-value container such as a cache.
 */
class NoSuchEntry : public std::runtime_error
{
public:

    /** Default constructor */
    NoSuchEntry() : std::runtime_error("") {};
};

/**
 * Exception raised when the trace file cannot be opened
 */
class NoSuchTraceFile : public std::runtime_error
{
public:

    /** Default constructor */
    NoSuchTraceFile(std::string filename) : std::runtime_error(filename) {};
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
