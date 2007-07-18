#ifndef LRU_COMPLEX_CACHE_TEST_H
#define LRU_COMPLEX_CACHE_TEST_H

#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "../src/client/lru_complex_cache.h"
using namespace std;

/** Unit test for LRUComplexCache */
class LRUComplexCacheTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(LRUComplexCacheTest);
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
    LRUComplexCache* cache1_;
    LRUComplexCache* cache2_;
};

void LRUComplexCacheTest::setUp()
{
    cache1_ = new LRUComplexCache(100,100000);
    cache2_ = new LRUComplexCache(5, 100);
    printf("\n---began complex cache setup \n");
    fflush(stdout);
}

void LRUComplexCacheTest::tearDown()
{
    delete cache1_;
    cache1_ = 0;
    delete cache2_;
    cache2_ = 0;
}

void LRUComplexCacheTest::testConstructor()
{
    LRUComplexCache cache(10, 100);
    CPPUNIT_ASSERT_EQUAL(0, cache.size());
}

void LRUComplexCacheTest::testInsert()
{
    printf("first insert \n");
    LRUComplexCache cache(1, 10000);
    cache.insert(1, 0, 300);

    printf("end of insert \n");
    fflush(stdout);
    cache1_->insert(1, 1, 20);
    cache1_->insert(1, 2, 2);
    cache1_->insert(1, 3, 2);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
    
    cache1_->insert(653, 1, 20);
    //cache1_->removeOffset(655,2);
    cache1_->insert(653, 2, 2);
    cache1_->insert(653, 30, 30);
    cache1_->insert(56, 3, 2);
    //cache1_->removeOffset(655,2);
    CPPUNIT_ASSERT_EQUAL(3, cache1_->size());

    cache2_->insert(43, 3, 5);
    //cache2_->insert(43, 10, 5);
    cache2_->insert(655, 7, 40);
    cache2_->insert(56, 3, 30);
    cache2_->insert(75, 10, 35);
    cache2_->mapPrint();
    CPPUNIT_ASSERT_EQUAL(2, cache2_->size());
    
}

void LRUComplexCacheTest::testRemove()
{
    // Remove from an empty cache
    cache1_->removeHandle(77);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Remove an existing entry from a cache
    cache1_->insert(63, 0, 63);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
    cache1_->removeOffset(63, 30);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());
    
    cache1_->insert(653, 1, 20);
    cache1_->insert(653, 2, 2);
    cache1_->removeOffset(653,2);
    cache1_->insert(655, 1, 30);
    cache1_->insert(56, 3, 2);
    cache1_->removeOffset(655,30);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
    cache1_->removeHandle(56);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

}

