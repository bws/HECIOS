#ifndef NO_TRANSLATION_TEST_H
#define NO_TRANSLATION_TEST_H
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

    // Test a ReadBlocks request
    spfsOSReadBlocksRequest req;
    moduleTester.deliverMessage(&req, "in");
    cMessage* output1 = moduleTester.getOutputMessage();
    CPPUNIT_ASSERT(0 == output1);
    
    // Test a response
    spfsOSReadDeviceResponse resp;
    //moduleTester.deliverMessage(&resp, "response");
    cMessage* output2 = moduleTester.getOutputMessage();
    CPPUNIT_ASSERT(0 == output2);
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
