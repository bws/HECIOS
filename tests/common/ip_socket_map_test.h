#ifndef IP_SOCKET_MAP_TEST_H
#define IP_SOCKET_MAP_TEST_H

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
    socketMap1.addSocket(ip1, sock1);
    CPPUNIT_ASSERT_EQUAL(sock1, socketMap1.getSocket(ip1));

    // Test for multiple additions
    string ip2 = "ip2";
    TCPSocket* sock2 = new TCPSocket();
    string ip3 = "ip3";
    TCPSocket* sock3 = new TCPSocket();

    IPSocketMap socketMap2;
    socketMap2.addSocket(ip2, sock2);
    socketMap2.addSocket(ip3, sock3);
    CPPUNIT_ASSERT_EQUAL(sock2, socketMap2.getSocket(ip2));
    CPPUNIT_ASSERT_EQUAL(sock3, socketMap2.getSocket(ip3));

    // Test for identical additions
    string ip4 = "ip4";
    TCPSocket* sock4 = new TCPSocket();
    IPSocketMap socketMap3;
    socketMap3.addSocket(ip4, sock4);
    socketMap3.addSocket(ip4, sock4);
    CPPUNIT_ASSERT_EQUAL(sock4, socketMap3.getSocket(ip4));
}

void IPSocketMapTest::testDeleteSocket()
{
    // Create a null socket
    TCPSocket* nullSocket = 0;
    
    // Test for socket stored in map
    string ip5 = "ip5";
    TCPSocket* sock5 = new TCPSocket();
    IPSocketMap socketMap5;
    socketMap5.addSocket(ip5, sock5);
    socketMap5.removeSocket(ip5);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap5.getSocket(ip5));

    // Test for socket not stored in map
    string ip6 = "ip6";
    IPSocketMap socketMap6;
    socketMap6.removeSocket(ip6);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap6.getSocket(ip6));

    // Test for multiple deletions
    string ip7 = "ip7";
    string ip8 = "ip8";
    TCPSocket* sock7 = new TCPSocket();
    TCPSocket* sock8 = new TCPSocket();
    IPSocketMap socketMap7;
    socketMap7.addSocket(ip7, sock7);
    socketMap7.addSocket(ip8, sock8);
    socketMap7.removeSocket(ip7);
    socketMap7.removeSocket(ip8);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap7.getSocket(ip7));
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap7.getSocket(ip8));

    // Test for identical deletions
    string ip9 = "ip9";
    TCPSocket* sock9 = new TCPSocket();
    IPSocketMap socketMap9;
    socketMap9.addSocket(ip9, sock9);
    socketMap9.removeSocket(ip9);
    socketMap9.removeSocket(ip9);
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap9.getSocket(ip9));
}

void IPSocketMapTest::testGetSocket()
{
    // Create a null socket
    TCPSocket* nullSocket = 0;
    
    // Test for socket stored in map
    string ip10 = "ip10";
    TCPSocket* sock10 = new TCPSocket();
    IPSocketMap socketMap10;
    socketMap10.addSocket(ip10, sock10);
    CPPUNIT_ASSERT_EQUAL(sock10, socketMap10.getSocket(ip10));

    // Test for socket not stored in map
    string ip11 = "ip11";
    TCPSocket* sock11 = new TCPSocket();
    IPSocketMap socketMap11;
    socketMap11.addSocket(ip11, sock11);
    string ip20 = "ip20";
    CPPUNIT_ASSERT_EQUAL( nullSocket, socketMap11.getSocket(ip20));

    // Test for multiple retrieve
    string ip12 = "ip12";
    string ip13 = "ip13";
    TCPSocket* sock12 = new TCPSocket();
    TCPSocket* sock13 = new TCPSocket();
    IPSocketMap socketMap12;
    socketMap12.addSocket(ip12, sock12);
    socketMap12.addSocket(ip13, sock13);
    CPPUNIT_ASSERT_EQUAL( sock12, socketMap12.getSocket(ip12));
    CPPUNIT_ASSERT_EQUAL( sock13, socketMap12.getSocket(ip13));
    
    // Test for identical retrieve
    string ip14 = "ip14";
    TCPSocket* sock14 = new TCPSocket();
    IPSocketMap socketMap14;
    socketMap14.addSocket(ip14, sock14);
    CPPUNIT_ASSERT_EQUAL( sock14, socketMap14.getSocket(ip14));
    CPPUNIT_ASSERT_EQUAL( sock14, socketMap14.getSocket(ip14));
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
