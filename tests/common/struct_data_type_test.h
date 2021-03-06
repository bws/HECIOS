#ifndef STRUCT_DATA_TYPE_TEST_H_
#define STRUCT_DATA_TYPE_TEST_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_data_type.h"
#include "struct_data_type.h"
#include "subarray_data_type.h"
using namespace std;

/** Unit test for StructDataType */
class StructDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(StructDataTypeTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetRepresentationByteLength);
    CPPUNIT_TEST(testGetTrueExtent);
    CPPUNIT_TEST(testGetRegionsByBytes);
    CPPUNIT_TEST(testGetRegionsByCount);
    CPPUNIT_TEST(testSubarrayStruct);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testGetRepresentationByteLength();

    void testGetTrueExtent();

    void testGetRegionsByBytes();

    void testGetRegionsByCount();

    void testSubarrayStruct();

private:
    StructDataType* testStructType_;
};

void StructDataTypeTest::setUp()
{
    // Set up the array of displacements
    size_t displacements[] = {0, 10, 100, 200};

    // Set up the block lengths
    size_t blockLengths[] = {5, 2, 20, 40};

    // Set up the old types
    vector<const DataType*> types(4);
    types[0] = new ByteDataType();
    types[1] = new DoubleDataType();
    types[2] = new ByteDataType();
    types[3] = new DoubleDataType();

    testStructType_ = new StructDataType(vector<size_t>(blockLengths, blockLengths + 4),
                                         vector<size_t>(displacements, displacements + 4),
                                         types);
}

void StructDataTypeTest::tearDown()
{
}

void StructDataTypeTest::testConstructor()
{

}

void StructDataTypeTest::testGetRepresentationByteLength()
{
    CPPUNIT_ASSERT_EQUAL(size_t(16 + 16 + 16),
                         testStructType_->getRepresentationByteLength());
}

void StructDataTypeTest::testGetTrueExtent()
{
    CPPUNIT_ASSERT_EQUAL(size_t(200 + 8 * 40),
                         testStructType_->getTrueExtent());
}

void StructDataTypeTest::testGetRegionsByBytes()
{
    vector<FileRegion> regions;

    // Test getting regions on aligned boundary
    regions = testStructType_->getRegionsByBytes(0, 15);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(5), regions[0].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(10), regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(10), regions[1].extent);

    // Test getting regions on unaligned boundary
    regions = testStructType_->getRegionsByBytes(2, 15);
    CPPUNIT_ASSERT_EQUAL(FSSize(2), regions.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(2), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(3), regions[0].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(10), regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(12), regions[1].extent);

    //
    // Test 2
    //
    // Get the regions from the center of the data type
    regions = testStructType_->getRegionsByBytes(75, 20 * ByteDataType::MPI_BYTE_WIDTH +
                                                     4 * DoubleDataType::MPI_DOUBLE_WIDTH);

    // Verify that 3 regions are returned
    CPPUNIT_ASSERT_EQUAL((size_t)2, regions.size());

    // Verify they first regions begins at 10, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(100), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(20), regions[0].extent);

    // Verify they first regions begins at 2, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(200),  regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(DoubleDataType::MPI_DOUBLE_WIDTH * 4),
                         regions[1].extent);
}

void StructDataTypeTest::testGetRegionsByCount()
{
}

void StructDataTypeTest::testSubarrayStruct()
{

}

#endif /*STRUCT_DATA_TYPE_TEST_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */

