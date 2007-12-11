#ifndef VECTOR_DATA_TYPE_TEST_H
#define VECTOR_DATA_TYPE_TEST_H
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
#include "vector_data_type.h"
using namespace std;

/** Unit test for VectorDataType */
class VectorDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(VectorDataTypeTest);
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

void VectorDataTypeTest::setUp()
{
}

void VectorDataTypeTest::tearDown()
{
}

void VectorDataTypeTest::testConstructor()
{
    BasicDataType dt1(1);
    VectorDataType dt2(3, 2, 3, dt1);
    VectorDataType dt3(3, 2, 1, dt1);
    VectorDataType dt4(3, 2, -3, dt1);
}

void VectorDataTypeTest::testGetRepresentationByteLength()
{
    BasicDataType bdt1(1);
    VectorDataType vdt1(3, 2, 3, bdt1);
    CPPUNIT_ASSERT_EQUAL((size_t)12 + bdt1.getRepresentationByteLength(),
                         vdt1.getRepresentationByteLength());
}

void VectorDataTypeTest::testGetRegions()
{
    // Test a simple scenario
    BasicDataType bdt1(BasicDataType::MPI_DOUBLE_WIDTH);
    VectorDataType vdt1(3,2,3, bdt1);
    vector<FileRegion> regions = vdt1.getRegions(0, 1);
    CPPUNIT_ASSERT_EQUAL((size_t)3, regions.size());

    for (size_t i = 0; i < regions.size(); i++)
    {
        cerr << regions[i].offset << " " << regions[i].extent << endl;
    }
    
    FileRegion fr0 = regions[0];
    //CPPUNIT_ASSERT_EQUAL((FSOffset)0, fr0.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * BasicDataType::MPI_DOUBLE_WIDTH,
                         fr0.extent);

    FileRegion fr1 = regions[1];
    CPPUNIT_ASSERT_EQUAL((FSOffset)3 * BasicDataType::MPI_DOUBLE_WIDTH,
                         fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * BasicDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);
    
    FileRegion fr2 = regions[2];
    CPPUNIT_ASSERT_EQUAL((FSOffset)6 * BasicDataType::MPI_DOUBLE_WIDTH,
                         fr2.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * BasicDataType::MPI_DOUBLE_WIDTH,
                         fr2.extent);
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
