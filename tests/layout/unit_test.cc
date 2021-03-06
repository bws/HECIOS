/**
 * Unit test driver for subsystem module
 */

#include <cppunit/TextTestRunner.h>
#include "data_type_layout_test.h"
#include "data_type_processor_test.h"
#include "file_builder_test.h"
#include "simple_stripe_distribution_test.h"

/** Main test driver */
int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    runner.addTest( DataTypeLayoutTest::suite() );
    runner.addTest( DataTypeProcessorTest::suite() );
    runner.addTest( FileBuilderTest::suite() );
    runner.addTest( SimpleStripeDistributionTest::suite() );

    bool success = runner.run();
    return (success ? 0 : 1);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
