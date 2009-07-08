#ifndef IP_SOCKET_MAP_TEST_H
#define IP_SOCKET_MAP_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include <omnetpp.h>
#include "GenericAppMsg_m.h"
#include "ip_socket_map.h"
using namespace std;

/** Unit test for IPSocketMap */
class IPSocketMapTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(IPSocketMapTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testAddSocket);
    CPPUNIT_TEST(testDeleteSocket);
    CPPUNIT_TEST(testGetSocket);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp() {};

    /** Called after each test function */
    void tearDown() {};

    void testConstructor();

    void testAddSocket();

    void testDeleteSocket();

    void testGetSocket();

};

void IPSocketMapTest::testConstructor()
{
    IPSocketMap socketMap;
    CPPUNIT_ASSERT(true);
}

void IPSocketMapTest::testAddSocket()
{
    // Create an ip and socket
    string ip1 = "ip1";
    TCPSocket* sock1 = new TCPSocket();

    // Test for a single addition
    IPSocketMap socketMap1;
    socketMap1.addSocket(ip1, 600, sock1);
    CPPUNIT_ASSERT_EQUAL(sock1, socketMap1.getSocket(ip1, 600));

    // Test for multiple additions
    string ip2 = "ip2";
    TCPSocket* sock2 = new TCPSocket();
    string ip3 = "ip3";
    TCPSocket* sock3 = new TCPSocket();

    IPSocketMap socketMap2;
    socketMap2.addSocket(ip2, 600, sock2);
    socketMap2.addSocket(ip3, 300, sock3);
    CPPUNIT_ASSERT_EQUAL(sock2, socketMap2.getSocket(ip2, 600));
    CPPUNIT_ASSERT_EQUAL(sock3, socketMap2.getSocket(ip3, 300));

    // Test for identical additions
    string ip4 = "ip4";
    TCPSocket* sock4 = new TCPSocket();
    IPSocketMap socketMap3;
    socketMap3.addSocket(ip4, 80, sock4);
    socketMap3.addSocket(ip4, 80, sock4);
    CPPUNIT_ASSERT_EQUAL(sock4, socketMap3.getSocket(ip4, 80));
}

void IPSocketMapTest::testDeleteSocket()
{
    // Create a null socket
    TCPSocket* nullSocket = 0;

    // Test for socket stored in map
    string ip5 = "ip5";
    TCPSocket* sock5 = new TCPSocket();
    IPSocketMap socketMap5;
    socketMap5.addSocket(ip5, 5, sock5);
    socketMap5.removeSocket(ip5, 5);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap5.getSocket(ip5, 5));

    // Test for socket not stored in map
    string ip6 = "ip6";
    IPSocketMap socketMap6;
    socketMap6.removeSocket(ip6, 6);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap6.getSocket(ip6, 6));

    // Test for multiple deletions
    string ip7 = "ip7";
    string ip8 = "ip8";
    TCPSocket* sock7 = new TCPSocket();
    TCPSocket* sock8 = new TCPSocket();
    IPSocketMap socketMap7;
    socketMap7.addSocket(ip7, 7, sock7);
    socketMap7.addSocket(ip8, 8, sock8);
    socketMap7.removeSocket(ip7, 7);
    socketMap7.removeSocket(ip8, 8);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap7.getSocket(ip7, 7));
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap7.getSocket(ip8, 8));

    // Test for identical deletions
    string ip9 = "ip9";
    TCPSocket* sock9 = new TCPSocket();
    IPSocketMap socketMap9;
    socketMap9.addSocket(ip9, 9, sock9);
    socketMap9.removeSocket(ip9, 9);
    socketMap9.removeSocket(ip9, 9);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap9.getSocket(ip9, 9));
}

void IPSocketMapTest::testGetSocket()
{
    // Create a null socket
    TCPSocket* nullSocket = 0;

    // Test for socket stored in map
    string ip10 = "ip10";
    TCPSocket* sock10 = new TCPSocket();
    IPSocketMap socketMap10;
    socketMap10.addSocket(ip10, 1, sock10);
    CPPUNIT_ASSERT_EQUAL(sock10, socketMap10.getSocket(ip10, 1));

    // Test for socket not stored in map
    string ip11 = "ip11";
    TCPSocket* sock11 = new TCPSocket();
    IPSocketMap socketMap11;
    socketMap11.addSocket(ip11, 2, sock11);
    string ip20 = "ip20";
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap11.getSocket(ip20, 80));

    // Test for multiple retrieve
    string ip12 = "ip12";
    string ip13 = "ip13";
    TCPSocket* sock12 = new TCPSocket();
    TCPSocket* sock13 = new TCPSocket();
    IPSocketMap socketMap12;
    socketMap12.addSocket(ip12, 1, sock12);
    socketMap12.addSocket(ip13, 1, sock13);
    CPPUNIT_ASSERT_EQUAL( sock12, socketMap12.getSocket(ip12, 1));
    CPPUNIT_ASSERT_EQUAL( sock13, socketMap12.getSocket(ip13, 1));

    // Test for identical retrieve
    string ip14 = "ip14";
    TCPSocket* sock14 = new TCPSocket();
    IPSocketMap socketMap14;
    socketMap14.addSocket(ip14, 10, sock14);
    CPPUNIT_ASSERT_EQUAL( sock14, socketMap14.getSocket(ip14, 10));
    CPPUNIT_ASSERT_EQUAL( sock14, socketMap14.getSocket(ip14, 10));
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
