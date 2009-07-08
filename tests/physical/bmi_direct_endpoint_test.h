#ifndef BMI_DIRECT_ENDPOINT_TEST_H
#define BMI_DIRECT_ENDPOINT_TEST_H
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
#include "csimple_module_tester.h"
#include "bmi_proto_m.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for BMIDirectEndpoint */
class BMIDirectEndpointTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BMIDirectEndpointTest);
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

void BMIDirectEndpointTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester(
        "BMIDirectEndpoint", "src/physical/bmi_direct_endpoint.ned");
}

void BMIDirectEndpointTest::tearDown()
{
    //delete moduleTester_;
    moduleTester_ = 0;
}

// Test the sending an application request to the client
void BMIDirectEndpointTest::testClientRequest()
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
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsBMIUnexpectedMessage*>(out1));
}

// Test the sending an application response to the client
void BMIDirectEndpointTest::testClientResponse()
{
    // Create the application response
    spfsGetAttrRequest getAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    getAttrRequest.setHandle(1);

    spfsGetAttrResponse* getAttrResponse =
        new spfsGetAttrResponse(0, SPFS_GET_ATTR_REQUEST);
    getAttrResponse->setContextPointer(&getAttrRequest);

    // Create and deliver the BMI wrapper
    spfsBMIExpectedMessage* bmiMsg = new spfsBMIExpectedMessage();
    bmiMsg->encapsulate(getAttrResponse);
    moduleTester_->deliverMessage(bmiMsg, "netIn");

    // Test output
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsGetAttrResponse*>(out1));
    CPPUNIT_ASSERT_EQUAL((cMessage*)getAttrResponse, out1);
}

void BMIDirectEndpointTest::testServerRequest()
{
    // Create the application request
    spfsMPIFileReadRequest mpiRequest(0, SPFS_MPI_FILE_READ_REQUEST);
    spfsGetAttrRequest* getAttrRequest =
        new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    getAttrRequest->setHandle(1);
    getAttrRequest->setContextPointer(&mpiRequest);

    // Create and deliver the BMI wrapper
    spfsBMIExpectedMessage* bmiMsg = new spfsBMIExpectedMessage();
    bmiMsg->encapsulate(getAttrRequest);
    moduleTester_->deliverMessage(bmiMsg, "netIn");

    // Test output
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsGetAttrRequest*>(out1));
}

void BMIDirectEndpointTest::testServerResponse()
{
    // Create the application response
    spfsGetAttrRequest getAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    getAttrRequest.setHandle(1);

    spfsGetAttrResponse* getAttrResponse =
        new spfsGetAttrResponse(0, SPFS_GET_ATTR_REQUEST);
    getAttrResponse->setContextPointer(&getAttrRequest);

    // Deliver the application request
    moduleTester_->deliverMessage(getAttrResponse, "appIn");

    // Test output
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsBMIExpectedMessage*>(out1));
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
