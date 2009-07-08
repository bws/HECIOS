#ifndef DIRTY_FILE_REGION_SET_TEST_H
#define DIRTY_FILE_REGION_SET_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cppunit/extensions/HelperMacros.h>
#include "dirty_file_region_set.h"

class DirtyFileRegionSetTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DirtyFileRegionSetTest);
    CPPUNIT_TEST(testNumBytes);
    CPPUNIT_TEST(testRegionSpan);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testDirtyInsert);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Pre test code */
    void setUp();

    /** Post test code */
    void tearDown();

    void testNumBytes();
    void testRegionSpan();
    void testSize();

    /** Test the basic insert functionality */
    void testInsert();

    /** Test the more complicated inserts where dirty and clean regions overlap */
    void testDirtyInsert();
};

void DirtyFileRegionSetTest::setUp()
{
}

void DirtyFileRegionSetTest::tearDown()
{
}

void DirtyFileRegionSetTest::testNumBytes()
{
    DirtyFileRegionSet set;
    CPPUNIT_ASSERT_EQUAL(size_t(0), set.numBytes());

    DirtyFileRegion f1(0, 10, true);
    set.insert(f1);
    CPPUNIT_ASSERT_EQUAL(size_t(10), set.numBytes());

    DirtyFileRegion f2(2, 10, false);
    set.insert(f2);
    CPPUNIT_ASSERT_EQUAL(size_t(12), set.numBytes());

    DirtyFileRegion f3(30, 10, true);
    set.insert(f3);
    CPPUNIT_ASSERT_EQUAL(size_t(22), set.numBytes());

}

void DirtyFileRegionSetTest::testRegionSpan()
{
    DirtyFileRegionSet set;
    CPPUNIT_ASSERT_EQUAL(size_t(0), set.regionSpan());

    DirtyFileRegion f1(0, 10, true);
    set.insert(f1);
    CPPUNIT_ASSERT_EQUAL(size_t(10), set.regionSpan());

    DirtyFileRegion f2(0, 15, false);
    set.insert(f2);
    CPPUNIT_ASSERT_EQUAL(size_t(15), set.regionSpan());

    DirtyFileRegion f3(25, 1, true);
    set.insert(f3);
    CPPUNIT_ASSERT_EQUAL(size_t(26), set.regionSpan());

}

void DirtyFileRegionSetTest::testSize()
{
    DirtyFileRegionSet set;
    CPPUNIT_ASSERT_EQUAL(size_t(0), set.size());

    DirtyFileRegion f1(10, 10, true);
    set.insert(f1);
    CPPUNIT_ASSERT_EQUAL(size_t(1), set.size());

    DirtyFileRegion f2(40, 10, true);
    set.insert(f2);
    CPPUNIT_ASSERT_EQUAL(size_t(2), set.size());

    DirtyFileRegion f3(0, 50, true);
    set.insert(f3);
    CPPUNIT_ASSERT_EQUAL(size_t(1), set.size());
}

