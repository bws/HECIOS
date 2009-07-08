//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cppunit/TextTestRunner.h>
#include "basic_data_type_test.h"
#include "block_indexed_data_type_test.h"
#include "client_cache_directory_test.h"
#include "comm_man_test.h"
#include "contiguous_data_type_test.h"
#include "dirty_file_region_set_test.h"
#include "file_descriptor_test.h"
#include "file_page_utils_test.h"
#include "file_region_set_test.h"
#include "file_view_test.h"
#include "filename_test.h"
#include "indexed_data_type_test.h"
#include "ip_socket_map_test.h"
#include "lru_cache_test.h"
#include "lru_timeout_cache_test.h"
#include "pfs_utils_test.h"
#include "phtf_io_trace_test.h"
#include "shtf_io_trace_test.h"
#include "struct_data_type_test.h"
#include "subarray_data_type_test.h"
#include "umd_io_trace_test.h"
#include "vector_data_type_test.h"

/**
 * Unit test driver for common subsystem module
 */

/** Main test driver */
int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests

    runner.addTest( BasicDataTypeTest::suite() );
    runner.addTest( BlockIndexedDataTypeTest::suite() );
    runner.addTest( ClientCacheDirectoryTest::suite() );
    runner.addTest( CommManTest::suite() );
    runner.addTest( ContiguousDataTypeTest::suite() );
    runner.addTest( DirtyFileRegionSetTest::suite() );
    runner.addTest( FileDescriptorTest::suite() );
    runner.addTest( FilePageUtilsTest::suite() );
    runner.addTest( FileRegionSetTest::suite() );
    runner.addTest( FileViewTest::suite() );
    runner.addTest( FilenameTest::suite() );
    runner.addTest( IPSocketMapTest::suite() );
    runner.addTest( IndexedDataTypeTest::suite() );
    runner.addTest( LRUCacheTest::suite() );
    runner.addTest( LRUTimeoutCacheTest::suite() );
    runner.addTest( PFSUtilsTest::suite() );
    //runner.addTest( PHTFIOTraceTest::suite() );
    runner.addTest( SHTFIOTraceTest::suite() );
    runner.addTest( StructDataTypeTest::suite() );
    runner.addTest( SubarrayDataTypeTest::suite() );
    runner.addTest( UMDIOTraceTest::suite() );
    runner.addTest( VectorDataTypeTest::suite() );


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
