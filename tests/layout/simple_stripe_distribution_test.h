#ifndef SIMPLE_STRIPE_DISTRIBUTION_TEST_H
#define SIMPLE_STRIPE_DISTRIBUTION_TEST_H
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
#include "simple_stripe_distribution.h"
using namespace std;

/** Unit test for SimpleStripeDistribution */
class SimpleStripeDistributionTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(SimpleStripeDistributionTest);
    CPPUNIT_TEST(testLogicalToPhysicalOffset);
    CPPUNIT_TEST(testNextMappedLogicalOffset);
    CPPUNIT_TEST(testPhysicalToLogicalOffset);
    CPPUNIT_TEST(testContiguousLength);
    CPPUNIT_TEST(testLogicalFileSize);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testLogicalToPhysicalOffset();
    void testNextMappedLogicalOffset();
    void testPhysicalToLogicalOffset();
    void testContiguousLength();
    void testLogicalFileSize();
};

void SimpleStripeDistributionTest::setUp()
{
}

void SimpleStripeDistributionTest::tearDown()
{
}

void SimpleStripeDistributionTest::testLogicalToPhysicalOffset()
{
    SimpleStripeDistribution dist(0, 4, 1000);

    // Test on server 0
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(100), dist.logicalToPhysicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1500), dist.logicalToPhysicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2500), dist.logicalToPhysicalOffset(8500));
    
    // Test on server 1
    dist.setObjectIdx(1);
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(500), dist.logicalToPhysicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.logicalToPhysicalOffset(8500));
    
    // Test on server 2
    dist.setObjectIdx(2);
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(500), dist.logicalToPhysicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.logicalToPhysicalOffset(8500));
    
    // Test on server 3
    dist.setObjectIdx(3);
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.logicalToPhysicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(500), dist.logicalToPhysicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.logicalToPhysicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.logicalToPhysicalOffset(8500));
}

void SimpleStripeDistributionTest::testNextMappedLogicalOffset()
{
    SimpleStripeDistribution dist(0, 4, 1000);

    // Test on server 0
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.nextMappedLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(100), dist.nextMappedLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4000), dist.nextMappedLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4000), dist.nextMappedLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4000), dist.nextMappedLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4000), dist.nextMappedLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4500), dist.nextMappedLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(8500), dist.nextMappedLogicalOffset(8500));
    
    // Test on server 1
    dist.setObjectIdx(1);
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.nextMappedLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.nextMappedLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.nextMappedLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1500), dist.nextMappedLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(5000), dist.nextMappedLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(5000), dist.nextMappedLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(5000), dist.nextMappedLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(9000), dist.nextMappedLogicalOffset(8500));
    
    // Test on server 2
    dist.setObjectIdx(2);
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.nextMappedLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.nextMappedLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.nextMappedLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.nextMappedLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2500), dist.nextMappedLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(6000), dist.nextMappedLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(6000), dist.nextMappedLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(10000), dist.nextMappedLogicalOffset(8500));
    
    // Test on server 3
    dist.setObjectIdx(3);
    CPPUNIT_ASSERT_EQUAL(FSOffset(3000), dist.nextMappedLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(3000), dist.nextMappedLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(3000), dist.nextMappedLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(3000), dist.nextMappedLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(3000), dist.nextMappedLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(3500), dist.nextMappedLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(7000), dist.nextMappedLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(11000), dist.nextMappedLogicalOffset(8500));
}

void SimpleStripeDistributionTest::testPhysicalToLogicalOffset()
{
    SimpleStripeDistribution dist(0, 4, 1000);

    // Test on server 0
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), dist.physicalToLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(100), dist.physicalToLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4000), dist.physicalToLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(4500), dist.physicalToLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(8500), dist.physicalToLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(12500), dist.physicalToLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(16500), dist.physicalToLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(32500), dist.physicalToLogicalOffset(8500));
    
    // Test on server 1
    dist.setObjectIdx(1);
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), dist.physicalToLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(1100), dist.physicalToLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(5000), dist.physicalToLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(5500), dist.physicalToLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(9500), dist.physicalToLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(13500), dist.physicalToLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(17500), dist.physicalToLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(33500), dist.physicalToLogicalOffset(8500));
    
    // Test on server 2
    dist.setObjectIdx(2);
    CPPUNIT_ASSERT_EQUAL(FSOffset(2000), dist.physicalToLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(2100), dist.physicalToLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(6000), dist.physicalToLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(6500), dist.physicalToLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(10500), dist.physicalToLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(14500), dist.physicalToLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(18500), dist.physicalToLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(34500), dist.physicalToLogicalOffset(8500));
    
    // Test on server 3
    dist.setObjectIdx(3);
    CPPUNIT_ASSERT_EQUAL(FSOffset(3000), dist.physicalToLogicalOffset(0));
    CPPUNIT_ASSERT_EQUAL(FSOffset(3100), dist.physicalToLogicalOffset(100));
    CPPUNIT_ASSERT_EQUAL(FSOffset(7000), dist.physicalToLogicalOffset(1000));
    CPPUNIT_ASSERT_EQUAL(FSOffset(7500), dist.physicalToLogicalOffset(1500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(11500), dist.physicalToLogicalOffset(2500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(15500), dist.physicalToLogicalOffset(3500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(19500), dist.physicalToLogicalOffset(4500));
    CPPUNIT_ASSERT_EQUAL(FSOffset(35500), dist.physicalToLogicalOffset(8500));
}

void SimpleStripeDistributionTest::testContiguousLength()
{
    SimpleStripeDistribution dist(0, 4, 1000);

    // Test on server 0
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(0));
    CPPUNIT_ASSERT_EQUAL(FSSize(900), dist.contiguousLength(100));
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(1000));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(1500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(2500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(3500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(4500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(8500));
    
    // Test on server 1
    dist.setObjectIdx(1);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(0));
    CPPUNIT_ASSERT_EQUAL(FSSize(900), dist.contiguousLength(100));
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(1000));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(1500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(2500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(3500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(4500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(8500));
    
    // Test on server 2
    dist.setObjectIdx(2);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(0));
    CPPUNIT_ASSERT_EQUAL(FSSize(900), dist.contiguousLength(100));
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(1000));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(1500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(2500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(3500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(4500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(8500));
    
    // Test on server 3
    dist.setObjectIdx(3);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(0));
    CPPUNIT_ASSERT_EQUAL(FSSize(900), dist.contiguousLength(100));
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), dist.contiguousLength(1000));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(1500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(2500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(3500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(4500));
    CPPUNIT_ASSERT_EQUAL(FSSize(500), dist.contiguousLength(8500));
}

void SimpleStripeDistributionTest::testLogicalFileSize()
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
