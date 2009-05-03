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
#include "file_page_utils.h"
#include "file_view.h"
#include "pfs_types.h"
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
