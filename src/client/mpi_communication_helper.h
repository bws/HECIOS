#ifndef MPI_COMMUNICATION_HELPER_H
#define MPI_COMMUNICATION_HELPER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <map>
#include <vector>
#include "comm_man.h"
#include "pfs_types.h"
#include "singleton.h"
class spfsMPICollectiveRequest;
class spfsMPIRequest;

/**
 * Abstract interface implemented by classes wishing to use the
 * MPICommunicationHelper
 */
class MPICommunicationUserIF
{
public:
    /** Pure virtual destructor to ensure abstract interface */
    virtual ~MPICommunicationUserIF() = 0;

    /**
     * Callback invoked when the communication this collective user
     * invokes completes
     */
    virtual void completeCommunicationCB(spfsMPIRequest* origRequest) = 0;
};

/**
 * Provides functionality so that a user can register to receive notification
 * when a communication completes
 */
class MPICommunicationHelper : public Singleton<MPICommunicationHelper>
{
    /** Enable singleton construction */
    friend class Singleton<MPICommunicationHelper>;

public:
    /** Perform the communication */
    void performCommunication(MPICommunicationUserIF* commUser,
                              spfsMPIRequest* request,
                              int otherRank);

    /** Perform the collective */
    void performCollective(MPICommunicationUserIF* commUser,
                           spfsMPICollectiveRequest* request);

protected:
    /** Constructor */
    MPICommunicationHelper();

    /** Destructor */
    ~MPICommunicationHelper();

private:
    typedef std::pair<MPICommunicationUserIF*,
                      spfsMPICollectiveRequest*> UserCallback;

    /** Map of the number of collective participants indexed by communicator */
    typedef std::map<Communicator, std::size_t> CollectiveCountMap;

    /** Map of the collective participants indexed by communicator */
    typedef std::map<Communicator,
                     std::vector<UserCallback> > CollectiveCallbackMap;

    /** Disabled copy constructor */
    MPICommunicationHelper(const MPICommunicationHelper& other);

    /** Disabled assignment operator */
    MPICommunicationHelper& operator=(const MPICommunicationHelper& other);

    /** Map of the number collective participants indexed by communicator */
    CollectiveCountMap numParticipantsByCommunicator_;

    /** Map of callbacks indexed by communicator */
    CollectiveCallbackMap callbacksByCommunicator_;
};

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