void DirtyFileRegionSetTest::testInsert()
{
    // First test completely unique regions
    DirtyFileRegionSet set1;
    DirtyFileRegion f1(10, 1, true);
    DirtyFileRegion f2(20, 1, false);
    DirtyFileRegion f3(0, 1, true);
    DirtyFileRegion f4(30, 1, false);
    set1.insert(f1);
    set1.insert(f2);
    set1.insert(f3);
    set1.insert(f4);
    std::set<DirtyFileRegion>::const_iterator iter1= set1.begin();
    CPPUNIT_ASSERT_EQUAL(f3, *iter1++);
    CPPUNIT_ASSERT_EQUAL(f1, *iter1++);
    CPPUNIT_ASSERT_EQUAL(f2, *iter1++);
    CPPUNIT_ASSERT_EQUAL(f4, *iter1++);
    CPPUNIT_ASSERT(iter1 == set1.end());

    // Test insert that includes only a lower region
    DirtyFileRegionSet set2;
    DirtyFileRegion f5(10, 5, false);
    DirtyFileRegion f6(20, 5, false);
    DirtyFileRegion f7(12, 2, false);
    set2.insert(f5);
    set2.insert(f6);
    set2.insert(f7);
    std::set<DirtyFileRegion>::const_iterator iter2 = set2.begin();
    CPPUNIT_ASSERT_EQUAL(f5, *iter2++);
    CPPUNIT_ASSERT_EQUAL(f6, *iter2++);
    CPPUNIT_ASSERT(iter2 == set2.end());

    // Test insert that includes only an upper region
    DirtyFileRegionSet set3;
    DirtyFileRegion f8(10,5, true);
    DirtyFileRegion f9(20,5, true);
    DirtyFileRegion f10(18,2, true);
    set3.insert(f8);
    set3.insert(f9);
    set3.insert(f10);
    std::set<DirtyFileRegion>::const_iterator iter3 = set3.begin();
    CPPUNIT_ASSERT_EQUAL(f8, *iter3++);
    DirtyFileRegion temp1(18, 7, true);
    CPPUNIT_ASSERT_EQUAL(temp1, *iter3++);
    CPPUNIT_ASSERT(iter3 == set3.end());

    // Test insert that includes several upper regions
    DirtyFileRegionSet set4;
    DirtyFileRegion f11(10,5, false);
    DirtyFileRegion f12(20,5, false);
    DirtyFileRegion f13(30,5, false);
    DirtyFileRegion f14(17,20, false);
    set4.insert(f11);
    set4.insert(f12);
    set4.insert(f13);
    set4.insert(f14);
    std::set<DirtyFileRegion>::const_iterator iter4 = set4.begin();
    CPPUNIT_ASSERT_EQUAL(f11, *iter4++);
    CPPUNIT_ASSERT_EQUAL(f14, *iter4++);
    CPPUNIT_ASSERT(iter4 == set4.end());

    // Test insert that includes several lower and upper regions
    DirtyFileRegionSet set5;
    DirtyFileRegion f15(10, 5, true);
    DirtyFileRegion f16(20, 5, true);
    DirtyFileRegion f17(30, 5, true);
    DirtyFileRegion f18(40, 5, true);
    DirtyFileRegion f19(12, 50, true);
    set5.insert(f15);
    set5.insert(f16);
    set5.insert(f17);
    set5.insert(f18);
    set5.insert(f19);
    std::set<DirtyFileRegion>::const_iterator iter5 = set5.begin();
    DirtyFileRegion temp2(10, 52, true);
    CPPUNIT_ASSERT_EQUAL(temp2, *iter5++);
    CPPUNIT_ASSERT(iter5 == set5.end());

    // Test insert that includes perfectly aligned regions
    DirtyFileRegionSet set6;
    DirtyFileRegion f20(10, 5, false);
    DirtyFileRegion f21(20, 5, false);
    DirtyFileRegion f22(15, 5, false);
    set6.insert(f20);
    set6.insert(f21);
    set6.insert(f22);
    std::set<DirtyFileRegion>::const_iterator iter6 = set6.begin();
    DirtyFileRegion temp3(10, 15, false);
    CPPUNIT_ASSERT_EQUAL(size_t(1), set6.size());
    CPPUNIT_ASSERT_EQUAL(temp3, *iter6++);
    CPPUNIT_ASSERT(iter6 == set6.end());
}

