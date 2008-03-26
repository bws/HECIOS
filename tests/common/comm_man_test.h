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
    CommMan& cm = *CommMan::getInstance();
    CPPUNIT_ASSERT_EQUAL(cm.commSize(MPI_COMM_WORLD), 0);
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(MPI_COMM_WORLD, 0), 0);
    CPPUNIT_ASSERT_EQUAL(cm.commSize(MPI_COMM_WORLD), 1);
    CPPUNIT_ASSERT_EQUAL(cm.commSize(MPI_COMM_SELF), 1);
}

void CommManTest::testCommRank()
{
    CommMan& cm = *CommMan::getInstance();
    int rank = cm.joinComm(MPI_COMM_WORLD, 0);

    int grank = cm.joinComm(1, rank);

    CPPUNIT_ASSERT_EQUAL(cm.commRank(1, rank), grank);
    CPPUNIT_ASSERT_EQUAL(cm.commRank(MPI_COMM_SELF, rank), 0);
}

void CommManTest::testCommTrans()
{
    CommMan& cm = *CommMan::getInstance();
    int rank = cm.joinComm(MPI_COMM_WORLD, 0);

    int grank = cm.joinComm(1, rank);

    CPPUNIT_ASSERT_EQUAL(cm.commTrans(MPI_COMM_WORLD, rank, 1), grank);
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(MPI_COMM_SELF, 0, 1), -1);
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(1, grank, MPI_COMM_WORLD), rank);
    CPPUNIT_ASSERT_EQUAL(cm.commTrans(1, grank, MPI_COMM_SELF), 0);
}
