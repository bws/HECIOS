#ifndef CLIENT_FS_STATE_TEST_H
#define CLIENT_FS_STATE_TEST_H
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
    CPPUNIT_TEST(testInsertName);
    CPPUNIT_TEST(testRemoveName);
    CPPUNIT_TEST(testLookupName);
    CPPUNIT_TEST(testLookupName2);
    CPPUNIT_TEST(testServerNotUsed);
    CPPUNIT_TEST(testSelectServer);
    CPPUNIT_TEST(testHashPath);
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

    void testInsertName();

    void testRemoveName();

    void testLookupName();

    void testLookupName2();

    void testServerNotUsed();

    void testSelectServer();

    void testHashPath();

    void testServers();

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
    //CPPUNIT_ASSERT_EQUAL(0, state.root());
    //CPPUNIT_ASSERT_EQUAL(2, state.defaultNumServers());
    //CPPUNIT_ASSERT_EQUAL(2, state.totalNumServers());
}

void ClientFSStateTest::testInsertAttr()
{
    ClientFSState state;
    FSMetaData attr1 = {0};
    FSHandle handle1 = 1;

    state.insertAttr(handle1, attr1);
    CPPUNIT_ASSERT(*(state.lookupAttr(handle1)) == attr1);
}

void ClientFSStateTest::testRemoveAttr()
{
    ClientFSState state;
    FSMetaData attr1 = {0};
    FSHandle handle1 = 1;

    state.insertAttr(handle1, attr1);
    state.removeAttr(handle1);
    CPPUNIT_ASSERT(0 == state.lookupAttr(handle1));
}

void ClientFSStateTest::testLookupAttr()
{
    ClientFSState state;
    FSMetaData attr1 = {0};
    FSHandle handle1 = 1;

    state.insertAttr(handle1, attr1);
    CPPUNIT_ASSERT(*(state.lookupAttr(handle1)) == attr1);
}

void ClientFSStateTest::testInsertName()
{
    ClientFSState state;
    string dir1 = "/dir1";
    FSHandle handle1 = 1;

    state.insertName(dir1, handle1);
    CPPUNIT_ASSERT_EQUAL(*(state.lookupName(dir1)), handle1);
}

void ClientFSStateTest::testRemoveName()
{
    ClientFSState state;
    string dir1 = "/dir1";
    FSHandle handle1 = 1;

    // Ensure directory removal works
    state.insertName(dir1, handle1);
    state.removeName(dir1);
    CPPUNIT_ASSERT(0 == state.lookupName(dir1));
}

void ClientFSStateTest::testLookupName()
{
    ClientFSState state;

    // Attempt to lookup non-existant entry
    CPPUNIT_ASSERT(0 == state.lookupName(string("non-entry")));

    // Lookup an entry that exists
    string dir1 = "/dir1";
    FSHandle handle1 = 1;
    state.insertName(dir1, handle1);
    CPPUNIT_ASSERT_EQUAL(*(state.lookupName(dir1)), handle1);
}

void ClientFSStateTest::testLookupName2()
{
    ClientFSState state;

    // Insert several entries
    Filename file1("/dir1");
    FSHandle handle1 = 1;
    state.insertName(file1.str(), handle1);
    Filename file2("/dir1/dir2");
    FSHandle handle2 = 2;
    state.insertName(file2.str(), handle2);
    Filename file3("/dir1/dir2/dir3");
    FSHandle handle3 = 3;
    state.insertName(file3.str(), handle3);

    // Lookup a full entry
    size_t resolvedSeg1 = 0;
    FSHandle lookup1 = 101;
    FSLookupStatus status1 = state.lookupName(file1, resolvedSeg1, &lookup1);
    CPPUNIT_ASSERT_EQUAL(SPFS_FOUND, status1);
    CPPUNIT_ASSERT_EQUAL(file1.getNumPathSegments(), resolvedSeg1);
    CPPUNIT_ASSERT_EQUAL(handle1, lookup1);

    // Lookup a partial entry
    size_t resolvedSeg2 = 0;
    FSHandle lookup2 = 102;
    FSLookupStatus status2 = state.lookupName(Filename("/dir1/foo"), resolvedSeg2, &lookup2);
    CPPUNIT_ASSERT_EQUAL(SPFS_PARTIAL, status2);
    CPPUNIT_ASSERT_EQUAL(size_t(2), resolvedSeg2);
    CPPUNIT_ASSERT_EQUAL(handle1, lookup2);

    // Lookup a partial entry
    size_t resolvedSeg3 = 0;
    FSHandle lookup3 = 103;
    FSLookupStatus status3 = state.lookupName(Filename("/dir1/dir2/bar"), resolvedSeg3, &lookup3);
    CPPUNIT_ASSERT_EQUAL(SPFS_PARTIAL, status3);
    CPPUNIT_ASSERT_EQUAL(size_t(3), resolvedSeg3);
    CPPUNIT_ASSERT_EQUAL(handle2, lookup3);

    // Lookup a non-existent entry
    size_t resolvedSeg4 = 0;
    FSHandle lookup4 = 104;
    FSLookupStatus status4 = state.lookupName(Filename("/so/long/and"), resolvedSeg4, &lookup4);
    CPPUNIT_ASSERT_EQUAL(SPFS_NOTFOUND, status4);
    CPPUNIT_ASSERT_EQUAL(size_t(0), resolvedSeg4);
    CPPUNIT_ASSERT_EQUAL(FSHandle(104), lookup4);
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
