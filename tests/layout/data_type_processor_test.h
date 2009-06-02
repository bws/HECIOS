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
#include "basic_data_type.h"
#include "basic_types.h"
#include "contiguous_data_type.h"
#include "data_type_layout.h"
#include "file_page_utils.h"
#include "file_view.h"
#include "simple_stripe_distribution.h"
#include "subarray_data_type.h"
using namespace std;

/** Unit test for DataTypeProcessor */
class DataTypeProcessorTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(DataTypeProcessorTest);
    CPPUNIT_TEST(testCreateClientFileLayoutForWrite);
    CPPUNIT_TEST(testCreateServerFileLayoutForRead);
    CPPUNIT_TEST(testCreateServerFileLayoutForWrite);
    CPPUNIT_TEST(test512kFileLayoutForServer);
    CPPUNIT_TEST(testTileIO);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testCreateClientFileLayoutForWrite();
    void testCreateServerFileLayoutForRead();
    void testCreateServerFileLayoutForWrite();
    void test512kFileLayoutForServer();
    void testTileIO();
};

void DataTypeProcessorTest::setUp()
{
}

void DataTypeProcessorTest::tearDown()
{
}

void DataTypeProcessorTest::testCreateClientFileLayoutForWrite()
{
    // Use a simple stripe distribution for all these tests
    ByteDataType byteType;
    SimpleStripeDistribution dist0(0, 4, 1000);
    SimpleStripeDistribution dist1(1, 4, 1000);
    DataTypeProcessor dtp;

    // TEST 1: with count 1, offset 0, data type 1, server0
    FileView view1(0, new ByteDataType());
    FSSize aggSize1 = 0;
    int bytesProcessed = dtp.createClientFileLayoutForWrite(0, byteType, 1,
                                                            view1, dist0, aggSize1);
    CPPUNIT_ASSERT_EQUAL(1, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL(FSSize(1), aggSize1);

    // TEST 2: with count 1, offset 0, data type 1, server1
    FileView view2(0, new ByteDataType());
    FSSize aggSize2 = 0;
    bytesProcessed = dtp.createClientFileLayoutForWrite(0, byteType, 1,
                                                        view2, dist1, aggSize2);
    CPPUNIT_ASSERT_EQUAL(0, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL(FSSize(1), aggSize2);

    // TEST 3: with count 8000, offset 0, data type 1, server0
    FileView view3(0, new ByteDataType());
    FSSize aggSize3 = 0;
    bytesProcessed = dtp.createClientFileLayoutForWrite(0, byteType, 8000,
                                                        view3, dist0, aggSize3);
    CPPUNIT_ASSERT_EQUAL(2000, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL(FSSize(8000), aggSize3);

    // TEST 4: with count 8000, offset 0, data type 1, server1
    FileView view4(0, new ByteDataType());
    FSSize aggSize4 = 0;
    bytesProcessed = dtp.createClientFileLayoutForWrite(0, byteType, 8000,
                                                   view4, dist1, aggSize4);
    CPPUNIT_ASSERT_EQUAL(2000, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL(FSSize(8000), aggSize4);
}

void DataTypeProcessorTest::testCreateServerFileLayoutForRead()
{
    SimpleStripeDistribution dist0(0, 1, 100000);
    DataTypeProcessor dtp;

    // Test 1
    set<FilePageId> pageIds;
    pageIds.insert(1);
    FSSize pageSize = 4096;
    FileView* view = FilePageUtils::instance().createPageViewDescriptor(pageSize, pageIds);
    FSSize dataSize = 4096;

    DataTypeLayout layout1;
    int bytesProcessed = dtp.createServerFileLayoutForRead(FSOffset(0),
                                                           dataSize,
                                                           *view,
                                                           dist0,
                                                           10000,
                                                           layout1);
    CPPUNIT_ASSERT_EQUAL(4096, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)1, layout1.getRegions().size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(4096), layout1.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(4096), layout1.getRegion(0).extent);
}

void DataTypeProcessorTest::testCreateServerFileLayoutForWrite()
{
    // Use a simple stripe distribution for all these tests
    SimpleStripeDistribution dist0(0, 4, 1000);
    SimpleStripeDistribution dist1(1, 4, 1000);
    DataTypeProcessor dtp;

    // TEST 1: with offset 0, dataSize 1, server0
    DataTypeLayout layout1;
    FileView view1(0, new ByteDataType());
    int bytesProcessed = dtp.createServerFileLayoutForWrite(0, 1,
                                                            view1, dist0, layout1);
    CPPUNIT_ASSERT_EQUAL(1, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)1, layout1.getRegions().size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), layout1.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1), layout1.getRegion(0).extent);

    // TEST 2: with offset 0, dataSize 1, server1
    DataTypeLayout layout2;
    FileView view2(0, new ByteDataType());
    bytesProcessed = dtp.createServerFileLayoutForWrite(0, 1,
                                                   view2, dist1, layout2);
    CPPUNIT_ASSERT_EQUAL(0, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)0, layout2.getRegions().size());

    // TEST 3: with count 8000, offset 0, data type 1, server0
    DataTypeLayout layout3;
    FileView view3(0, new ByteDataType());
    bytesProcessed = dtp.createServerFileLayoutForWrite(0, 8000,
                                                        view3, dist0, layout3);
    CPPUNIT_ASSERT_EQUAL(2000, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)2, layout3.getRegions().size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), layout3.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), layout3.getRegion(0).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), layout3.getRegion(1).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), layout3.getRegion(1).extent);

    // TEST 4: with count 8000, offset 0, data type 1, server1
    DataTypeLayout layout4;
    FileView view4(0, new ByteDataType());
    bytesProcessed = dtp.createServerFileLayoutForWrite(0, 8000,
                                                   view4, dist1, layout4);
    CPPUNIT_ASSERT_EQUAL(2000, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)2, layout4.getRegions().size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0000), layout4.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), layout4.getRegion(0).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(1000), layout4.getRegion(1).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(1000), layout4.getRegion(1).extent);
}

