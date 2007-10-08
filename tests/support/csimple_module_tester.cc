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
#include "csimple_module_tester.h"
#include <cassert>
#include <string>
using namespace std;

cSimpleModuleTester::cSimpleModuleTester(const char* moduleTypeName,
                                         const char* nedFile)
    : cModule(),
      sim_(new cSimulation("TestDriverSimulation")),
      module_(0)
{
    // Load the ned file to construct the gates
    sim_->loadNedFile(nedFile);

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
        dummyGates_.push_back(new cGate(gateName.c_str(), 'I'));
        dummyGates_[i]->setOwnerModule(this, i);
        module_->gate(i)->connectTo(dummyGates_[i]);
    }
    
    // Register the module with the simulation
    //sim_->setSystemModule(module_);
    //sim_->registerModule(module_);

    // Initialize the module
    module_->callInitialize();
}

cSimpleModuleTester::~cSimpleModuleTester()
{
    // Cleanup the message arrivals
    for (size_t i = 0; i < arrivals_.size(); i++)
    {
        delete arrivals_[i];
    }

    // Cleanup the simulation
    //sim_->callFinish();
    //sim_->transferToMain();
    //module_->removeFromOwnershipTree();
    //sim_->deleteNetwork();
    delete sim_;
    sim_ = 0;
    
    // Cleanup module
    module_->callFinish();
    delete module_;
    module_ = 0;

    // Cleanup the dummy gates created for testing
    for (size_t i = 0; i < dummyGates_.size(); i++)
    {
        delete dummyGates_[i];
    }
}

void cSimpleModuleTester::deliverMessage(cMessage* msg, const char* inGateName)
{
    cGate* inGate = module_->gate(inGateName);
    deliverMessage(msg, inGate);
}

void cSimpleModuleTester::deliverMessage(cMessage* msg, cGate* inGate)
{
    // Set the message's arrival data
    msg->setArrival(module_, inGate->id());

    // Add the message to the simulator's message queue
    sim_->msgQueue.insert(msg);

    // perform the message
    sim_->doOneEvent(module_);
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

void cSimpleModuleTester::arrived(cMessage* msg, int, simtime_t)
{
    arrivals_.push_back(msg);
}

void cSimpleModuleTester::scheduleStart(simtime_t)
{
    cerr << "Schedule started" << endl;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
