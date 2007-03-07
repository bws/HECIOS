#ifndef LRU_TIMEOUT_CACHE_TEST_H
#define LRU_TIMEOUT_CACHE_TEST_H

#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "lru_timeout_cache.h"
using namespace std;

/** Unit test for UMDIOTrace */
class LRUTimeoutCacheTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(LRUTimeoutCacheTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testRemove);
    CPPUNIT_TEST(testLookup);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testLRUPolicy);
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

private:
    LRUTimeoutCache<int, std::string>* cache1_;
    LRUTimeoutCache<int, std::string>* cache2_;
};

void LRUTimeoutCacheTest::setUp()
{
    cache1_ = new LRUTimeoutCache<int, std::string>(10, 10.0);
    cache2_ = new LRUTimeoutCache<int, std::string>(2, 10.0);
}

void LRUTimeoutCacheTest::tearDown()
{
    delete cache1_;
    cache1_ = 0;
    delete cache2_;
    cache2_ = 0;
}

void LRUTimeoutCacheTest::testConstructor()
{
    LRUTimeoutCache<int, int> cache(1, 1.0);
    CPPUNIT_ASSERT(true);
}

void LRUTimeoutCacheTest::testInsert()
{
    LRUTimeoutCache <int, int> cache(1, 1.0);
    cache.insert(1,1);
    
    cache1_->insert(1, "value1");
    cache1_->insert(2, "Value2");
        
    cache2_->insert(101, "Value101");
    cache2_->insert(102, "Value102");
    CPPUNIT_ASSERT(true);
}

void LRUTimeoutCacheTest::testRemove()
{
    // Remove from an empty cache
    cache1_->remove(77);
    CPPUNIT_ASSERT(true);

    // Remove an existing entry from a cache
    cache1_->insert(63, "value63");
    cache1_->remove(63);
    CPPUNIT_ASSERT(true);
}

void LRUTimeoutCacheTest::testLookup()
{
    // Lookup an entry in an empty cache
    LRUTimeoutCache<int,string>::EntryType* e77 = cache1_->lookup(77);
    CPPUNIT_ASSERT(0 == e77);

    // Find an existing entry
    cache1_->insert(74, "value74");
    LRUTimeoutCache<int,string>::EntryType* e74 = cache1_->lookup(74);
    CPPUNIT_ASSERT("value74" == e74->data);
}

void LRUTimeoutCacheTest::testSize()
{
    // Check the size of an empty cache
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Check the size of cache after insert
    cache1_->insert(74, "value74");
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());

    // Check the size of cache after remove
    cache1_->remove(74);
    CPPUNIT_ASSERT_EQUAL(0, cache1_->size());

    // Check the size after duplicate insertion
    cache1_->insert(640, "val640");
    cache1_->insert(640, "val640");
    CPPUNIT_ASSERT_EQUAL(1, cache1_->size());

    // Check the size after maximum fill
    cache1_->insert(640, "val640");
    cache1_->insert(641, "val641");
    cache1_->insert(642, "val642");
    cache1_->insert(643, "val643");
    cache1_->insert(644, "val644");
    cache1_->insert(645, "val645");
    cache1_->insert(646, "val646");
    cache1_->insert(647, "val647");
    cache1_->insert(648, "val648");
    cache1_->insert(649, "val649");
    cache1_->insert(650, "val650");
    cache1_->insert(651, "val651");
    cache1_->insert(652, "val652");
    cache1_->insert(653, "val653");
    CPPUNIT_ASSERT_EQUAL(10, cache1_->size());
}

void LRUTimeoutCacheTest::testLRUPolicy()
{
    CPPUNIT_ASSERT(false);
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