void LRUComplexCacheTest::testLookup()
{
    // Lookup an entry in an empty cache
    LRUComplexCache::ComplexEntryType* e77 = cache1_->lookup(77,0,0);
    CPPUNIT_ASSERT(0 == e77);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Find an existing entry
    cache1_->insert(2000, 0, 1000);
    LRUComplexCache::ComplexEntryType* e2000 = cache1_->lookup(2000,0,1);
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());
    CPPUNIT_ASSERT(2000 == e2000->handle);

    // Fill the cache and lookup all entries
    cache1_->insert(2001, 0, 2001);
    cache1_->insert(2002, 6, 2002);
    cache1_->insert(2003, 5, 2003);
    cache1_->insert(2004, 0, 2004);
    cache1_->insert(2005, 6, 2005);
    cache1_->insert(2006, 2, 2006);
    cache1_->insert(2007, 2, 2007);
    cache1_->insert(2008, 2, 2008);
    cache1_->insert(2009, 0, 2009);
    CPPUNIT_ASSERT_EQUAL(10, cache1_->size());

    CPPUNIT_ASSERT(!cache1_->lookup(3000,0,0));
    CPPUNIT_ASSERT(!cache1_->lookup(8000,0,0));
    //CPPUNIT_ASSERT(cache1_->lookup(2000,3,2));
    //printf("cache size id %d\n", cache1_->size());
    CPPUNIT_ASSERT_EQUAL(2008, cache1_->lookup(2008,50,40)->handle);

    CPPUNIT_ASSERT(cache1_->lookup(2001,0,10));
    CPPUNIT_ASSERT(!cache1_->lookup(2001,0,6000));


    CPPUNIT_ASSERT(cache1_->lookup(2005,10,20));
    CPPUNIT_ASSERT_EQUAL(2005, cache1_->lookup(2005,10,30)->handle);

    CPPUNIT_ASSERT(cache1_->lookup(2007, 4,3));
    CPPUNIT_ASSERT_EQUAL(2007, cache1_->lookup(2007,20,2)->handle);

    CPPUNIT_ASSERT(cache1_->lookup(2009,3,1000));
    CPPUNIT_ASSERT_EQUAL(2009, cache1_->lookup(2009,5,20)->handle);

    // Lookup a non-existant entry
    CPPUNIT_ASSERT(0 == cache1_->lookup(9010,0,0));
    
    cache1_->insert(2020, 0, 10);
    CPPUNIT_ASSERT_EQUAL(11, cache1_->size());
    CPPUNIT_ASSERT(!cache1_->lookup(2020,10,15));
    printf("got here --\n");
    fflush(stdout);
    cache1_->insert(2020, 10, 15);
    printf("got here --\n");
    fflush(stdout);
    CPPUNIT_ASSERT_EQUAL(11, cache1_->size());
    cache1_->insert(2020, 0, 2001);
    cache1_->insert(2020, 0, 2001);
    cache1_->insert(2020, 0, 2001);
    cache1_->insert(2020, 0, 2001);
}

void LRUComplexCacheTest::testSize()
{
    // Fill the cache and lookup all entries
    cache1_->insert(2001, 0, 2001);
    cache1_->insert(2002, 6, 2002);
    cache1_->insert(2003, 5, 2003);
    cache1_->insert(2004, 0, 2004);
    cache1_->insert(2005, 6, 2005);
    cache1_->insert(2006, 2, 2006);
    cache1_->insert(2007, 2, 2007);
    cache1_->insert(2008, 2, 2008);
    cache1_->insert(2009, 0, 2009);
    CPPUNIT_ASSERT_EQUAL(9, cache1_->size());
    

}

void LRUComplexCacheTest::testLRUPolicy()
{
    // Fill the cache and lookup all entries
    cache2_->insert(2001, 0, 1);
    cache2_->insert(2002, 6, 2);
    cache2_->insert(2003, 5, 3);
    cache2_->insert(2004, 0, 4);
    cache2_->insert(2005, 6, 5);
    CPPUNIT_ASSERT(!cache2_->lookup(2005,0,2));
    CPPUNIT_ASSERT_EQUAL(2005, cache2_->lookup(2005,7,3)->handle);
    cache2_->insert(2006, 2, 6);
    CPPUNIT_ASSERT(!cache2_->lookup(2001,7,2));
    CPPUNIT_ASSERT_EQUAL(5, cache2_->size());
    cache2_->insert(2007, 2, 7);
    cache2_->insert(2008, 2, 8);
    cache2_->insert(2009, 0, 9);
    CPPUNIT_ASSERT(!cache2_->lookup(2004,7,2));
    CPPUNIT_ASSERT_EQUAL(5, cache2_->size());
    
}

void LRUComplexCacheTest::testCombinePolicy()
{
    cache1_->insert(653, 1, 20);
    //cache1_->removeOffset(655,2);
    cache1_->insert(653, 2, 2);
    cache1_->insert(653, 30, 10);
    cache1_->insert(653, 34, 50);
    cache1_->insert(653, 42, 30);
    cache1_->insert(56, 3, 2);
    cache1_->insert(655, 420, 30);
    //cache1_->removeOffset(655,2);
    CPPUNIT_ASSERT_EQUAL(3, cache1_->size());
    
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
