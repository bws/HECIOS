#ifndef MPI_COMMUNICATION_HELPER_H
#define MPI_COMMUNICATION_HELPER_H
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
    /** Disabled copy constructor */
    MPICommunicationHelper(const MPICommunicationHelper& other);

    /** Disabled assignment operator */
    MPICommunicationHelper& operator=(const MPICommunicationHelper& other);
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
