//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include "multi_cache.h"

class MultiCacheTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MultiCacheTest);
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

void MultiCacheTest::setUp()
{
}

void MultiCacheTest::tearDown()
{
}

void MultiCacheTest::testCapacity()
{
    MultiCache cache1(10);
    CPPUNIT_ASSERT_EQUAL(size_t(10), cache1.capacity());

    MultiCache cache2(44);
    CPPUNIT_ASSERT_EQUAL(size_t(44), cache2.capacity());
}

void MultiCacheTest::testSize()
{
    MultiCache cache1(10);
    CPPUNIT_ASSERT_EQUAL(size_t(0), cache1.size());

    // Check that insertion modifies size correctly
    MultiCache::Page page1;
    MultiCache::Key outKey(Filename("/out"), 1000);
    MultiCache::Page* outPage;
    bool outDirtyStatus;
    try {
        MultiCache::Key key1(Filename("/"), 1);
        cache1.insertFullPageAndRecall(key1, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(1), cache1.size());
    } catch(...) {}

    try {
        MultiCache::Key key2(Filename("/"), 2);
        cache1.insertFullPageAndRecall(key2, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(2), cache1.size());
    } catch(...) {}

    try {
        MultiCache::Key key3(Filename("/"), 3);
        cache1.insertFullPageAndRecall(key3, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(3), cache1.size());
    } catch(...) {}

    try {
        MultiCache::Key key4(Filename("/"), 4);
        cache1.insertFullPageAndRecall(key4, page1, false, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(4), cache1.size());

        MultiCache::PartialPage partialPage;
        cache1.insertDirtyPartialPageAndRecall(key4, partialPage, outKey, outPage, outDirtyStatus);
        CPPUNIT_ASSERT_EQUAL(size_t(5), cache1.size());
    } catch(...) {}
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
