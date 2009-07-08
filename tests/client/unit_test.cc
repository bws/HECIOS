//
// This file is part of Hecios
//
// Copyright (C) 2008 Yang Wu
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cppunit/TextTestRunner.h>
#include "client_fs_state_test.h"
#include "direct_paged_middleware_cache_test.h"
#include "fs_client_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    runner.addTest( ClientFSStateTest::suite() );
    runner.addTest( DirectPagedMiddlewareCacheTest::suite() );
    runner.addTest( FSClientTest::suite() );

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
