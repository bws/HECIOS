//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "csimple_module_tester.h"
#include <algorithm>
#include <cassert>
#include <string>
using namespace std;

vector<string> cSimpleModuleTester::loadedNedFiles_;

/** A mock parent module to set use for the initial simulation context */
class MockParentModule : public cModule
{
protected:
    virtual void scheduleStart(simtime_t t) {};

    virtual void arrived(cMessage *msg,int n,simtime_t t) {};
};
    
cSimpleModuleTester::cSimpleModuleTester(const char* moduleTypeName,
                                         const char* nedFile,
                                         bool autoInitialize)
    : cModule(),
      module_(0),
      moduleInitialized_(false),
      parentModule_(0)
{
    // Attempt to load the ned file into the simulation
    loadNedFile(nedFile);

    // Create the mock parent module and set it as the module context
    parentModule_ = new MockParentModule();
    simulation.setContextModule(parentModule_);
    
    // Retrieve the constructed module interface
    cModuleInterface* moduleIF =
        dynamic_cast<cModuleInterface*>(modinterfaces.instance()->get(0));
    assert(0 != moduleIF);

    // Construct the module type
    cModuleType* moduleType = findModuleType(moduleTypeName);
    assert(0 != moduleType);

    // Create the correct module type
    module_ = dynamic_cast<cSimpleModule*>(moduleType->create("ModuleToTest",
                                                              this));
    assert(0 != module_);

    // Construct dummy gates to connect to each of the modules gates
    int numGates = module_->gates();
    for (int i = 0; i < numGates; i++)
    {
        ostringstream s;
        s << "dummy" << i;
        string gateName = s.str();
        cGate* gate = new cGate(gateName.c_str(), 'I');
        dummyGates_.push_back(gate);
        dummyGates_[i]->setOwnerModule(this, i);
        module_->gate(i)->connectTo(dummyGates_[i]);
    }
    
    // Initialize if requested
    if (autoInitialize)
    {
        callInitialize();
    }
}

cSimpleModuleTester::~cSimpleModuleTester()
{
    // Cleanup the message arrivals first
    for (size_t i = 0; i < arrivals_.size(); i++)
    {
        delete arrivals_[i];
    }

    // Cleanup the simulation (NOTE: this is likely to fail in the future)
    simulation.setContextModule(0);
    
    // Cleanup module
    if (moduleInitialized_)
    {
        module_->callFinish();
    }
    delete module_;
    module_ = 0;

    // Cleanup the dummy gates created for testing
    for (size_t i = 0; i < dummyGates_.size(); i++)
    {
        delete dummyGates_[i];
    }
}

void cSimpleModuleTester::callInitialize()
{
    module_->callInitialize();
    simulation.setContextModule(module_);
    moduleInitialized_ = true;    
}

void cSimpleModuleTester::deliverMessage(cMessage* msg, const char* inGateName)
{
    assert(true == moduleInitialized_);
    assert(0 != msg);
    assert(0 != inGateName);

    // Send the message over the gate
    cGate* inGate = module_->gate(inGateName);
    deliverMessage(msg, inGate);
}

void cSimpleModuleTester::deliverMessage(cMessage* msg, cGate* inGate)
{
    assert(true == moduleInitialized_);
    assert(0 != msg);
    assert(0 != inGate);
    
    // Set the message's arrival data
    msg->setArrival(module_, inGate->id());

    // Add the message to the simulator's message queue
    simulation.msgQueue.insert(msg);

    // perform the message
    simulation.doOneEvent(module_);
}

size_t cSimpleModuleTester::getNumOutputMessages() const
{
    return arrivals_.size();
}

cMessage* cSimpleModuleTester::getOutputMessage() const
{
    cMessage* out = 0;
    if (0 < arrivals_.size())
    {
        out = arrivals_.back();
    }
    return out;
}

cMessage* cSimpleModuleTester::getOutputMessage(size_t idx) const
{
    assert(idx < arrivals_.size());
    return arrivals_[idx];
}

cMessage* cSimpleModuleTester::popOutputMessage()
{
    cMessage* out = 0;
    if (0 < arrivals_.size())
    {
        out = arrivals_.back();
        arrivals_.pop_back();
    }
    return out;
}

void cSimpleModuleTester::arrived(cMessage* msg, int, simtime_t)
{
    arrivals_.push_back(msg);
}

void cSimpleModuleTester::scheduleStart(simtime_t)
{
    cerr << "Schedule started" << endl;
}

void cSimpleModuleTester::loadNedFile(const char* nedFile)
{
    //assert(0 != sim_);
    // FIXME: Strictly speaking, this is not correct, we need to make
    // sure that this ned file has not been loaded before, but because
    // relative and absolute paths may differ, this only works because
    // I always use the shortest possible relative path
    //
    // Better would be to figure out why we can't just create a new
    // simulation for each cSimpleModuleTester
    //
    vector<string>::const_iterator iter = find(loadedNedFiles_.begin(),
                                               loadedNedFiles_.end(),
                                               nedFile);
    // If the nedfile isn't already loaded
    if (loadedNedFiles_.end() == iter)
    {
        // Load the ned file to construct the gates
        simulation.loadNedFile(nedFile);
        loadedNedFiles_.push_back(nedFile);
    }
}
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
