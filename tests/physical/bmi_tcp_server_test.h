#ifndef BMI_TCP_SERVER_TEST_H
#define BMI_TCP_SERVER_TEST_H
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

/** Unit test for BMITcpServer */
class BMITcpServerTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BMITcpServerTest);
    CPPUNIT_TEST(testCreateExpectedMessage);
    CPPUNIT_TEST(testCreateUnexpectedMessage);
    CPPUNIT_TEST(testCreatePullDataResponse);
    CPPUNIT_TEST(testCreatePushDataResponse);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test creation of an expected message */
    void testCreateExpectedMessage();

    /** Test creation of an unexpected message */
    void testCreateUnexpectedMessage();

    /** Test creation of a pull data response */
    void testCreatePullDataResponse();

    /** Test creation of a push data response */
    void testCreatePushDataResponse();

private:

    cSimpleModuleTester* moduleTester_;
    //BMITcpServer* module_
};

void BMITcpServerTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester(
        "BMITcpServer", "src/physical/bmi_tcp_server.ned", false);
    //module_ = dynamic_cast<BMITcpEndpoint*>(moduleTester_->getModule());

    // Set the listen port
    cModule* module = moduleTester_->getModule();
    module->par("listenPort") = 8000;

    moduleTester_->callInitialize();
}

void BMITcpServerTest::tearDown()
{
    delete moduleTester_;
    moduleTester_ = 0;
}

void BMITcpServerTest::testCreateExpectedMessage()
{
}

void BMITcpServerTest::testCreateUnexpectedMessage()
{
}

void BMITcpServerTest::testCreatePullDataResponse()
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

void BMITcpServerTest::testCreatePushDataResponse()
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
