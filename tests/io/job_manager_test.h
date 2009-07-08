#ifndef JOB_MANAGER_TEST_H
#define JOB_MANAGER_TEST_H
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
#include "basic_data_type.h"
#include "basic_distribution.h"
#include "bmi_list_io_data_flow.h"
#include "file_view.h"
#include "csimple_module_tester.h"
#include "job_manager.h"
#include "pfs_types.h"
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
    view_ = new FileView(0, new DoubleDataType());

    // Create the read request
    readRequest_ = new spfsReadRequest(0, SPFS_READ_REQUEST);
    readRequest_->setOffset(0);
    readRequest_->setView(view_);
    readRequest_->setDist(distribution_);

    // Create the begin flow request
    spfsClientDataFlowStart* flowStart = new spfsClientDataFlowStart(0, SPFS_DATA_FLOW_START);
    flowStart->setContextPointer(readRequest_);
    flowStart->setOffset(0);
    flowStart->setDist(distribution_);
    flowStart->setView(view_);
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
