#ifndef STRUCT_DATA_TYPE_TEST_H_
#define STRUCT_DATA_TYPE_TEST_H_
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

#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_data_type.h"
#include "struct_data_type.h"
using namespace std;

/** Unit test for StructDataType */
class StructDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(StructDataTypeTest);
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

void StructDataTypeTest::setUp()
{
}

void StructDataTypeTest::tearDown()
{
}

void StructDataTypeTest::testConstructor()
{

}

void StructDataTypeTest::testGetRepresentationByteLength()
{

}

void StructDataTypeTest::testGetRegionsByBytes()
{
}

void StructDataTypeTest::testGetRegionsByCount()
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

