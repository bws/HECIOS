//
// This file is part of Hecios
//
// Copyright (C) 2008 Yang Wu, Brad Settlemyer
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
#include <cppunit/extensions/HelperMacros.h>
#include "comm_man.h"

class CommManTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommManTest);
    CPPUNIT_TEST(testCommSize);
    CPPUNIT_TEST(testJoinComm);
    CPPUNIT_TEST(testCommRank);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Pre test code */
    void setUp();

    /** Post test code */
    void tearDown();

    void testCommSize();
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
}

void CommManTest::testJoinComm()
{
    CommMan& cm = CommMan::instance();

    // now test for a new communicator
    int id = 3;
    
    // 1st node join, rank should be 0
    CPPUNIT_ASSERT_EQUAL(cm.joinComm(id, 1), 0);
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

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
