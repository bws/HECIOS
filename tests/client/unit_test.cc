
#include <cppunit/TextTestRunner.h>
//#include "umd_io_trace_test.h"
#include "lru_simple_cache_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    //runner.addTest( UMDIOTraceTest::suite() );
    //runner.addTest( LRUTimeoutCacheTest::suite() );
    runner.addTest( LRUSimpleCacheTest::suite() );

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
