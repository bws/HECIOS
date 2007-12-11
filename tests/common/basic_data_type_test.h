#ifndef BASIC_DATA_TYPE_TEST_H
#define BASIC_DATA_TYPE_TEST_H
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

#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_data_type.h"
using namespace std;

/** Unit test for BasicDataType */
class BasicDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BasicDataTypeTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetRepresentationByteLength);
    CPPUNIT_TEST(testGetRegions);
    CPPUNIT_TEST_SUITE_END();
    
public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testConstructor();

    void testGetRepresentationByteLength();

    void testGetRegions();
    
private:
};

void BasicDataTypeTest::setUp()
{
}

void BasicDataTypeTest::tearDown()
{
}

void BasicDataTypeTest::testConstructor()
{
    BasicDataType(1);
}

void BasicDataTypeTest::testGetRepresentationByteLength()
{
    BasicDataType dt1(1);
    CPPUNIT_ASSERT_EQUAL((size_t)4, dt1.getRepresentationByteLength());        

    BasicDataType dt2(2);
    CPPUNIT_ASSERT_EQUAL((size_t)4, dt2.getRepresentationByteLength());        

    BasicDataType dt3(16);
    CPPUNIT_ASSERT_EQUAL((size_t)4, dt3.getRepresentationByteLength());
}

void BasicDataTypeTest::testGetRegions()
{
    BasicDataType dt1(BasicDataType::MPI_DOUBLE_WIDTH);
    vector<FileRegion> regions = dt1.getRegions(4, 4);
    CPPUNIT_ASSERT_EQUAL((size_t)1, regions.size());
    FileRegion fr1 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)4, fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)4 * BasicDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);
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
