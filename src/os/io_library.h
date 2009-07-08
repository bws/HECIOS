#ifndef IO_LIBRARY_H
#define IO_LIBRARY_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
class NativeFileSystem;

/**
 * Abstract base class for I/O library models
 */
class IOLibrary : public cSimpleModule
{
public:

protected:

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage(cMessage *msg);

private:

    /** in gate connection id */
    int inGateId_;

    /** out gate connection id */
    int outGateId_;

    /** request gate connection id */
    int requestGateId_;
};

/**
 *
 */
class ListIOLibrary : public IOLibrary
{
};

/**
 * I/O library model that passes requests through with no translation
 */
class PassThroughIOLibrary : public IOLibrary
{
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
