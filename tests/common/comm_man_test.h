#ifndef COMM_MAN_TEST_H
#define COMM_MAN_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2008 Yang Wu, Brad Settlemyer
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cppunit/extensions/HelperMacros.h>
#include "comm_man.h"

class CommManTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommManTest);
    CPPUNIT_TEST(testCommSize);
    CPPUNIT_TEST(testDupComm);
    CPPUNIT_TEST(testJoinComm);
    CPPUNIT_TEST(testCommRank);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Pre test code */
    void setUp();

    /** Post test code */
    void tearDown();

    void testCommSize();
    void testDupComm();
    void testJoinComm();
    void testCommRank();

};

void CommManTest::setUp()
{
    // Register the predefined communicators
    CommMan::instance().setCommWorld(0);
    CommMan::instance().setCommSelf(1);

    // Register 4 ranks
    CommMan::instance().registerRank(1);
    CommMan::instance().registerRank(2);
    CommMan::instance().registerRank(3);
    CommMan::instance().registerRank(4);
}

void CommManTest::tearDown()
{
    CommMan::clearState();
}

void CommManTest::testCommSize()
{
    CommMan& cm = CommMan::instance();
    CPPUNIT_ASSERT_EQUAL(size_t(1), cm.commSize(SPFS_COMM_SELF));
    CPPUNIT_ASSERT_EQUAL(size_t(4), cm.commSize(SPFS_COMM_WORLD));
}

void CommManTest::testDupComm()
{
    CommMan& cm = CommMan::instance();

    // Copy COMM_SELF and make sure results are identical
    Communicator selfCopy = 10;
    cm.dupComm(SPFS_COMM_SELF, selfCopy);
    CPPUNIT_ASSERT_EQUAL(cm.commSize(SPFS_COMM_SELF), cm.commSize(selfCopy));
    CPPUNIT_ASSERT_EQUAL(size_t(1), cm.commSize(selfCopy));

    // Copy COMM_WORLD and make sure results are identical
    Communicator worldCopy = 11;
    cm.dupComm(SPFS_COMM_WORLD, worldCopy);
    CPPUNIT_ASSERT_EQUAL(cm.commSize(SPFS_COMM_WORLD), cm.commSize(worldCopy));
    CPPUNIT_ASSERT_EQUAL(size_t(4), cm.commSize(worldCopy));
}

void CommManTest::testJoinComm()
{
    CommMan& cm = CommMan::instance();
    Communicator id = 3;

    // 1st node joins communicator, rank should be 0
    int rank0 = cm.joinComm(id, 1);
    CPPUNIT_ASSERT_EQUAL(0, rank0);
    CPPUNIT_ASSERT_EQUAL(size_t(1), cm.commSize(id));

    // 2nd node joins communicator, rank should be 1
    int rank1 = cm.joinComm(id, 4);
    CPPUNIT_ASSERT_EQUAL(1, rank1);
    CPPUNIT_ASSERT_EQUAL(size_t(2), cm.commSize(id));
}

void CommManTest::testCommRank()
{
    CommMan& cm = CommMan::instance();

    // rank in MPI_COMM_SELF should be 0
    CPPUNIT_ASSERT_EQUAL(cm.commRank(SPFS_COMM_SELF, 2), 0);

    // rank in MPI_COMM_WORLD should be the same
    CPPUNIT_ASSERT_EQUAL(cm.commRank(SPFS_COMM_WORLD, 2), 2);

    // rank in communicator should be groupRank
    int id = 3;
    int groupRank = cm.joinComm(id, 3);
    CPPUNIT_ASSERT_EQUAL(0, groupRank);
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
