#ifndef INDEXED_DATA_TYPE_TEST_H
#define INDEXED_DATA_TYPE_TEST_H
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
#include "indexed_data_type.h"
using namespace std;

/** Unit test for IndexedDataType */
class IndexedDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(IndexedDataTypeTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetRepresentationByteLength);
    CPPUNIT_TEST(testGetRegionsByBytes);
    CPPUNIT_TEST(testGetRegionsByCount);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testGetRepresentationByteLength();

    void testGetRegionsByBytes();

    void testGetRegionsByCount();

private:
};

void IndexedDataTypeTest::setUp()
{
}

void IndexedDataTypeTest::tearDown()
{
}

void IndexedDataTypeTest::testConstructor()
{
    size_t blockLengths[] = {1, 2, 3, 4};
    size_t displacements[] = {4, 8, 12, 16};
    ByteDataType dt1;
    IndexedDataType dt2(vector<size_t>(blockLengths, blockLengths + 4),
                        vector<size_t>(displacements, displacements + 4),
                        dt1);
}

void IndexedDataTypeTest::testGetRepresentationByteLength()
{
    size_t blockLengths[] = {1, 2, 3, 4};
    size_t displacements[] = {10, 20, 30, 40};
    ByteDataType dt1;
    IndexedDataType dt2(vector<size_t>(blockLengths, blockLengths + 4),
                        vector<size_t>(displacements, displacements + 4),
                        dt1);
    CPPUNIT_ASSERT_EQUAL(size_t(4) + 2 * size_t(16) +
                         dt1.getRepresentationByteLength(),
                         dt2.getRepresentationByteLength());
}

void IndexedDataTypeTest::testGetRegionsByBytes()
{
    // Test a simple scenario
    size_t blockLengths[] = {1, 2, 3, 4};
    size_t displacements[] = {10, 20, 30, 40};
    DoubleDataType dt1;
    IndexedDataType idt1(vector<size_t>(blockLengths, blockLengths + 4),
                         vector<size_t>(displacements, displacements + 4),
                         dt1);

    // Get the regions from the beginning
    vector<FileRegion> regions =
        idt1.getRegionsByBytes(0, DoubleDataType::MPI_DOUBLE_WIDTH * 10);

    // Verify that all regions are returned
    CPPUNIT_ASSERT_EQUAL((size_t)4, regions.size());

    // Verify that the 1st region begins at 10 and extends for 1
    FileRegion fr0 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(10 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr0.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)1 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr0.extent);

    // Verify that the 2nd region begins at 20 and extends for 2
    FileRegion fr1 = regions[1];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(20 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);

    // Verify that the 3rd region begins at 30 and extends for 3
    FileRegion fr2 = regions[2];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(30 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr2.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)3 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr2.extent);

    //
    // Test 2
    //
    // Get the regions from the center of the data type
    regions = idt1.getRegionsByBytes(2, 4 * DoubleDataType::MPI_DOUBLE_WIDTH);

    // Verify that 3 regions are returned
    CPPUNIT_ASSERT_EQUAL((size_t)3, regions.size());

    // Verify they first regions begins at 10, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(10 * DoubleDataType::MPI_DOUBLE_WIDTH) + 2,
                         regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(DoubleDataType::MPI_DOUBLE_WIDTH) - 2,
                         regions[0].extent);

    // Verify they first regions begins at 2, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(20 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(DoubleDataType::MPI_DOUBLE_WIDTH * 2),
                         regions[1].extent);

    // Verify they first regions begins at 2, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(30 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         regions[2].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1 * DoubleDataType::MPI_DOUBLE_WIDTH) + 2,
                         regions[2].extent);
}

void IndexedDataTypeTest::testGetRegionsByCount()
{
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
