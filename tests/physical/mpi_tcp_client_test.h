#ifndef MPI_TCP_CLIENT_TEST_H
#define MPI_TCP_CLIENT_TEST_H
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
