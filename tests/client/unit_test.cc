
#include <cppunit/TextTestRunner.h>
#include "client_fs_state_test.h"
#include "fs_client_test.h"
#include "lru_complex_cache_test.h"
#include "lru_simple_cache_test.h"
#include "mpi_middleware_bcast_sm_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    runner.addTest( ClientFSStateTest::suite() );
    runner.addTest( FSClientTest::suite() );
    runner.addTest( MPIMiddlewareBcastSmTest::suite() );
    //runner.addTest( LRUSimpleCacheTest::suite() );
    //runner.addTest( LRUComplexCacheTest::suite() );

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
