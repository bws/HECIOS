#ifndef SPFS_EXCEPTIONS_H
#define SPFS_EXCEPTIONS_H

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

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
