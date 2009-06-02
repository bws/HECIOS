#ifndef SUBARRAY_DATA_TYPE_TEST_H_
#define SUBARRAY_DATA_TYPE_TEST_H_
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
#include "contiguous_data_type.h"
#include "subarray_data_type.h"
using namespace std;

/** Unit test for SubarrayDataType */
class SubarrayDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(SubarrayDataTypeTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetRepresentationByteLength);
    CPPUNIT_TEST(testGetRegionsByBytes);
    CPPUNIT_TEST(testGetRegionsByCount);
    CPPUNIT_TEST(testTileIOExample);
    CPPUNIT_TEST(testTileIOIteration);
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

    void testTileIOExample();

    void testTileIOIteration();

private:

    SubarrayDataType* cOrderType_;
    SubarrayDataType* fOrderType_;
};

void SubarrayDataTypeTest::setUp()
{
    // Build 2 subarray types that are exactly inverted for easier testing

    // 12x7x9x4
    size_t cOrderSizes[] = {12,7,9,4};
    // Subarray of size 6x2x4x2
    size_t cOrderSubSizes[] = {6,2,4,2};
    // Starting indices: 2,3,4,1
    size_t cOrderStarts[] = {2,3,4,1};

    // Construct C Order/Row major Subarray
    cOrderType_ = new SubarrayDataType(vector<size_t>(cOrderSizes, cOrderSizes+4),
                                       vector<size_t>(cOrderSubSizes, cOrderSubSizes+4),
                                       vector<size_t>(cOrderStarts, cOrderStarts+4),
                                       SubarrayDataType::C_ORDER,
                                       *(new ByteDataType()));

    // 4x9x7x12
    size_t fOrderSizes[] = {4,9,7,12};

    size_t fOrderSubSizes[] = {2,4,2,6};

    size_t fOrderStarts[] = {1,4,3,2};

    // Construct Fortran Order/Column Major subarray
    fOrderType_ = new SubarrayDataType(vector<size_t>(fOrderSizes, fOrderSizes+4),
                                       vector<size_t>(fOrderSubSizes, fOrderSubSizes+4),
                                       vector<size_t>(fOrderStarts, fOrderStarts+4),
                                       SubarrayDataType::FORTRAN_ORDER,
                                       *(new ByteDataType()));
}

void SubarrayDataTypeTest::tearDown()
{
    delete cOrderType_;
    delete fOrderType_;
}

void SubarrayDataTypeTest::testConstructor()
{
    vector<size_t> sizes;
    vector<size_t> subSizes;
    vector<size_t> starts;
    ByteDataType dt1;

    // Check construction with an empty subarray
    SubarrayDataType dt2(sizes, subSizes, starts, SubarrayDataType::C_ORDER, dt1);

    // Check construction with an empty subarray
    sizes.push_back(4);
    subSizes.push_back(2);
    starts.push_back(0);
    SubarrayDataType dt3(sizes, subSizes, starts, SubarrayDataType::C_ORDER, dt1);

    // Check construction with an empty subarray
    SubarrayDataType dt4(sizes, subSizes, starts, SubarrayDataType::C_ORDER, dt1);
}

void SubarrayDataTypeTest::testGetRepresentationByteLength()
{
}

void SubarrayDataTypeTest::testGetRegionsByBytes()
{
    vector<FileRegion> regions;

    //
    // Test 1
    //
    // Retrieve Regions from the beginning of the C Order data type
    regions = cOrderType_->getRegionsByBytes(0, 25);

    CPPUNIT_ASSERT_EQUAL(size_t(13), regions.size());

    CPPUNIT_ASSERT_EQUAL(FSOffset(629), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[0].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(633), regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[1].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(637), regions[2].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[2].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(641), regions[3].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[3].extent);

    CPPUNIT_ASSERT_EQUAL(FSOffset(665), regions[4].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[4].extent);

    //
    // Test 2
    //
    // Retrieve Regions from the beginning of the Fortran Order data type
    regions = fOrderType_->getRegionsByBytes(0, 25);

    CPPUNIT_ASSERT_EQUAL(size_t(13), regions.size());

    CPPUNIT_ASSERT_EQUAL(FSOffset(629), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[0].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(633), regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[1].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(637), regions[2].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[2].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(641), regions[3].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[3].extent);

    CPPUNIT_ASSERT_EQUAL(FSOffset(665), regions[4].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions[4].extent);


}

