#ifndef LRU_CACHE_TEST_H
#define LRU_CACHE_TEST_H

#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "lru_cache.h"
using namespace std;

/** Unit test for LRUCache */
class LRUCacheTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(LRUCacheTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testRemove);
    CPPUNIT_TEST(testSetDirtyBit);
    CPPUNIT_TEST(testLookup);
    CPPUNIT_TEST(testCapacity);
    CPPUNIT_TEST(testGetDirtyEntries);
    CPPUNIT_TEST(testGetLRU);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testLRUPolicy);
    CPPUNIT_TEST(testPercentDirty);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testInsert();

    void testRemove();

    void testSetDirtyBit();

    void testLookup();

    void testCapacity();

    void testGetDirtyEntries();

    void testGetLRU();

    void testSize();

    void testLRUPolicy();

    void testPercentDirty();

private:
    LRUCache<int, std::string>* cache1_;
    LRUCache<int, std::string>* cache2_;
};

void LRUCacheTest::setUp()
{
    cache1_ = new LRUCache<int, std::string>(10);
    cache2_ = new LRUCache<int, std::string>(2);
}

void LRUCacheTest::tearDown()
{
    delete cache1_;
    cache1_ = 0;
    delete cache2_;
    cache2_ = 0;
}

void LRUCacheTest::testConstructor()
{
    LRUCache<int, int> cache(1);
    CPPUNIT_ASSERT_EQUAL((size_t)0, cache.size());
}

void LRUCacheTest::testInsert()
{
    LRUCache<int, int> cache(1);
    cache.insert(1,1);

    cache1_->insert(1, "value1");
    cache1_->insert(2, "Value2");
    CPPUNIT_ASSERT_EQUAL((size_t)2, cache1_->size());

    cache2_->insert(101, "Value101");
    cache2_->insert(102, "Value102");
    CPPUNIT_ASSERT_EQUAL((size_t)2, cache2_->size());
}

void LRUCacheTest::testRemove()
{
    // Remove from an empty cache
    CPPUNIT_ASSERT_EQUAL((size_t)0, cache1_->size());
    CPPUNIT_ASSERT_THROW(cache1_->remove(77), NoSuchEntry);

    // Remove an existing entry from a cache
    cache1_->insert(63, "value63");
    cache1_->remove(63);
    CPPUNIT_ASSERT_EQUAL((size_t)0, cache1_->size());
}

void LRUCacheTest::testSetDirtyBit()
{
    LRUCache<int, int> cache(10);
    cache.insert(1, 1, false);
    cache.insert(2, 1, false);
    cache.insert(3, 1, false);
    cache.insert(4, 1, false);

    vector<int> dirtyEntries = cache.getDirtyEntries();
    CPPUNIT_ASSERT_EQUAL(size_t(0), dirtyEntries.size());

    cache.setDirtyBit(3, true);
    dirtyEntries = cache.getDirtyEntries();
    CPPUNIT_ASSERT_EQUAL(size_t(1), dirtyEntries.size());
    CPPUNIT_ASSERT_EQUAL(3, dirtyEntries[0]);
}

void LRUCacheTest::testLookup()
{
    // Lookup an entry in an empty cache
    CPPUNIT_ASSERT_THROW(cache1_->lookup(77), NoSuchEntry);

    // Find an existing entry
    cache1_->insert(2000, "value2000");
    string e2000 = cache1_->lookup(2000);
    CPPUNIT_ASSERT_EQUAL(string("value2000"), e2000);

    // Fill the cache and lookup all entries
    cache1_->insert(2001, "value2001");
    cache1_->insert(2002, "value2002");
    cache1_->insert(2003, "value2003");
    cache1_->insert(2004, "value2004");
    cache1_->insert(2005, "value2005");
    cache1_->insert(2006, "value2006");
    cache1_->insert(2007, "value2007");
    cache1_->insert(2008, "value2008");
    cache1_->insert(2009, "value2009");

    CPPUNIT_ASSERT_EQUAL(string("value2000"), cache1_->lookup(2000));

    CPPUNIT_ASSERT_EQUAL(string("value2001"), cache1_->lookup(2001));

    CPPUNIT_ASSERT_EQUAL(string("value2002"), cache1_->lookup(2002));

    CPPUNIT_ASSERT_EQUAL(string("value2003"), cache1_->lookup(2003));

    CPPUNIT_ASSERT_EQUAL(string("value2004"), cache1_->lookup(2004));

    CPPUNIT_ASSERT_EQUAL(string("value2005"), cache1_->lookup(2005));

    CPPUNIT_ASSERT_EQUAL(string("value2006"), cache1_->lookup(2006));

    CPPUNIT_ASSERT_EQUAL(string("value2007"), cache1_->lookup(2007));

    CPPUNIT_ASSERT_EQUAL(string("value2008"), cache1_->lookup(2008));

    CPPUNIT_ASSERT_EQUAL(string("value2009"), cache1_->lookup(2009));

    // Lookup a non-existant entry
    CPPUNIT_ASSERT_THROW(cache1_->lookup(2010), NoSuchEntry);
}

