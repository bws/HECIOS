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
#include <cppunit/TextTestRunner.h>
#include "basic_data_type_test.h"
#include "contiguous_data_type_test.h"
#include "file_descriptor_test.h"
#include "file_view_test.h"
#include "filename_test.h"
#include "ip_socket_map_test.h"
#include "umd_io_trace_test.h"
#include "lru_cache_test.h"
#include "lru_timeout_cache_test.h"
#include "pfs_utils_test.h"
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
    runner.addTest( ContiguousDataTypeTest::suite() );
    runner.addTest( FileDescriptorTest::suite() );
    runner.addTest( FileViewTest::suite() );
    runner.addTest( FilenameTest::suite() );
    runner.addTest( IPSocketMapTest::suite() );
    runner.addTest( LRUCacheTest::suite() );
    runner.addTest( LRUTimeoutCacheTest::suite() );
    runner.addTest( PFSUtilsTest::suite() );
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
