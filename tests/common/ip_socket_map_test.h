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
    //CPPUNIT_FAIL("no delete test yet");
    string ip5 = "ip5";
    TCPSocket* sock5 = new TCPSocket();

    IPSocketMap socketMap5;
    socketMap5.addSocket(ip5, sock5);
    socketMap5.removeSocket(ip5);
    CPPUNIT_ASSERT_EQUAL(sock5, socketMap5.getSocket(ip5));
}

void IPSocketMapTest::testGetSocket()
{
    // Create an ip and socket
    string ip6 = "ip6";
    TCPSocket* sock6 = new TCPSocket();

    //CPPUNIT_FAIL("no get test yet");
    IPSocketMap socketMap6;
    socketMap6.addSocket(ip6, sock6);
    CPPUNIT_ASSERT_EQUAL(sock6, socketMap6.getSocket(ip6));

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
