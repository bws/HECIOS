#ifndef STORAGE_LAYOUT_TEST_H
#define STORAGE_LAYOUT_TEST_H
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
#include <vector>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_types.h"
#include "storage_layout.h"
using namespace std;

/** Unit test for StorageLayout */
class StorageLayoutTest : public CppUnit::TestFixture
{
    // Create unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(StorageLayoutTest);
    CPPUNIT_TEST(testAddDirectory);
    CPPUNIT_TEST(testAddFile);
    CPPUNIT_TEST(testGetFileMetaDataBlocks);
    CPPUNIT_TEST(testGetFileDataBlocks);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp() {};

    /** Called after each test function */
    virtual void tearDown() {};

    void testAddDirectory();
    void testAddFile();
    void testGetFileMetaDataBlocks();
    void testGetFileDataBlocks();    
};

void StorageLayoutTest::testAddDirectory()
{
    StorageLayout layout(256);
    Filename f1("/1");
    layout.addDirectory(f1);
    vector<FSBlock> blocks = layout.getFileMetaDataBlocks(f1);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)0, blocks[0]);
}

void StorageLayoutTest::testAddFile()
{
    StorageLayout layout(256);
    Filename f1("/1");
    layout.addFile(f1, 1024);
    vector<FSBlock> blocks = layout.getFileMetaDataBlocks(f1);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)0, blocks[0]);

    // Data blocks start at block 1000
    blocks = layout.getFileDataBlocks(f1, 0, 1024);
    CPPUNIT_ASSERT_EQUAL((size_t)4, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1000, blocks[0]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[1]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1002, blocks[2]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1003, blocks[3]);
}

void StorageLayoutTest::testGetFileMetaDataBlocks()
{
    StorageLayout layout(256);

    // Create a directory to test
    Filename f1("/1");
    layout.addDirectory(f1);    
    vector<FSBlock> blocks1 = layout.getFileMetaDataBlocks(f1);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks1.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)0, blocks1[0]);

    
    // Create a file to test
    Filename f2("/2");
    layout.addFile(f2, 0);
    vector<FSBlock> blocks2 = layout.getFileMetaDataBlocks(f2);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks2.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1, blocks2[0]);
}

void StorageLayoutTest::testGetFileDataBlocks()
{
    StorageLayout layout(256);
    vector<FSBlock> blocks;
    
    // Create a file to test
    Filename f1("/1");
    layout.addFile(f1, 2048);

    // Test retrieving no blocks
    blocks = layout.getFileDataBlocks(f1, 0, 0);
    CPPUNIT_ASSERT_EQUAL((size_t)0, blocks.size());

    // Test retrieving entire first block
    blocks = layout.getFileDataBlocks(f1, 0, 256);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1000, blocks[0]);

    // Test retrieving entire last block
    blocks = layout.getFileDataBlocks(f1, 1792, 256);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1007, blocks[0]);

    // Test retrieving entire 2nd block
    blocks = layout.getFileDataBlocks(f1, 256, 256);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[0]);

    // Test retrieving entire file
    blocks = layout.getFileDataBlocks(f1, 0, 2048);
    CPPUNIT_ASSERT_EQUAL((size_t)8, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1000, blocks[0]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[1]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1002, blocks[2]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1003, blocks[3]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1004, blocks[4]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1005, blocks[5]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1006, blocks[6]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1007, blocks[7]);

    // Test a single block request in the center of a block
    blocks = layout.getFileDataBlocks(f1, 312, 4);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[0]);

    // Test a single block request at the beginning of a block
    blocks = layout.getFileDataBlocks(f1, 256, 12);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[0]);

    // Test a single block request at the end of a block
    blocks = layout.getFileDataBlocks(f1, 500, 11);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[0]);

    // Test a small request that spans two blocks
    blocks = layout.getFileDataBlocks(f1, 248, 20);
    CPPUNIT_ASSERT_EQUAL((size_t)2, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1000, blocks[0]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[1]);

    // Test a request with partial blocks at the beginning and end
    blocks = layout.getFileDataBlocks(f1, 248, 312);
    CPPUNIT_ASSERT_EQUAL((size_t)3, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)1000, blocks[0]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1001, blocks[1]);
    CPPUNIT_ASSERT_EQUAL((FSBlock)1002, blocks[2]);
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
