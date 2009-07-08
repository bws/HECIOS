#ifndef FILE_VIEW_TEST_H
#define FILE_VIEW_TEST_H
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
#include "file_view.h"

/** Unit test for FileView */
class FileViewTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FileViewTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testAssignment);
    CPPUNIT_TEST(testGetDisplacement);
    CPPUNIT_TEST(testGetDataType);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testCopyConstructor();

    void testAssignment();

    void testGetDisplacement();

    void testGetDataType();
};

void FileViewTest::setUp()
{
}

void FileViewTest::tearDown()
{
}

void FileViewTest::testConstructor()
{
    BasicDataType<13>* dataType = new BasicDataType<13>();
    FileView view1(12, dataType);
    CPPUNIT_ASSERT_EQUAL(FSOffset(12), view1.getDisplacement());
    CPPUNIT_ASSERT(dataType == view1.getDataType());
}

void FileViewTest::testCopyConstructor()
{
    BasicDataType<10>* dataType = new BasicDataType<10>();
    FileView view1(6, dataType);
    FileView view2(view1);
    CPPUNIT_ASSERT_EQUAL(view1.getDisplacement(), view2.getDisplacement());
    //CPPUNIT_ASSERT_EQUAL(view1.getDataType(), view2.getDataType());
}

void FileViewTest::testAssignment()
{
    BasicDataType<10>* dataType1 = new BasicDataType<10>();
    FileView view1(6, dataType1);

    BasicDataType<33>* dataType2 = new BasicDataType<33>();
    FileView view2(11, dataType2);

    view2 = view1;
    CPPUNIT_ASSERT_EQUAL(view1.getDisplacement(), view2.getDisplacement());
    //CPPUNIT_ASSERT_EQUAL(view1.getDataType(), view2.getDataType());
}

void FileViewTest::testGetDisplacement()
{
    BasicDataType<1>* dataType = new BasicDataType<1>();
    FileView view1(811, dataType);
    CPPUNIT_ASSERT_EQUAL(FSOffset(811), view1.getDisplacement());
}

void FileViewTest::testGetDataType()
{
    BasicDataType<43>* dataType = new BasicDataType<43>();
    FileView view1(0, dataType);
    CPPUNIT_ASSERT(dataType == view1.getDataType());
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
