/**
 * Unit test driver for subsystem module
 */

#include <cppunit/TextTestRunner.h>
#include "umd_io_trace_test.h"
#include "lru_timeout_cache_test.h"
#include "pfs_utils_test.h"
#include "ip_socket_map_test.h"


/** Main test driver */
int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    runner.addTest( UMDIOTraceTest::suite() );
    runner.addTest( LRUTimeoutCacheTest::suite() );
    runner.addTest( PFSUtilsTest::suite() );
    runner.addTest( IPSocketMapTest::suite() );

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
