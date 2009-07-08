#ifndef BMI_TCP_ENDPOINT_TEST_H
#define BMI_TCP_ENDPOINT_TEST_H
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

/** Unit test for BMITcpEndpoint */
class BMITcpEndpointTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BMITcpEndpointTest);
    CPPUNIT_TEST(testCreateExpectedMessage);
    CPPUNIT_TEST(testCreateUnexpectedMessage);
    CPPUNIT_TEST(testCreatePullDataResponse);
    CPPUNIT_TEST(testCreatePushDataResponse);
    //CPPUNIT_TEST(testClientRequest);
    //CPPUNIT_TEST(testClientResponse);
    //CPPUNIT_TEST(testServerRequest);
    //CPPUNIT_TEST(testServerResponse);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a GetAttr request */
    void testCreateExpectedMessage();

    /** Test components of a GetAttr request */
    void testCreateUnexpectedMessage();

    /** Test components of a GetAttr request */
    void testCreatePullDataResponse();

    /** Test components of a GetAttr request */
    void testCreatePushDataResponse();

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
    //BMITcpEndpoint* module_
};

void BMITcpEndpointTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester(
        "BMITcpEndpoint", "src/physical/bmi_tcp_endpoint.ned", false);
    //module_ = dynamic_cast<BMITcpEndpoint*>(moduleTester_->getModule());

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

void BMITcpEndpointTest::testCreateExpectedMessage()
{
}

void BMITcpEndpointTest::testCreateUnexpectedMessage()
{
}

void BMITcpEndpointTest::testCreatePullDataResponse()
{
    /*
    spfsBMIPullDataRequest* pullRequest = new spfsBMIPullDataRequest();
    spfsBMIPullDataResponse* pullResponse =
        module_->createPullDataResponse(pullRequest);
    CPPUNIT_ASSERT(0 == pullResponse);
    delete pullRequest;
    delete pullResponse;
    */
}

void BMITcpEndpointTest::testCreatePushDataResponse()
{
    /*
    spfsBMIPushDataRequest* pushRequest = new spfsBMIPushDataRequest();
    spfsBMIPushDataResponse* pushResponse =
        module_->createPushDataResponse(pushRequest);
    CPPUNIT_ASSERT(0 == pushResponse);
    delete pushRequest;
    delete pushResponse;
    */
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
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
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
