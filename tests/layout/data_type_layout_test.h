#ifndef DATA_TYPE_LAYOUT_TEST_H
#define DATA_TYPE_LAYOUT_TEST_H
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

#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "data_type_layout.h"
using namespace std;

/** Unit test for DataTypeLayout */
class DataTypeLayoutTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(DataTypeLayoutTest);
    CPPUNIT_TEST(testGetRegion);
    CPPUNIT_TEST(testGetRegions);
    CPPUNIT_TEST(testGetLength);
    CPPUNIT_TEST(testGetSubRegions);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testGetRegion();
    void testGetRegions();
    void testGetLength();
    void testGetSubRegions();

private:
    vector<FSOffset> offsets_;
    vector<FSSize> extents_;
};

void DataTypeLayoutTest::setUp()
{
    offsets_.push_back(12);
    extents_.push_back(10);
    
    offsets_.push_back(130);
    extents_.push_back(20);
    
    offsets_.push_back(150);
    extents_.push_back(40);
}

void DataTypeLayoutTest::tearDown()
{
    offsets_.clear();
    extents_.clear();
}

void DataTypeLayoutTest::testGetRegion()
{
    DataTypeLayout dtl(offsets_, extents_);

    CPPUNIT_ASSERT_EQUAL(FSOffset(12), dtl.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(10), dtl.getRegion(0).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(150), dtl.getRegion(2).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(40), dtl.getRegion(2).extent);
}

void DataTypeLayoutTest::testGetRegions()
{
    DataTypeLayout dtl(offsets_, extents_);
    vector<FileRegion> regions = dtl.getRegions();

    CPPUNIT_ASSERT_EQUAL((size_t)3, regions.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(12), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(10), regions[0].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(150), regions[2].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(40), regions[2].extent);
}

void DataTypeLayoutTest::testGetLength()
{
    DataTypeLayout dtl(offsets_, extents_);
    CPPUNIT_ASSERT_EQUAL(FSSize(70), dtl.getLength());
}

void DataTypeLayoutTest::testGetSubRegions()
{
    DataTypeLayout dtl(offsets_, extents_);
    vector<FileRegion> subRegions;
    
    // Test a subregion that falls on a region boundary
    subRegions = dtl.getSubRegions(10, 20);
    CPPUNIT_ASSERT_EQUAL((size_t)1, subRegions.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(130), subRegions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(20), subRegions[0].extent);

    // Test a subregion that begins at 0
    subRegions = dtl.getSubRegions(0, 20);
    CPPUNIT_ASSERT_EQUAL((size_t)2, subRegions.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(12), subRegions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(10), subRegions[0].extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(130), subRegions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(10), subRegions[1].extent);

    // Test a bug exhbiting in some reads in the simulator
    DataTypeLayout layout2(0, 65536);
    subRegions = layout2.getSubRegions(4096, 4096);
    CPPUNIT_ASSERT_EQUAL((size_t)1, subRegions.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(4096), subRegions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(4096), subRegions[0].extent);
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
