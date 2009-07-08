#ifndef NO_TRANSLATION_TEST_H
#define NO_TRANSLATION_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "block_translator.h"
#include "csimple_module_tester.h"
#include "os_proto_m.h"
using namespace std;

/** Unit test for NoTranslation */
class NoTranslationTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(NoTranslationTest);
    CPPUNIT_TEST(testHandleMessage);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testHandleMessage();

private:
};

void NoTranslationTest::setUp()
{
}

void NoTranslationTest::tearDown()
{
}

void NoTranslationTest::testHandleMessage()
{
    cSimpleModuleTester moduleTester("NoTranslation",
                                     "src/os/block_translator.ned");

    // Setup the originating request
    spfsOSBlockIORequest ioRequest;
    
    // Test a ReadBlocks request
    spfsOSReadBlocksRequest blocksRequest;
    blocksRequest.setContextPointer(&ioRequest);
    moduleTester.deliverMessage(&blocksRequest, "in");
    cMessage* output1 = moduleTester.popOutputMessage();
    CPPUNIT_ASSERT(0 == output1);
    
    // Test a response that triggers a response
    ioRequest.setNumRemainingResponses(1);
    spfsOSReadDeviceResponse resp;
    resp.setContextPointer(&blocksRequest);
    //moduleTester.deliverMessage(&resp, "response");
    //cMessage* output2 = moduleTester.getOutputMessage();
    //CPPUNIT_ASSERT(0 == output2);
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
