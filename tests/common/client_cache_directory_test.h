#ifndef CLIENT_CACHE_DIRECTORY_TEST_H
#define CLIENT_CACHE_DIRECTORY_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
    //CPPUNIT_TEST(testGetClientsNeedingInvalidate);
    //CPPUNIT_TEST(testRemoveClientCacheEntry);
    //CPPUNIT_TEST(testAddClientCacheEntry);
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
    ClientCacheDirectory::ClientCache cache0 = {0,0}, cache1 = {1,1}, cache2 = {2,2};
    instance.addClientCacheEntry(cache0, file1, page10, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache0, file1, page11, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache0, file1, page12, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache1, file1, page11, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache1, file1, page12, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache2, file1, page10, ClientCacheDirectory::EXCLUSIVE);

    FileView view(0, new ByteDataType());
    BasicDistribution dist;
    ClientCacheDirectory::InvalidationMap connections = instance.getClientsNeedingInvalidate(file1,
                                                                         10,
                                                                         0,
                                                                         1000,
                                                                         view,
                                                                         dist);
    ClientCacheDirectory::InvalidationMap::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(3), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), (iter++)->first.connection);
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), (iter++)->first.connection);
    CPPUNIT_ASSERT_EQUAL(ConnectionId(2), (iter++)->first.connection);
    CPPUNIT_ASSERT(connections.end() == iter);
}

void ClientCacheDirectoryTest::testRemoveClientCacheEntry()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();

    Filename file1("/foo"), file2("/bar");
    FilePageId page1(1), page2(2);
    ClientCacheDirectory::ClientCache cache0 = {0,0}, cache1 = {1,1}, cache2 = {2,2};
    instance.addClientCacheEntry(cache0, file1, page1, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache0, file1, page2, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache0, file2, page1, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache0, file2, page2, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache1, file1, page2, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache2, file2, page1, ClientCacheDirectory::EXCLUSIVE);

    // Now remove two entries
    instance.removeClientCacheEntry(cache0, file1, page2);
    instance.removeClientCacheEntry(cache2, file2, page1);

    FileView view(0, new ByteDataType());
    BasicDistribution dist;
    ClientCacheDirectory::InvalidationMap connections = instance.getClientsNeedingInvalidate(file1,
                                                                         10,
                                                                         10,
                                                                         10,
                                                                         view,
                                                                         dist);
    ClientCacheDirectory::InvalidationMap::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), (iter++)->first.connection);
    CPPUNIT_ASSERT(connections.end() == iter);

    connections = instance.getClientsNeedingInvalidate(file1,
                                                       10,
                                                       20,
                                                       10,
                                                       view,
                                                       dist);
    iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), (iter++)->first.connection);
    CPPUNIT_ASSERT(connections.end() == iter);
}

void ClientCacheDirectoryTest::testAddClientCacheEntry()
{
    ClientCacheDirectory& instance = ClientCacheDirectory::instance();

    Filename file1("/foo"), file2("/bar");
    FilePageId page44(44), page55(55), page66(66);
    ClientCacheDirectory::ClientCache cache0 = {0,0}, cache1 = {1,1}, cache2 = {2,2}, cache3 = {3,3};
    instance.addClientCacheEntry(cache0, file1, page44, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache1, file2, page55, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache2, file1, page66, ClientCacheDirectory::EXCLUSIVE);
    instance.addClientCacheEntry(cache3, file2, page44, ClientCacheDirectory::EXCLUSIVE);

    FileView view(0, new ByteDataType());
    BasicDistribution dist;
    ClientCacheDirectory::InvalidationMap connections = instance.getClientsNeedingInvalidate(file1,
                                                                         10,
                                                                         0,
                                                                         700,
                                                                         view,
                                                                         dist);
    ClientCacheDirectory::InvalidationMap::const_iterator iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(2), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(0), (iter++)->first.connection);
    CPPUNIT_ASSERT_EQUAL(ConnectionId(2), (iter++)->first.connection);
    CPPUNIT_ASSERT(connections.end() == iter);

    connections = instance.getClientsNeedingInvalidate(file2,
                                                       10,
                                                       0,
                                                       600,
                                                       view,
                                                       dist);
    iter = connections.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(2), connections.size());
    CPPUNIT_ASSERT_EQUAL(ConnectionId(1), (iter++)->first.connection);
    CPPUNIT_ASSERT_EQUAL(ConnectionId(3), (iter++)->first.connection);
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
