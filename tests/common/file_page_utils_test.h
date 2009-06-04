#ifndef FILE_PAGE_UTILS_TEST_H
#define FILE_PAGE_UTILS_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
#include <set>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_data_type.h"
#include "contiguous_data_type.h"
#include "file_page_utils.h"
#include "file_view.h"
#include "pfs_types.h"
#include "subarray_data_type.h"
using namespace std;

/** Unit test for FilePageUtils */
class FilePageUtilsTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FilePageUtilsTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testPageBeginOffset);
    CPPUNIT_TEST(testDetermineRequestPages);
    CPPUNIT_TEST(testDetermineRequestFullPages);
    CPPUNIT_TEST(testDetermineRequestPartialPages);
    CPPUNIT_TEST(testDeterminePartialPageRegions);
    CPPUNIT_TEST(testTileIOPageRegions);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testPageBeginOffset();

    void testDetermineRequestPages();

    void testDetermineRequestFullPages();

    void testDetermineRequestPartialPages();

    void testDeterminePartialPageRegions();

    void testTileIOPageRegions();

private:
};

void FilePageUtilsTest::setUp()
{
}

void FilePageUtilsTest::tearDown()
{
    FilePageUtils::clearState();
}

void FilePageUtilsTest::testConstructor()
{
    //FilePageUtils& instance = FilePageUtils::instance();
}

void FilePageUtilsTest::testPageBeginOffset()
{
    FilePageUtils& utils = FilePageUtils::instance();

    CPPUNIT_ASSERT_EQUAL(FSOffset(0), utils.pageBeginOffset(10, 0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), utils.pageBeginOffset(37, 0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(408), utils.pageBeginOffset(2, 204));
}

void FilePageUtilsTest::testDetermineRequestPages()
{
    FilePageUtils& pageUtils = FilePageUtils::instance();

    // Test condition that is failing at runtime
    FSSize pageSize = 1024;
    FSOffset offset = 34612;
    FSSize size = 1960;
    FileView view(0, new ByteDataType());
    set<FilePageId> pageIds = pageUtils.determineRequestPages(pageSize,
                                                              offset,
                                                              size,
                                                              view);
    set<FilePageId>::const_iterator iter = pageIds.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(3), pageIds.size());
    CPPUNIT_ASSERT_EQUAL(size_t(33), *(iter++));
    CPPUNIT_ASSERT_EQUAL(size_t(34), *(iter++));
    CPPUNIT_ASSERT_EQUAL(size_t(35), *(iter++));
}

void FilePageUtilsTest::testDetermineRequestFullPages()
{
    FilePageUtils& pageUtils = FilePageUtils::instance();

    // Test condition that is failing at runtime
    FSSize pageSize = 1024;
    FSOffset offset = 34612;
    FSSize size = 1960;
    FileView view(0, new ByteDataType());
    set<FilePageId> pageIds = pageUtils.determineRequestFullPages(pageSize,
                                                                  offset,
                                                                  size,
                                                                  view);
    set<FilePageId>::const_iterator iter = pageIds.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), pageIds.size());
    CPPUNIT_ASSERT_EQUAL(size_t(34), *(iter++));
}

void FilePageUtilsTest::testDetermineRequestPartialPages()
{
    FilePageUtils& pageUtils = FilePageUtils::instance();

    // Test condition that is failing at runtime
    FSSize pageSize = 1024;
    FSOffset offset = 34612;
    FSSize size = 1960;
    FileView view(0, new ByteDataType());
    set<FilePageId> pageIds = pageUtils.determineRequestPartialPages(pageSize,
                                                                     offset,
                                                                     size,
                                                                     view);
    set<FilePageId>::const_iterator iter = pageIds.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(2), pageIds.size());
    CPPUNIT_ASSERT_EQUAL(size_t(33), *(iter++));
    CPPUNIT_ASSERT_EQUAL(size_t(35), *(iter++));
}

void FilePageUtilsTest::testDeterminePartialPageRegions()
{
    FilePageUtils& pageUtils = FilePageUtils::instance();

    // Test condition that is failing at runtime
    FSSize pageSize = 1024;
    FSOffset offset = 34612;
    FSSize size = 1960;
    FileView view(0, new ByteDataType());

    // Regions for page 33
    FileRegionSet regionSet1 = pageUtils.determinePartialPageRegions(pageSize,
                                                                     33,
                                                                     offset,
                                                                     size,
                                                                     view);
    FileRegionSet::const_iterator iter1 = regionSet1.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), regionSet1.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(34612), iter1->offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(204), iter1->extent);

    // Regions for page 34
    FileRegionSet regionSet2 = pageUtils.determinePartialPageRegions(pageSize,
                                                                     34,
                                                                     offset,
                                                                     size,
                                                                     view);
    FileRegionSet::const_iterator iter2 = regionSet2.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), regionSet2.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(34816), iter2->offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1024), iter2->extent);

    // Regions for page 35
    FileRegionSet regionSet3 = pageUtils.determinePartialPageRegions(pageSize,
                                                                     35,
                                                                     offset,
                                                                     size,
                                                                     view);
    FileRegionSet::const_iterator iter3 = regionSet3.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), regionSet3.size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(35840), iter3->offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(732), iter3->extent);
}

void FilePageUtilsTest::testTileIOPageRegions()
{
    // Base type
    ByteDataType byteType;
    ContiguousDataType elementType(8, byteType);
    FilePageUtils& pageUtils = FilePageUtils::instance();

    // Tile Subarray processor 0
    size_t sizes[] = {1000, 5120};
    size_t subSizes[] = {1000, 10};
    size_t starts[] = {0, 0};
    SubarrayDataType* tileType0 = new SubarrayDataType(vector<size_t>(sizes, sizes + 2),
                                                       vector<size_t>(subSizes, subSizes + 2),
                                                       vector<size_t>(starts, starts + 2),
                                                       SubarrayDataType::C_ORDER,
                                                       elementType);
    FileView view0(0, tileType0);

    set<FilePageId> pages = pageUtils.determineRequestPages(4096,
                                                            0,
                                                            80000,
                                                            view0);
    CPPUNIT_ASSERT_EQUAL(size_t(1000), pages.size());
    set<FilePageId>::const_iterator first = pages.begin();
    set<FilePageId>::const_iterator last = pages.end();
    size_t counter = 0;
    while (first != last)
    {
        CPPUNIT_ASSERT_EQUAL(counter*10, *first);
        first++;
        counter++;
    }

    // Now retrieve the partial regions for each page
    first = pages.begin();
    last = pages.end();
    counter = 0;
    while (first != last)
    {
        FileRegionSet frs = pageUtils.determinePartialPageRegions(4096,
                                                              *first,
                                                              0,
                                                              80000,
                                                              view0);
        CPPUNIT_ASSERT_EQUAL(size_t(1), frs.size());
        CPPUNIT_ASSERT_EQUAL(FSOffset(4096 * counter), frs.begin()->offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(80), frs.begin()->extent);
        first++;
        counter++;
    }

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
