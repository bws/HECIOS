#ifndef BMI_LIST_IO_DATA_FLOW_TEST_H
#define BMI_LIST_IO_DATA_FLOW_TEST_H
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
#include "bmi_list_io_data_flow.h"
using namespace std;

/** Unit test for BMIListIODataFlow */
class BMIListIODataFlowTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BMIListIODataFlowTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testConstructor();
    
private:

};

void BMIListIODataFlowTest::setUp()
{
}

void BMIListIODataFlowTest::tearDown()
{
}

void BMIListIODataFlowTest::testConstructor()
{
    CPPUNIT_FAIL("Test not yet implemented.");
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
