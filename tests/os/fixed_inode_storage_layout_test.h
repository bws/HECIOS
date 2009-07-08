#ifndef FIXED_INODE_STORAGE_LAYOUT_TEST_H
#define FIXED_INODE_STORAGE_LAYOUT_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <vector>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_types.h"
#include "fixed_inode_storage_layout.h"
using namespace std;

/** Unit test for FixedINodeStorageLayout */
class FixedINodeStorageLayoutTest : public CppUnit::TestFixture
{
    // Create unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FixedINodeStorageLayoutTest);
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

void FixedINodeStorageLayoutTest::testAddDirectory()
{
    FixedINodeStorageLayout layout(256);
    Filename f1("/1");
    layout.addDirectory(f1);
    vector<FSBlock> blocks = layout.getFileMetaDataBlocks(f1);
    CPPUNIT_ASSERT_EQUAL((size_t)1, blocks.size());
    CPPUNIT_ASSERT_EQUAL((FSBlock)0, blocks[0]);
}

void FixedINodeStorageLayoutTest::testAddFile()
{
    FixedINodeStorageLayout layout(256);
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

void FixedINodeStorageLayoutTest::testGetFileMetaDataBlocks()
{
    FixedINodeStorageLayout layout(256);

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

void FixedINodeStorageLayoutTest::testGetFileDataBlocks()
{
    FixedINodeStorageLayout layout(256);
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
