#ifndef LRU_SIMPLE_CACHE_TEST_H
#define LRU_SIMPLE_CACHE_TEST_H

#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "lru_simple_cache.h"
using namespace std;

/** Unit test for LRUSimpleCache */
class LRUSimpleCacheTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(LRUSimpleCacheTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testRemove);
    CPPUNIT_TEST(testLookup);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testLRUPolicy);
    CPPUNIT_TEST(testCombinePolicy);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testConstructor();

    void testInsert();

    void testRemove();
    
    void testLookup();

    void testSize();

    void testLRUPolicy();

    void testCombinePolicy();

private:
    LRUSimpleCache* cache2_;
};

void LRUSimpleCacheTest::setUp()
{
}

void LRUSimpleCacheTest::tearDown()
{
}

void LRUSimpleCacheTest::testConstructor()
{
}

void LRUSimpleCacheTest::testInsert()
{
}

void LRUSimpleCacheTest::testRemove()
{
}

void LRUSimpleCacheTest::testLookup()
{
    
}

void LRUSimpleCacheTest::testSize()
{

}

void LRUSimpleCacheTest::testLRUPolicy()
{
}

void LRUSimpleCacheTest::testCombinePolicy()
{
    
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
