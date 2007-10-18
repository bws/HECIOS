#ifndef C_SIMPLE_MODULE_TESTER_H
#define C_SIMPLE_MODULE_TESTER_H
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
#include <cstddef>
#include <string>
#include <vector>
#include <omnetpp.h>

/** Used to deliver a test message to a module and then capture the
 * response messages.  The responses are captured by connecting
 * the cSimpleModuleTester's gates to every gate on the module to test
 * and receiving the message responses
 */
class cSimpleModuleTester : public cModule
{
public:

    /** Create the tester for module */
    cSimpleModuleTester(const char* moduleTypeName,
                        const char* nedFile,
                        bool autoInitialize = true);

    /** Destructor */
    ~cSimpleModuleTester();

    /** Initialize the module */
    void callInitialize();
    
    /** Deliver a message to the module */
    void deliverMessage(cMessage* msg, const char* inGateName);

    /** Deliver a message to the module */
    void deliverMessage(cMessage* msg, cGate* inGate);

    /** @return the module being tested */
    cSimpleModule* getModule() const { return module_; };
    
    /**
     * @return the number of output messages sent by the module
     */
    std::size_t getNumOutputMessages() const;
    
    /**
     * @return the most recently sent message by the module
     */
    cMessage* getOutputMessage() const;

    /**
     * @return the (idx + 1)th message sent by the module
     */
    cMessage* getOutputMessage(std::size_t idx) const;
    
    /**
     * @return the most recently sent message by the module and removes it
     * from the list of output messages
     */
    cMessage* popOutputMessage();

protected:

    /** Called when the module to test sends an outgoing message */
    virtual void arrived(cMessage*, int, simtime_t);

    /** Pure virtual function required for inheritance */
    virtual void scheduleStart(simtime_t);

private:

    /** Load a ned file if it hasn't been previously loaded */
    static void loadNedFile(const char* nedFile);
    
    /** List of nedfiles that have been loaded */
    static std::vector<std::string> loadedNedFiles_;
    
    /** The module to send the test message to */
    cSimpleModule* module_;

    /** Set to true after the model has been initialized */
    bool moduleInitialized_;

    /** Dummy gates to connect to the test module */
    std::vector<cGate*> dummyGates_;

    /** List of response messages sent by the module being tested */
    std::vector<cMessage*> arrivals_;
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
