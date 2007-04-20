#ifndef CLIENT_FS_STATE_TEST_H
#define CLIENT_FS_STATE_TEST_H

#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "client_fs_state.h"
using namespace std;

/** Unit test for ClientFSState */
class ClientFSStateTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(ClientFSStateTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testInsertAttr);
    CPPUNIT_TEST(testRemoveAttr);
    CPPUNIT_TEST(testLookupAttr);
    CPPUNIT_TEST(testInsertDir);
    CPPUNIT_TEST(testRemoveDir);
    CPPUNIT_TEST(testLookupDir);
    CPPUNIT_TEST(testServerNotUsed);
    CPPUNIT_TEST(testSelectServer);
    CPPUNIT_TEST(testHashPath);
    CPPUNIT_TEST(testRoot);
    CPPUNIT_TEST(testTotalNumServers);
    CPPUNIT_TEST(testDefaultNumServers);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testConstructor();

    void testInsertAttr();

    void testRemoveAttr();
    
    void testLookupAttr();

    void testInsertDir();

    void testRemoveDir();
    
    void testLookupDir();

    void testServerNotUsed();

    void testSelectServer();

    void testHashPath();

    void testServers();

    void testRoot();

    void testTotalNumServers();

    void testDefaultNumServers();

private:
};

void ClientFSStateTest::setUp()
{
}

void ClientFSStateTest::tearDown()
{
}

void ClientFSStateTest::testConstructor()
{
    ClientFSState state;
    CPPUNIT_ASSERT_EQUAL(0, state.root());
    CPPUNIT_ASSERT_EQUAL(2, state.defaultNumServers());
    CPPUNIT_ASSERT_EQUAL(2, state.totalNumServers());
}

void ClientFSStateTest::testInsertAttr()
{
    ClientFSState state;
    FSMetaData attr1;
    FSHandle handle1 = 1;

    state.insertAttr(handle1, attr1);
    CPPUNIT_ASSERT(*(state.lookupAttr(handle1)) == attr1);
}

void ClientFSStateTest::testRemoveAttr()
{
    ClientFSState state;
    FSMetaData attr1;
    FSHandle handle1 = 1;

    state.insertAttr(handle1, attr1);
    state.removeAttr(handle1);
    CPPUNIT_ASSERT(0 == state.lookupAttr(handle1));
}

void ClientFSStateTest::testLookupAttr()
{
    ClientFSState state;
    FSMetaData attr1;
    FSHandle handle1 = 1;

    state.insertAttr(handle1, attr1);
    CPPUNIT_ASSERT(*(state.lookupAttr(handle1)) == attr1);
}

void ClientFSStateTest::testInsertDir()
{
    ClientFSState state;
    string dir1 = "/dir1";
    FSHandle handle1 = 1;

    state.insertDir(dir1, handle1);
    CPPUNIT_ASSERT_EQUAL(*(state.lookupDir(dir1)), handle1);
}

void ClientFSStateTest::testRemoveDir()
{
    ClientFSState state;
    string dir1 = "/dir1";
    FSHandle handle1 = 1;

    // Ensure directory removal works
    state.insertDir(dir1, handle1);
    state.removeDir(dir1);
    CPPUNIT_ASSERT(0 == state.lookupDir(dir1));
}

void ClientFSStateTest::testLookupDir()
{
    ClientFSState state;

    // Attempt to lookup non-existant entry
    CPPUNIT_ASSERT(0 == state.lookupDir(string("non-entry")));

    // Lookup an entry that exists
    string dir1 = "/dir1";
    FSHandle handle1 = 1;
    state.insertDir(dir1, handle1);
    CPPUNIT_ASSERT_EQUAL(*(state.lookupDir(dir1)), handle1);
}

void ClientFSStateTest::testServerNotUsed()
{
    //ClientFSState state;
}

void ClientFSStateTest::testSelectServer()
{
    ClientFSState state;
    CPPUNIT_ASSERT_EQUAL(state.selectServer(), 0);
}

void ClientFSStateTest::testHashPath()
{
    ClientFSState state;
    string dir1 = "dir1";
    string dir2 = "dir2";
    // FIXME
    // CPPUNIT_ASSERT(state.hashPath(dir1) != state.hashPath(dir2));
}

void ClientFSStateTest::testServers()
{
    // FIXME
    //ClientFSState state;
}

void ClientFSStateTest::testRoot()
{
    ClientFSState state;
    CPPUNIT_ASSERT_EQUAL(0, state.root());
}

void ClientFSStateTest::testTotalNumServers()
{
    ClientFSState state;
    CPPUNIT_ASSERT_EQUAL(2, state.totalNumServers());
}

void ClientFSStateTest::testDefaultNumServers()
{
    ClientFSState state;
    CPPUNIT_ASSERT_EQUAL(2, state.defaultNumServers());
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
