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
    LRUSimpleCache* cache1_;
    LRUSimpleCache* cache2_;
};

void LRUSimpleCacheTest::setUp()
{
    cache1_ = new LRUSimpleCache(10);
    cache2_ = new LRUSimpleCache(2);
    printf("\n----began setup\n");
    fflush(stdout);
}

void LRUSimpleCacheTest::tearDown()
{
    delete cache1_;
    cache1_ = 0;
    delete cache2_;
    cache2_ = 0;
}

void LRUSimpleCacheTest::testConstructor()
{
    LRUSimpleCache cache(1);
    CPPUNIT_ASSERT_EQUAL(0, cache.size());
}

void LRUSimpleCacheTest::testInsert()
{
    printf("first insert \n");
    LRUSimpleCache cache(1);
    cache.insert(1,1);
   
    printf("end of insert \n");
    fflush(stdout); 
    cache1_->insert(1, 1);
    cache1_->insert(2, 2);
    cache1_->insert(10, 2);
    CPPUNIT_ASSERT_EQUAL(3, cache1_->size());
        
    cache2_->insert(101, 11);
    cache2_->insert(102, 22);
    //CPPUNIT_ASSERT_EQUAL(2, cache2_->size());
    CPPUNIT_ASSERT_EQUAL(1, cache2_->size());
}

void LRUSimpleCacheTest::testRemove()
{
    // Remove from an empty cache
    cache1_->remove(77);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Remove an existing entry from a cache
    cache1_->insert(63, 63);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
    cache1_->remove(63);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());
}

void LRUSimpleCacheTest::testLookup()
{
    // Lookup an entry in an empty cache
    LRUSimpleCache::EntryType* e77 = cache1_->lookup(77);
    CPPUNIT_ASSERT(0 == e77);

    // Find an existing entry
    cache1_->insert(2000, 2000);
    LRUSimpleCache::EntryType* e2000 = cache1_->lookup(2000);
    CPPUNIT_ASSERT(2000 == e2000->extent);

    // Fill the cache and lookup all entries
    cache1_->insert(2001, 2001);
    cache1_->insert(2002, 2002);
    cache1_->insert(2003, 2003);
    cache1_->insert(2004, 2004);
    cache1_->insert(2005, 2005);
    cache1_->insert(2006, 2006);
    cache1_->insert(2007, 2007);
    cache1_->insert(2008, 2008);
    cache1_->insert(2009, 2009);

    CPPUNIT_ASSERT(cache1_->lookup(3000));
    CPPUNIT_ASSERT(!cache1_->lookup(8000));
    CPPUNIT_ASSERT(cache1_->lookup(2000));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2000)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2001));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2001)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2002));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2002)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2003));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2003)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2004));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2004)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2005));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2005)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2006));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2006)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2007));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2007)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2008));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2008)->extent);
    
    CPPUNIT_ASSERT(cache1_->lookup(2009));
    CPPUNIT_ASSERT_EQUAL(2018, cache1_->lookup(2009)->extent);
    
    // Lookup a non-existant entry
    CPPUNIT_ASSERT(0 == cache1_->lookup(9010));
    
}

void LRUSimpleCacheTest::testSize()
{
    // Check the size of an empty cache
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Check the size of cache after insert
    cache1_->insert(74, 74);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());

    // Check the size of cache after remove
    cache1_->remove(74);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Check the size after duplicate insertion
    cache1_->insert(640, 640);
    cache1_->insert(640, 640);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());

    // Check the size after maximum fill
    cache1_->insert(640, 640);
    cache1_->insert(641, 641);
    cache1_->insert(642, 642);
    cache1_->insert(643, 643);
    cache1_->insert(644, 644);
    cache1_->insert(645, 645);
    cache1_->insert(646, 646);
    cache1_->insert(647, 647);
    cache1_->insert(648, 648);
    cache1_->insert(649, 649);
    cache1_->insert(650, 650);
    cache1_->insert(651, 651);
    cache1_->insert(652, 652);
    cache1_->insert(653, 653);
    // should be 1 b/c of all combined inserts
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
}

void LRUSimpleCacheTest::testLRUPolicy()
{
    // Fill the cache
    cache1_->insert(640, 640);
    cache1_->insert(641, 641);
    cache1_->insert(642, 642);
    cache1_->insert(643, 643);
    cache1_->insert(644, 644);
    cache1_->insert(645, 645);
    cache1_->insert(646, 646);
    cache1_->insert(647, 647);
    cache1_->insert(648, 648);
    cache1_->insert(649, 649);
    
    // Test LRU after inserts
    cache1_->insert(650, 650);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
    CPPUNIT_ASSERT_EQUAL(660, cache1_->lookup(640)->extent);
    CPPUNIT_ASSERT(0 == cache1_->lookup(6410));
    CPPUNIT_ASSERT(0 != cache1_->lookup(640));
    CPPUNIT_ASSERT(0 != cache1_->lookup(650));
    
    // Test LRU after lookups
    cache1_->lookup(641);
    cache1_->insert(651, 651);
    CPPUNIT_ASSERT(0 == cache1_->lookup(1303));
    CPPUNIT_ASSERT(0 != cache1_->lookup(641));
    CPPUNIT_ASSERT(0 != cache1_->lookup(651));
    
    // Test LRU after removes
    cache1_->remove(643);
    CPPUNIT_ASSERT(0 == cache1_->lookup(643));

    cache1_->insert(652, 652);
    cache1_->insert(653, 653);
    CPPUNIT_ASSERT(0 == cache1_->lookup(644));
    CPPUNIT_ASSERT(0 != cache1_->lookup(652));
    CPPUNIT_ASSERT(0 != cache1_->lookup(653));
        

    cache2_->insert(1200, 2);
    cache2_->insert(3999, 3);
    cache2_->lookup(1200);
    CPPUNIT_ASSERT_EQUAL(2, cache2_->size());
    cache2_->insert(14999, 3);
    CPPUNIT_ASSERT(0 == cache2_->lookup(3999));
    CPPUNIT_ASSERT(0 != cache2_->lookup(1200));
    CPPUNIT_ASSERT(0 != cache2_->lookup(14999));
    CPPUNIT_ASSERT_EQUAL(2, cache2_->size());
    

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
