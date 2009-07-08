#ifndef READ_DIR_H
#define READ_DIR_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsReadDirRequest;
class FSServer;

/**
 * State machine for performing directory read processing
 */
class ReadDir
{
public:
    /** Constructor */
    ReadDir(FSServer* module, spfsReadDirRequest* readDir);

    /**
     * Handle message as part of the read process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file creation message to the OS
     */
    void readDir();

    /**
     * Send the final response to the client
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating read directory request */
    spfsReadDirRequest* readDirReq_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
