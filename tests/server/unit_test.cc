
#include <cppunit/TextTestRunner.h>
#include "bmi_list_io_data_flow_test.h"
#include "fs_server_test.h"
#include "job_manager_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the subsystem tests
    runner.addTest( BMIListIODataFlowTest::suite() );
    runner.addTest( FSServerTest::suite() );
    runner.addTest( JobManagerTest::suite() );

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
