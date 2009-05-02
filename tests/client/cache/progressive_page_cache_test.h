#ifndef PROGRESSIVE_PAGE_CACHE_TEST_H
#define PROGRESSIVE_PAGE_CACHE_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
#include <cppunit/extensions/HelperMacros.h>
#include "progressive_page_cache.h"

class ProgressivePageCacheTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ProgressivePageCacheTest);
    CPPUNIT_TEST(testCapacity);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Pre test code */
    void setUp();

    /** Post test code */
    void tearDown();

    void testCapacity();
    void testSize();
};

void ProgressivePageCacheTest::setUp()
{
}

void ProgressivePageCacheTest::tearDown()
{
}

void ProgressivePageCacheTest::testCapacity()
{
    ProgressivePageCache cache1(10);
    CPPUNIT_ASSERT_EQUAL(size_t(10), cache1.capacity());

    ProgressivePageCache cache2(44);
    CPPUNIT_ASSERT_EQUAL(size_t(44), cache2.capacity());
}

void ProgressivePageCacheTest::testSize()
{
    ProgressivePageCache cache1(10);
    CPPUNIT_ASSERT_EQUAL(size_t(0), cache1.size());

    // Check that insertion modifies size correctly
    ProgressivePageCache::Page page1;
    ProgressivePageCache::Key outKey(Filename("/out"), 1000);
    ProgressivePageCache::Page* outPage;
    bool outDirtyStatus;
    try {
        ProgressivePageCache::Key key1(Filename("/"), 1);
        cache1.insertPageAndRecall(key1, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(1), cache1.size());
    } catch(...) {}

    try {
        ProgressivePageCache::Key key2(Filename("/"), 2);
        cache1.insertPageAndRecall(key2, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(2), cache1.size());
    } catch(...) {}

    try {
        ProgressivePageCache::Key key3(Filename("/"), 3);
        cache1.insertPageAndRecall(key3, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(3), cache1.size());
    } catch(...) {}

    try {
        ProgressivePageCache::Key key4(Filename("/"), 4);
        cache1.insertPageAndRecall(key4, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(4), cache1.size());

        ProgressivePageCache::Page partialPage;
        cache1.insertPageAndRecall(key4, partialPage, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(5), cache1.size());
    } catch(...) {}
}

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
