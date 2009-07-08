#ifndef BMI_LIST_IO_DATA_FLOW_TEST_H
#define BMI_LIST_IO_DATA_FLOW_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
