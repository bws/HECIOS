
#include <cppunit/TextTestRunner.h>
#include "bmi_direct_endpoint_test.h"
#include "bmi_tcp_client_test.h"
#include "bmi_tcp_endpoint_test.h"
#include "bmi_tcp_server_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the subsystem tests
    runner.addTest( BMIDirectEndpointTest::suite() );
    runner.addTest( BMITcpClientTest::suite() );
    runner.addTest( BMITcpEndpointTest::suite() );
    runner.addTest( BMITcpServerTest::suite() );

    bool success = runner.run();
    return (success ? 0 : 1);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
