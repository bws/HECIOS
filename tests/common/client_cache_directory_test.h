#ifndef CLIENT_CACHE_DIRECTORY_TEST_H
#define CLIENT_CACHE_DIRECTORY_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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

#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "client_cache_directory.h"
using namespace std;

/** Unit test for Client Cache Directory */
class ClientCacheDirectoryTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(ClientCacheDirectoryTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetClientsNeedingInvalidate);
    CPPUNIT_TEST(testRemoveClient);
    CPPUNIT_TEST(testAddClient);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testGetClientsNeedingInvalidate();

    void testRemoveClient();

    void testAddClient();

private:
};

void ClientCacheDirectoryTest::setUp()
{
}

void ClientCacheDirectoryTest::tearDown()
{
    ClientCacheDirectory::clearState();
}

void ClientCacheDirectoryTest::testConstructor()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();
}

void ClientCacheDirectoryTest::testGetClientsNeedingInvalidate()
{
    FSHandle handle1 = 0;
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();
    instance.addClient(handle1, 0);
    instance.addClient(handle1, 1);
    instance.addClient(handle1, 2);
    instance.addClient(handle1, 3);

    set<ConnectionId> connections = instance.getClientsNeedingInvalidate(handle1);
    set<ConnectionId>::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(2), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(3), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);
}

void ClientCacheDirectoryTest::testRemoveClient()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();

    FSHandle handle1 = 1024;
    instance.addClient(handle1, 0);
    instance.addClient(handle1, 1);
    instance.addClient(handle1, 2);
    instance.addClient(handle1, 3);

    // Now remove two entries
    instance.removeClient(handle1, 1);
    instance.removeClient(handle1, 2);

    set<ConnectionId> connections = instance.getClientsNeedingInvalidate(handle1);
    set<ConnectionId>::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(3), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);
}

void ClientCacheDirectoryTest::testAddClient()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();

    FSHandle handle1 = 3007;
    instance.addClient(handle1, 0);
    instance.addClient(handle1, 1);
    instance.addClient(handle1, 2);
    instance.addClient(handle1, 3);

    set<ConnectionId> connections = instance.getClientsNeedingInvalidate(handle1);
    set<ConnectionId>::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(2), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(3), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);
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