void SubarrayDataTypeTest::testGetRegionsByCount()
{
}

void SubarrayDataTypeTest::testTileIOExample()
{
    // Base type
    ByteDataType byteType;
    ContiguousDataType elementType(8, byteType);

    // Tile Subarray processor 0
    size_t sizes[] = {1000, 80};
    size_t subSizes[] = {1000, 10};
    size_t starts[] = {0, 0};
    SubarrayDataType tileType0(vector<size_t>(sizes, sizes + 2),
                               vector<size_t>(subSizes, subSizes + 2),
                               vector<size_t>(starts, starts + 2),
                               SubarrayDataType::C_ORDER,
                               elementType);

    // Check the correctness of the regions
    vector<FileRegion> regions = tileType0.getRegionsByBytes(0, 80000);
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions.size());
    for (size_t i = 0; i < regions.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(0 + i*640), regions[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions[i].extent);
    }

    // Tile Subarray processor 3
    size_t sizes3[] = {1000, 80};
    size_t subSizes3[] = {1000, 10};
    size_t starts3[] = {0, 30};
    SubarrayDataType tileType3(vector<size_t>(sizes3, sizes3 + 2),
                               vector<size_t>(subSizes3, subSizes3 + 2),
                               vector<size_t>(starts3, starts3 + 2),
                               SubarrayDataType::C_ORDER,
                               elementType);

    // Check the correctness of the regions
    vector<FileRegion> regions3 = tileType3.getRegionsByBytes(0, 80000);
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions3.size());
    for (size_t i = 0; i < regions3.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(240 + i*640), regions3[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions3[i].extent);
    }
}

void SubarrayDataTypeTest::testTileIOIteration()
{
    // Base type
    ByteDataType byteType;
    ContiguousDataType elementType(8, byteType);

    // Tile Subarray processor 0
    size_t sizes[] = {1000, 80};
    size_t subSizes[] = {1000, 10};
    size_t starts[] = {0, 0};
    SubarrayDataType tileType0(vector<size_t>(sizes, sizes + 2),
                               vector<size_t>(subSizes, subSizes + 2),
                               vector<size_t>(starts, starts + 2),
                               SubarrayDataType::C_ORDER,
                               elementType);

    // Check the correctness of the regions
    vector<FileRegion> regions = tileType0.getRegionsByBytes(0, 80000);
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions.size());
    for (size_t i = 0; i < regions.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(0 + i*640), regions[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions[i].extent);
    }

    // Now get the second set of regions
    regions = tileType0.getRegionsByBytes(80000, 800);
    CPPUNIT_ASSERT_EQUAL(size_t(10), regions.size());
    for (size_t i = 0; i < regions.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(640000 + i*640), regions[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions[i].extent);
    }

    // Tile Sub-array processor 3
    size_t sizes3[] = {1000, 80};
    size_t subSizes3[] = {1000, 10};
    size_t starts3[] = {0, 30};
    SubarrayDataType tileType3(vector<size_t>(sizes3, sizes3 + 2),
                               vector<size_t>(subSizes3, subSizes3 + 2),
                               vector<size_t>(starts3, starts3 + 2),
                               SubarrayDataType::C_ORDER,
                               elementType);

    // Check the correctness of the regions
    vector<FileRegion> regions3 = tileType3.getRegionsByBytes(0, 80000);
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions3.size());
    for (size_t i = 0; i < regions3.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(240 + i*640), regions3[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions3[i].extent);
    }

    // Now get the second set of regions
    regions3 = tileType3.getRegionsByBytes(80000, 800);
    CPPUNIT_ASSERT_EQUAL(size_t(10), regions3.size());
    for (size_t i = 0; i < regions3.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(640240 + i*640), regions3[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions3[i].extent);
    }

    // Now get the third set of regions
    regions3 = tileType3.getRegionsByBytes(160000, 800);
    CPPUNIT_ASSERT_EQUAL(size_t(10), regions3.size());
    for (size_t i = 0; i < regions3.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(1280240 + i*640), regions3[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), regions3[i].extent);
    }

}

#endif /*SUBARRAY_DATA_TYPE_TEST_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */

