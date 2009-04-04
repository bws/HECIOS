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
#include "basic_data_type.h"
#include "basic_distribution.h"
#include "client_cache_directory.h"
#include "file_view.h"
using namespace std;

/** Unit test for Client Cache Directory */
class ClientCacheDirectoryTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(ClientCacheDirectoryTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetClientsNeedingInvalidate);
    CPPUNIT_TEST(testRemoveClientCacheEntry);
    CPPUNIT_TEST(testAddClientCacheEntry);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testGetClientsNeedingInvalidate();

    void testRemoveClientCacheEntry();

    void testAddClientCacheEntry();

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
    //ClientCacheDirectory& instance = ClientCacheDirectory::instance();
}

void ClientCacheDirectoryTest::testGetClientsNeedingInvalidate()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();
    Filename file1("/foo");
    FilePageId page10(10), page11(11), page12(12);
    instance.addClientCacheEntry(0, file1, page10);
    instance.addClientCacheEntry(0, file1, page11);
    instance.addClientCacheEntry(0, file1, page12);
    instance.addClientCacheEntry(1, file1, page11);
    instance.addClientCacheEntry(1, file1, page12);
    instance.addClientCacheEntry(2, file1, page10);

    FileView view(0, new ByteDataType());
    BasicDistribution dist;
    set<ConnectionId> connections = instance.getClientsNeedingInvalidate(file1,
                                                                         10,
                                                                         0,
                                                                         1000,
                                                                         view,
                                                                         dist);
    set<ConnectionId>::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(3), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(2), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);
}

void ClientCacheDirectoryTest::testRemoveClientCacheEntry()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();

    Filename file1("/foo"), file2("/bar");
    FilePageId page1(1), page2(2);
    instance.addClientCacheEntry(0, file1, page1);
    instance.addClientCacheEntry(0, file1, page2);
    instance.addClientCacheEntry(0, file2, page1);
    instance.addClientCacheEntry(0, file2, page2);
    instance.addClientCacheEntry(1, file1, page2);
    instance.addClientCacheEntry(2, file2, page1);

    // Now remove two entries
    instance.removeClientCacheEntry(0, file1, page2);
    instance.removeClientCacheEntry(2, file2, page1);

    FileView view(0, new ByteDataType());
    BasicDistribution dist;
    set<ConnectionId> connections = instance.getClientsNeedingInvalidate(file1,
                                                                         10,
                                                                         10,
                                                                         10,
                                                                         view,
                                                                         dist);
    set<ConnectionId>::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);

    connections = instance.getClientsNeedingInvalidate(file1,
                                                       10,
                                                       20,
                                                       10,
                                                       view,
                                                       dist);
    iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);
}

void ClientCacheDirectoryTest::testAddClientCacheEntry()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();

    Filename file1("/foo"), file2("/bar");
    FilePageId page44(44), page55(55), page66(66);
    instance.addClientCacheEntry(0, file1, page44);
    instance.addClientCacheEntry(1, file2, page55);
    instance.addClientCacheEntry(2, file1, page66);
    instance.addClientCacheEntry(3, file2, page44);

    FileView view(0, new ByteDataType());
    BasicDistribution dist;
    set<ConnectionId> connections = instance.getClientsNeedingInvalidate(file1,
                                                                         10,
                                                                         0,
                                                                         700,
                                                                         view,
                                                                         dist);
    set<ConnectionId>::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(2), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), *(iter++));
    CPPUNIT_ASSERT_EQUAL(ConnectionId(2), *(iter++));
    CPPUNIT_ASSERT(connections.end() == iter);

    connections = instance.getClientsNeedingInvalidate(file2,
                                                       10,
                                                       0,
                                                       600,
                                                       view,
                                                       dist);
    iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(2), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), *(iter++));
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