void LRUCacheTest::testCapacity()
{
    LRUCache<int,int> cache1(10);
    CPPUNIT_ASSERT_EQUAL((size_t)10, cache1.capacity());

    LRUCache<int,int> cache2(20);
    CPPUNIT_ASSERT_EQUAL((size_t)20, cache2.capacity());
}

void LRUCacheTest::testGetDirtyEntries()
{
    LRUCache<int,int> cache(10);
    cache.insert(1, 1, false);
    cache.insert(2, 1, true);
    cache.insert(3, 1, true);
    cache.insert(4, 1, false);

    vector<int> dirtyEntries = cache.getDirtyEntries();
    CPPUNIT_ASSERT_EQUAL(size_t(2), dirtyEntries.size());
    CPPUNIT_ASSERT_EQUAL(2, dirtyEntries[0]);
    CPPUNIT_ASSERT_EQUAL(3, dirtyEntries[1]);
}

void LRUCacheTest::testGetLRU()
{
    LRUCache<int,int> cache1(2);

    // Test next eviction on an empty cache
    CPPUNIT_ASSERT_THROW(cache1.getLRU(), NoSuchEntry);

    // Test next eviction when cache has available capacity
    cache1.insert(1, 100);
    CPPUNIT_ASSERT_EQUAL(1, cache1.getLRU().first);
    CPPUNIT_ASSERT_EQUAL(100, cache1.getLRU().second);

    // Test next eviction
    cache1.insert(2, 200);
    CPPUNIT_ASSERT_EQUAL(1, cache1.getLRU().first);
    CPPUNIT_ASSERT_EQUAL(100, cache1.getLRU().second);

    cache1.insert(3, 300);
    CPPUNIT_ASSERT_EQUAL(2, cache1.getLRU().first);
    CPPUNIT_ASSERT_EQUAL(200, cache1.getLRU().second);
}

void LRUCacheTest::testSize()
{
    // Check the size of an empty cache
    CPPUNIT_ASSERT_EQUAL((size_t)0, cache1_->size());

    // Check the size of cache after insert
    cache1_->insert(74, "value74");
    CPPUNIT_ASSERT_EQUAL((size_t)1, cache1_->size());

    // Check the size of cache after remove
    cache1_->remove(74);
    CPPUNIT_ASSERT_EQUAL((size_t)0, cache1_->size());

    // Check the size after duplicate insertion
    cache1_->insert(640, "val640");
    cache1_->insert(640, "val640");
    CPPUNIT_ASSERT_EQUAL((size_t)1, cache1_->size());

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
    CPPUNIT_ASSERT_EQUAL((size_t)10, cache1_->size());
}

void LRUCacheTest::testLRUPolicy()
{
    // Fill the cache
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

    // Test LRU after inserts
    cache1_->insert(650, "val650");
    CPPUNIT_ASSERT(cache1_->exists(650));
    CPPUNIT_ASSERT(!cache1_->exists(640));

    // Test LRU after lookups
    cache1_->lookup(641);
    cache1_->insert(651, "val651");
    CPPUNIT_ASSERT(!cache1_->exists(642));
    CPPUNIT_ASSERT(cache1_->exists(641));
    CPPUNIT_ASSERT(cache1_->exists(651));

    // Test LRU after removes
    cache1_->remove(643);
    CPPUNIT_ASSERT(!cache1_->exists(643));

    cache1_->insert(652, "val652");
    cache1_->insert(653, "val653");
    CPPUNIT_ASSERT(!cache1_->exists(644));
    CPPUNIT_ASSERT(cache1_->exists(652));
    CPPUNIT_ASSERT(cache1_->exists(653));
}

void LRUCacheTest::testPercentDirty()
{
    // Check the beginning cache
    CPPUNIT_ASSERT_EQUAL(0.0, cache1_->percentDirty());

    // Fill the cache
    cache1_->insert(640, "val640", true);
    cache1_->insert(641, "val641", true);
    cache1_->insert(642, "val642", true);
    cache1_->insert(643, "val643", true);
    cache1_->insert(644, "val644", true);
    cache1_->insert(645, "val645", true);
    cache1_->insert(646, "val646", true);
    cache1_->insert(647, "val647", true);
    cache1_->insert(648, "val648", true);
    cache1_->insert(649, "val649", true);

    // Test percent dirty after inserts
    CPPUNIT_ASSERT_EQUAL(1.0, cache1_->percentDirty());

    // Test percent dirty after non-dirty insertion
    cache1_->insert(650, "val650", false);
    CPPUNIT_ASSERT_EQUAL(9.0/10.0, cache1_->percentDirty());

    // Test percent dirty after non-dirty insertion
    cache1_->insert(651, "val651", false);
    cache1_->insert(651, "val651", false);
    CPPUNIT_ASSERT_EQUAL(8.0/10.0, cache1_->percentDirty());

    // Test percent dirty after non-dirty insertion
    cache1_->insert(652, "val652", false);
    CPPUNIT_ASSERT_EQUAL(7.0/10.0, cache1_->percentDirty());

    // Test percent dirty after dirty insertion
    cache1_->insert(653, "val653", false);
    CPPUNIT_ASSERT_EQUAL(6.0/10.0, cache1_->percentDirty());
    cache1_->insert(653, "val653", true);
    CPPUNIT_ASSERT_EQUAL(7.0/10.0, cache1_->percentDirty());
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
