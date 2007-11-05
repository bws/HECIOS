#ifndef BMI_TCP_SERVER_TEST_H
#define BMI_TCP_SERVER_TEST_H
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