void DataTypeProcessorTest::test512kFileLayoutForServer()
{
    // Use a simple stripe distribution for this tests
    SimpleStripeDistribution dist0(0, 2, 65536);
    SimpleStripeDistribution dist1(1, 2, 65536);
    DataTypeProcessor dtp;

    // TEST 1: with offset 0, dataSize 524288, server0
    DataTypeLayout layout1;
    FileView view1(0, new ByteDataType());
    int bytesProcessed = dtp.createServerFileLayoutForWrite(0, 524288,
                                                            view1, dist0, layout1);
    CPPUNIT_ASSERT_EQUAL(262144, bytesProcessed);
    CPPUNIT_ASSERT_EQUAL((size_t)4, layout1.getRegions().size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), layout1.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout1.getRegion(0).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(65536), layout1.getRegion(1).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout1.getRegion(1).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(131072), layout1.getRegion(2).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout1.getRegion(2).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(196608), layout1.getRegion(3).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout1.getRegion(3).extent);

    // TEST 2: with offset 0, dataSize 524288, server1
    DataTypeLayout layout2;
    FileView view2(0, new ByteDataType());
    int bytesProcessed2 = dtp.createServerFileLayoutForWrite(0, 524288,
                                                             view2, dist1, layout2);
    CPPUNIT_ASSERT_EQUAL(262144, bytesProcessed2);
    CPPUNIT_ASSERT_EQUAL((size_t)4, layout2.getRegions().size());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), layout2.getRegion(0).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout2.getRegion(0).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(65536), layout2.getRegion(1).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout2.getRegion(1).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(131072), layout2.getRegion(2).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout2.getRegion(2).extent);
    CPPUNIT_ASSERT_EQUAL(FSOffset(196608), layout2.getRegion(3).offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(65536), layout2.getRegion(3).extent);
}

void DataTypeProcessorTest::testTileIO()
{
    ByteDataType byteType;
    ContiguousDataType elementType(8, byteType);

    // Tile I/O for process 0
    size_t sizes[] = {1000, 80};
    size_t subSizes[] = {1000, 10};
    size_t starts[] = {0, 0};
    SubarrayDataType* subarray = new SubarrayDataType(vector<size_t>(sizes, sizes + 2),
                                                      vector<size_t>(subSizes, subSizes + 2),
                                                      vector<size_t>(starts, starts + 2),
                                                      SubarrayDataType::C_ORDER, elementType);
    FileView subarrayView(0, subarray);

    // Retrieve the first read
    FileRegionSet regions0_0 = DataTypeProcessor::locateFileRegionSet(0,
                                                                    80000,
                                                                    subarrayView);
    FileRegionSet::const_iterator first = regions0_0.begin();
    FileRegionSet::const_iterator last = regions0_0.end();
    while (first != last)
    {
        first++;
        //cerr << *(first++) << endl;
    }
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions0_0.size());

    // Retrieve the second read
    vector<FileRegion> regions0_1 = DataTypeProcessor::locateFileRegions(80000,
                                                                         80000,
                                                                         subarrayView);
    vector<FileRegion>::const_iterator first0_1 = regions0_1.begin();
    vector<FileRegion>::const_iterator last0_1 = regions0_1.end();
    while (first0_1 != last0_1)
    {
        cerr << *(first0_1++) << endl;
    }
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions0_1.size());

    // Retrieve the third read
    vector<FileRegion> regions0_2 = DataTypeProcessor::locateFileRegions(160000,
                                                                         80000,
                                                                         subarrayView);
    vector<FileRegion>::const_iterator first0_2 = regions0_2.begin();
    vector<FileRegion>::const_iterator last0_2 = regions0_2.end();
    while (first0_2 != last0_2)
    {
        cerr << *(first0_2++) << endl;
    }
    CPPUNIT_ASSERT_EQUAL(size_t(1000), regions0_2.size());
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
