#include "csimple_module_tester.h"
#include <cassert>
#include <string>
using namespace std;

cSimpleModuleTester::cSimpleModuleTester(const char* moduleTypeName,
                                         const char* nedFile)
    : cSimulation("ModuleTester"),
      module_(0)
{
    // Load the ned file to construct the gates
    loadNedFile(nedFile);

    // Retrieve the constructed module interface
    cModuleInterface* moduleIF =
        dynamic_cast<cModuleInterface*>(modinterfaces.instance()->get(0));
    assert(0 != moduleIF);

    // Construct the module type
    cModuleType* moduleType = findModuleType(moduleTypeName);
    assert(0 != moduleType);

    // Create the correct module type
    module_ = dynamic_cast<cSimpleModule*>(moduleType->create(0,0));
    assert(0 != module_);

    // Register the module with the simulation
    registerModule(module_);

    // Initialize the module
    module_->callInitialize();
}

cSimpleModuleTester::~cSimpleModuleTester()
{
    // Finalize the module
    module_->callFinish();
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
    msgQueue.insert(msg);

    // perform the message
    doOneEvent(module_);
}

cMessage* cSimpleModuleTester::getOutputMessage()
{
    return msgQueue.getFirst();
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
