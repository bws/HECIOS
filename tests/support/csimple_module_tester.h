#ifndef C_SIMPLE_MODULE_TESTER_H
#define C_SIMPLE_MODULE_TESTER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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

    /** Note the arrival of a message */
    void arrived(cMessage* msg, cGate* arrivalGate, simtime_t arrivalTime);

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

    /** A faked up parent module */
    cModule* parentModule_;
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
