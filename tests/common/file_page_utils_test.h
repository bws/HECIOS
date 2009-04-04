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

#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "file_page_utils.h"
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
    CPPUNIT_TEST(testDetermineRequestPartialPageRegions);
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

    void testDetermineRequestPartialPageRegions();

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
    CPPUNIT_FAIL("fail");
}

void FilePageUtilsTest::testDetermineRequestFullPages()
{
    CPPUNIT_FAIL("fail");
}

void FilePageUtilsTest::testDetermineRequestPartialPages()
{
    CPPUNIT_FAIL("fail");
}

void FilePageUtilsTest::testDetermineRequestPartialPageRegions()
{
    CPPUNIT_FAIL("fail");
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
