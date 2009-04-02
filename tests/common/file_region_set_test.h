//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include "file_region_set.h"

class FileRegionSetTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FileRegionSetTest);
    CPPUNIT_TEST(testNumBytes);
    CPPUNIT_TEST(testRegionSpan);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Pre test code */
    void setUp();

    /** Post test code */
    void tearDown();

    void testNumBytes();
    void testRegionSpan();
    void testSize();
    void testInsert();
};

void FileRegionSetTest::setUp()
{
}

void FileRegionSetTest::tearDown()
{
}

void FileRegionSetTest::testNumBytes()
{
    FileRegionSet set;
    CPPUNIT_ASSERT_EQUAL(size_t(0), set.numBytes());

    FileRegion f1 = {0, 10};
    set.insert(f1);
    CPPUNIT_ASSERT_EQUAL(size_t(10), set.numBytes());

    FileRegion f2 = {2, 10};
    set.insert(f2);
    CPPUNIT_ASSERT_EQUAL(size_t(12), set.numBytes());

    FileRegion f3 = {30, 10};
    set.insert(f3);
    CPPUNIT_ASSERT_EQUAL(size_t(22), set.numBytes());

}

void FileRegionSetTest::testRegionSpan()
{
    FileRegionSet set;
    CPPUNIT_ASSERT_EQUAL(size_t(0), set.regionSpan());

    FileRegion f1 = {0, 10};
    set.insert(f1);
    CPPUNIT_ASSERT_EQUAL(size_t(10), set.regionSpan());

    FileRegion f2 = {0, 15};
    set.insert(f2);
    CPPUNIT_ASSERT_EQUAL(size_t(15), set.regionSpan());

    FileRegion f3 = {25, 1};
    set.insert(f3);
    CPPUNIT_ASSERT_EQUAL(size_t(26), set.regionSpan());

}

void FileRegionSetTest::testSize()
{
    FileRegionSet set;
    CPPUNIT_ASSERT_EQUAL(size_t(0), set.size());

    FileRegion f1 = {10, 10};
    set.insert(f1);
    CPPUNIT_ASSERT_EQUAL(size_t(1), set.size());

    FileRegion f2 = {40, 10};
    set.insert(f2);
    CPPUNIT_ASSERT_EQUAL(size_t(2), set.size());

    FileRegion f3 = {0, 50};
    set.insert(f3);
    CPPUNIT_ASSERT_EQUAL(size_t(1), set.size());
}

void FileRegionSetTest::testInsert()
{
    // First test completely unique regions
    FileRegionSet set1;
    FileRegion f1 = {10,1};
    FileRegion f2 = {20,1};
    FileRegion f3 = {0,1};
    FileRegion f4 = {30,1};
    set1.insert(f1);
    set1.insert(f2);
    set1.insert(f3);
    set1.insert(f4);
    std::set<FileRegion>::const_iterator iter1= set1.begin();
    CPPUNIT_ASSERT_EQUAL(f3, *iter1++);
    CPPUNIT_ASSERT_EQUAL(f1, *iter1++);
    CPPUNIT_ASSERT_EQUAL(f2, *iter1++);
    CPPUNIT_ASSERT_EQUAL(f4, *iter1++);
    CPPUNIT_ASSERT(iter1 == set1.end());

    // Test insert that includes only a lower region
    FileRegionSet set2;
    FileRegion f5 = {10,5};
    FileRegion f6 = {20,5};
    FileRegion f7 = {12,2};
    set2.insert(f5);
    set2.insert(f6);
    set2.insert(f7);
    std::set<FileRegion>::const_iterator iter2 = set2.begin();
    CPPUNIT_ASSERT_EQUAL(f5, *iter2++);
    CPPUNIT_ASSERT_EQUAL(f6, *iter2++);
    CPPUNIT_ASSERT(iter2 == set2.end());

    // Test insert that includes only an upper region
    FileRegionSet set3;
    FileRegion f8 = {10,5};
    FileRegion f9 = {20,5};
    FileRegion f10 = {18,2};
    set3.insert(f8);
    set3.insert(f9);
    set3.insert(f10);
    std::set<FileRegion>::const_iterator iter3 = set3.begin();
    CPPUNIT_ASSERT_EQUAL(f8, *iter3++);
    FileRegion temp1;
    temp1.offset = 18; temp1.extent = 7;
    CPPUNIT_ASSERT_EQUAL(temp1, *iter3++);
    CPPUNIT_ASSERT(iter3 == set3.end());

    // Test insert that includes several upper regions
    FileRegionSet set4;
    FileRegion f11 = {10,5};
    FileRegion f12 = {20,5};
    FileRegion f13 = {30,5};
    FileRegion f14 = {17,20};
    set4.insert(f11);
    set4.insert(f12);
    set4.insert(f13);
    set4.insert(f14);
    std::set<FileRegion>::const_iterator iter4 = set4.begin();
    CPPUNIT_ASSERT_EQUAL(f11, *iter4++);
    CPPUNIT_ASSERT_EQUAL(f14, *iter4++);
    CPPUNIT_ASSERT(iter4 == set4.end());

    // Test insert that includes several lower and upper regions
    FileRegionSet set5;
    FileRegion f15 = {10,5};
    FileRegion f16 = {20,5};
    FileRegion f17 = {30,5};
    FileRegion f18 = {40,5};
    FileRegion f19 = {12, 50};
    set5.insert(f15);
    set5.insert(f16);
    set5.insert(f17);
    set5.insert(f18);
    set5.insert(f19);
    std::set<FileRegion>::const_iterator iter5 = set5.begin();
    FileRegion temp2;
    temp2.offset = 10; temp2.extent = 52;
    CPPUNIT_ASSERT_EQUAL(temp2, *iter5++);
    CPPUNIT_ASSERT(iter5 == set5.end());
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
