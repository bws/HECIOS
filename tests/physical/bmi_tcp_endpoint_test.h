#ifndef BMI_TCP_ENDPOINT_TEST_H
#define BMI_TCP_ENDPOINT_TEST_H
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
#include "csimple_module_tester.h"
#include "bmi_proto_m.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for BMITcpEndpoint */
class BMITcpEndpointTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BMITcpEndpointTest);
    CPPUNIT_TEST(testClientRequest);
    CPPUNIT_TEST(testClientResponse);
    CPPUNIT_TEST(testServerRequest);
    CPPUNIT_TEST(testServerResponse);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a GetAttr request */
    void testClientRequest();

    /** Test components of a SetAttr request */
    void testClientResponse();

    /** Test components of a GetAttr request */
    void testServerRequest();

    /** Test components of a SetAttr request */
    void testServerResponse();

private:

    cSimpleModuleTester* moduleTester_;
};

void BMITcpEndpointTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester(
        "BMITcpEndpoint", "src/physical/bmi_tcp_endpoint.ned", false);

    // Set the listen port
    cModule* module = moduleTester_->getModule();
    module->par("listenPort") = 8000;

    moduleTester_->callInitialize();
}

void BMITcpEndpointTest::tearDown()
{
    delete moduleTester_;
    moduleTester_ = 0;
}

void BMITcpEndpointTest::testClientRequest()
{
    // Create the application request
    spfsMPIFileReadRequest mpiRequest(0, SPFS_MPI_FILE_READ_REQUEST);
    spfsGetAttrRequest* getAttrRequest =
        new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    getAttrRequest->setHandle(1);
    getAttrRequest->setContextPointer(&mpiRequest);

    // Deliver the application request
    moduleTester_->deliverMessage(getAttrRequest, "appIn");

    // Test output
    CPPUNIT_ASSERT_EQUAL(1u, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsBMIUnexpectedMessage*>(out1));
}

void BMITcpEndpointTest::testClientResponse()
{
    CPPUNIT_FAIL("Test not yet implemented.");
}

void BMITcpEndpointTest::testServerRequest()
{
    CPPUNIT_FAIL("Test not yet implemented.");
}

void BMITcpEndpointTest::testServerResponse()
{
    CPPUNIT_FAIL("Test not yet implemented.");
}

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
