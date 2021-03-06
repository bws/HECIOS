//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
/**
 * Unit test driver for subsystem module
 */
#include <cppunit/TextTestRunner.h>
#include "fixed_inode_storage_layout_test.h"
#include "native_file_system_test.h"
#include "no_translation_test.h"

/** Main test driver */
int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the requisite tests
    runner.addTest( FixedINodeStorageLayoutTest::suite() );
    runner.addTest( NativeFileSystemTest::suite() );
    runner.addTest( NoTranslationTest::suite() );

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
