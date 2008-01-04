#ifndef JOB_MANAGER_TEST_H
#define JOB_MANAGER_TEST_H
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
#include "basic_data_type.h"
#include "basic_distribution.h"
#include "bmi_list_io_data_flow.h"
#include "file_view.h"
#include "csimple_module_tester.h"
#include "job_manager.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for JobManager */
class JobManagerTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(JobManagerTest);
    CPPUNIT_TEST(testRegisterDataFlow);
    CPPUNIT_TEST(testDeregisterDataFlow);
    CPPUNIT_TEST(testLookupDataFlow);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test registerDataFlow */
    void testRegisterDataFlow();

    /** Test deregisterDataFlow */
    void testDeregisterDataFlow();

    /** Test lookupDataFlow */
    void testLookupDataFlow();

private:

    cSimpleModuleTester* moduleTester_;

    JobManager* module_;

    FileDistribution* distribution_;

    FileView* view_;
    
    spfsReadRequest* readRequest_;
    
    spfsDataFlowStart* flowStart_;
};

void JobManagerTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("JobManager",
                                            "src/io/job_manager.ned");
    module_ = dynamic_cast<JobManager*>(moduleTester_->getModule());
    CPPUNIT_ASSERT(0 != module_);

    // Create the distribution
    distribution_ = new BasicDistribution();

    // Create the file view
    view_ = new FileView(0, new BasicDataType(4));
    
    // Create the read request
    readRequest_ = new spfsReadRequest(0, SPFS_READ_REQUEST);
    readRequest_->setOffset(0);
    readRequest_->setView(view_);
    readRequest_->setDist(distribution_);

    // Create the begin flow request
    flowStart_ = new spfsDataFlowStart(0, SPFS_DATA_FLOW_START);
    flowStart_->setFlowType(1);
    flowStart_->setFlowMode(DataFlow::SERVER_READ);
    flowStart_->setContextPointer(readRequest_);
    flowStart_->setOffset(0);
    flowStart_->setDataSize(4);
    flowStart_->setDist(distribution_);
    flowStart_->setView(view_);
}

void JobManagerTest::tearDown()
{
    delete moduleTester_;
    moduleTester_ = 0;

    delete distribution_;
    //delete readRequest_;
    //delete flowBegin_;
}

void JobManagerTest::testRegisterDataFlow()
{
    BMIListIODataFlow flow1(*flowStart_, 1, 1, module_);
    BMIListIODataFlow flow2(*flowStart_, 1, 1, module_);
    int id1 = module_->registerDataFlow(&flow1);
    int id2 = module_->registerDataFlow(&flow2);
    CPPUNIT_ASSERT(id1 != id2);
}

void JobManagerTest::testDeregisterDataFlow()
{
    BMIListIODataFlow flow1(*flowStart_, 1, 1, module_);
    BMIListIODataFlow flow2(*flowStart_, 1, 1, module_);
    int id1 = module_->registerDataFlow(&flow1);
    int id2 = module_->registerDataFlow(&flow2);
    module_->deregisterDataFlow(id1);
    module_->deregisterDataFlow(id2);
    CPPUNIT_ASSERT(0 == module_->lookupDataFlow(id1));
    CPPUNIT_ASSERT(0 == module_->lookupDataFlow(id2));
}

void JobManagerTest::testLookupDataFlow()
{
    BMIListIODataFlow flow1(*flowStart_, 1, 1, module_);
    BMIListIODataFlow flow2(*flowStart_, 1, 1, module_);
    int id1 = module_->registerDataFlow(&flow1);
    int id2 = module_->registerDataFlow(&flow2);
    CPPUNIT_ASSERT(0 != module_->lookupDataFlow(id1));
    CPPUNIT_ASSERT(0 != module_->lookupDataFlow(id2));
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
