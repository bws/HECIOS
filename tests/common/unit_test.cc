
#include <cppunit/TextTestRunner.h>
#include "umd_io_trace_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    runner.addTest( UMDIOTraceTest::suite() );

    bool success = runner.run();
    return (success ? 0 : 1);
}
