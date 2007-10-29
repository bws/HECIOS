#ifndef DATA_TYPE_PROCESSOR_TEST_H
#define DATA_TYPE_PROCESSOR_TEST_H
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
#include "data_type_processor.h"
#include "basic_types.h"
#include "data_type_layout.h"
#include "simple_stripe_distribution.h"
using namespace std;

/** Unit test for DataTypeProcessor */
class DataTypeProcessorTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(DataTypeProcessorTest);
    CPPUNIT_TEST(testCreateFileLayoutForClient);
    CPPUNIT_TEST(testCreateFileLayoutForServer);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testCreateFileLayoutForClient();
    void testCreateFileLayoutForServer();
};

void DataTypeProcessorTest::setUp()
{
}

void DataTypeProcessorTest::tearDown()
{
}

void DataTypeProcessorTest::testCreateFileLayoutForClient()
{
    // Test with count 1, offset 0, data type 0
    DataTypeLayout layout;
    SimpleStripeDistribution dist(0, 4, 1000);
    DataTypeProcessor dtp;
    int bytesProcessed = dtp.createFileLayoutForClient(0, 0, 1, dist, 100,
                                                       layout);
    CPPUNIT_ASSERT_EQUAL(0, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)0, layout.getRegions().size());
    //CPPUNIT_ASSERT_EQUAL(FSOffset(0), layout.getRegion(0).offset);
    //CPPUNIT_ASSERT_EQUAL(FSSize(0), layout.getRegion(0).extent);
}

void DataTypeProcessorTest::testCreateFileLayoutForServer()
{
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
