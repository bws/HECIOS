#ifndef SPFS_EXCEPTIONS_H
#define SPFS_EXCEPTIONS_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
