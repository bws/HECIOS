#ifndef MPI_TCP_CLIENT_TEST_H
#define MPI_TCP_CLIENT_TEST_H
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
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for MPITcpClient */
class MPITcpClientTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(MPITcpClientTest);
    CPPUNIT_TEST(testMPIMessage);
    CPPUNIT_TEST(testSocketData);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test delivery of an MPI message */
    void testMPIMessage();

    /** Test delivery of a socket message */
    void testSocketData();

private:
    /** */
    cSimpleModuleTester* moduleTester_;
};

void MPITcpClientTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester(
        "MPITcpClient", "src/physical/mpi_tcp_client.ned", false);
    moduleTester_->callInitialize();
}

void MPITcpClientTest::tearDown()
{
    delete moduleTester_;
    moduleTester_ = 0;
}

void MPITcpClientTest::testMPIMessage()
{
}

void MPITcpClientTest::testSocketData()
{
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