void DirtyFileRegionSetTest::testDirtyInsert()
{
    // Test insert that combines dirty region into a preceding clean region
    DirtyFileRegionSet set1;
    DirtyFileRegion f1(10, 5, false);
    DirtyFileRegion f2(12, 5, true);
    set1.insert(f1);
    set1.insert(f2);
    std::set<DirtyFileRegion>::const_iterator iter1 = set1.begin();
    DirtyFileRegion temp1(10, 2, false);
    DirtyFileRegion temp2(12, 5, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), set1.size());
    CPPUNIT_ASSERT_EQUAL(temp1, *iter1++);
    CPPUNIT_ASSERT_EQUAL(temp2, *iter1++);
    CPPUNIT_ASSERT(iter1 == set1.end());

    // Test insert that combines a clean region into a preceding dirty region
    DirtyFileRegionSet set2;
    DirtyFileRegion f3(10, 5, true);
    DirtyFileRegion f4(12, 5, false);
    set2.insert(f3);
    set2.insert(f4);
    std::set<DirtyFileRegion>::const_iterator iter2 = set2.begin();
    DirtyFileRegion temp3(10, 5, true);
    DirtyFileRegion temp4(15, 2, false);
    CPPUNIT_ASSERT_EQUAL(size_t(2), set2.size());
    CPPUNIT_ASSERT_EQUAL(temp3, *iter2++);
    CPPUNIT_ASSERT_EQUAL(temp4, *iter2++);
    CPPUNIT_ASSERT(iter2 == set2.end());

    // Test insert that combines a dirty region into a succeeding clean region
    DirtyFileRegionSet set3;
    DirtyFileRegion f5(12, 5, false);
    DirtyFileRegion f6(10, 5, true);
    set3.insert(f5);
    set3.insert(f6);
    std::set<DirtyFileRegion>::const_iterator iter3 = set3.begin();
    DirtyFileRegion temp5(10, 5, true);
    DirtyFileRegion temp6(15, 2, false);
    CPPUNIT_ASSERT_EQUAL(size_t(2), set3.size());
    CPPUNIT_ASSERT_EQUAL(temp5, *iter3++);
    CPPUNIT_ASSERT_EQUAL(temp6, *iter3++);
    CPPUNIT_ASSERT(iter3 == set3.end());

    // Test insert that combines a clean region into a succeeding dirty region
    DirtyFileRegionSet set4;
    DirtyFileRegion f7(12, 5, true);
    DirtyFileRegion f8(10, 5, false);
    set4.insert(f7);
    set4.insert(f8);
    std::set<DirtyFileRegion>::const_iterator iter4 = set4.begin();
    DirtyFileRegion temp7(10, 2, false);
    DirtyFileRegion temp8(12, 5, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), set4.size());
    CPPUNIT_ASSERT_EQUAL(temp7, *iter4++);
    CPPUNIT_ASSERT_EQUAL(temp8, *iter4++);
    CPPUNIT_ASSERT(iter4 == set4.end());

    // Test insert that combines an encompassing clean region into a succeeding dirty region
    DirtyFileRegionSet set5;
    DirtyFileRegion f9(12, 5, true);
    DirtyFileRegion f10(10, 10, false);
    set5.insert(f9);
    set5.insert(f10);
    std::set<DirtyFileRegion>::const_iterator iter5 = set5.begin();
    DirtyFileRegion temp9(10, 2, false);
    DirtyFileRegion temp10(12, 5, true);
    DirtyFileRegion temp11(17, 3, false);
    CPPUNIT_ASSERT_EQUAL(size_t(3), set5.size());
    CPPUNIT_ASSERT_EQUAL(temp9, *iter5++);
    CPPUNIT_ASSERT_EQUAL(temp10, *iter5++);
    CPPUNIT_ASSERT_EQUAL(temp11, *iter5++);
    CPPUNIT_ASSERT(iter5 == set5.end());

    // Test insert that combines a encompassing dirty region into a succeeding clean region
    DirtyFileRegionSet set6;
    DirtyFileRegion f61(12, 5, false);
    DirtyFileRegion f62(10, 10, true);
    set6.insert(f61);
    set6.insert(f62);
    std::set<DirtyFileRegion>::const_iterator iter6 = set6.begin();
    CPPUNIT_ASSERT_EQUAL(size_t(1), set6.size());
    CPPUNIT_ASSERT_EQUAL(f62, *iter6++);
    CPPUNIT_ASSERT(iter6 == set6.end());
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
