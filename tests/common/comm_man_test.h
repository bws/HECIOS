#include <cppunit/extensions/HelperMacros.h>
#include "comm_man.h"

class CommManTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommManTest);
    CPPUNIT_TEST(testJoinComm);
    CPPUNIT_TEST(testCommRank);
    CPPUNIT_TEST(testCommTrans);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){};
    void tearDown(){};
    
    void testJoinComm();
    void testCommRank();
    void testCommTrans();
};

void CommManTest::testJoinComm()
{
    CommMan::clearState();
    CommMan& cm = CommMan::instance();

    // 1st node join, rank should be 0
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(MPI_COMM_WORLD, 0), 0);
    // size should be 1
    CPPUNIT_ASSERT_EQUAL(cm.commSize(MPI_COMM_WORLD), (size_t)1);
    // 2nd node join, rank should be 1
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(MPI_COMM_WORLD, 0), 1);
    // size should be 2
    CPPUNIT_ASSERT_EQUAL(cm.commSize(MPI_COMM_WORLD), (size_t)2);

    // size of MPI_COMM_SELF should always be 1
    CPPUNIT_ASSERT_EQUAL(cm.commSize(MPI_COMM_SELF), (size_t)1);

    // now test for a new communicator

    int id = 11321;
    
    // 1st node join, rank should be 0
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(id, 0), 0);
    // size should be 1
    CPPUNIT_ASSERT_EQUAL(cm.commSize(id), (size_t)1);
    // 2nd node join, rank should be 1
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(id, 1), 1);
    // size should be 2
    CPPUNIT_ASSERT_EQUAL(cm.commSize(id), (size_t)2);
    // 2nd node join twice, rank should be 1
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(id, 1), 1);
    // size should still be 2
    CPPUNIT_ASSERT_EQUAL(cm.commSize(id), (size_t)2);
}

void CommManTest::testCommRank()
{
    CommMan::clearState();
    CommMan& cm = CommMan::instance();

    int id = 13213;
    
    int rank = cm.joinComm(MPI_COMM_WORLD, 0);

    int group_rank = cm.joinComm(id, rank);

    // rank in communicator should be group_rank
    CPPUNIT_ASSERT_EQUAL(cm.commRank(id, rank), group_rank);

    // rank in MPI_COMM_SELF should be 0
    CPPUNIT_ASSERT_EQUAL(cm.commRank(MPI_COMM_SELF, rank), 0);
}

void CommManTest::testCommTrans()
{
    CommMan::clearState();
    CommMan& cm = CommMan::instance();

    int id = 1123;
    
    int rank = cm.joinComm(MPI_COMM_WORLD, 0);

    int group_rank = cm.joinComm(id, rank);

    // translate from MPI_COMM_WORLD to id should get group_rank
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(MPI_COMM_WORLD, rank, id), group_rank);
    // translate from id to MPI_COMM_WORLD should get rnak
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(id, group_rank, MPI_COMM_WORLD), rank);
    // translate any rank to MPI_COMM_SELF should get 0
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(id, group_rank, MPI_COMM_SELF), 0);
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(MPI_COMM_WORLD, rank, MPI_COMM_SELF), 0);
}
