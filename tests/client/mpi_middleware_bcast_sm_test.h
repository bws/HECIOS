#ifndef MPI_MIDDLEWARE_BCAST_SM_TEST_H
#define MPI_MIDDLEWARE_BCAST_SM_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "mpi_middleware_bcast_sm.h"
#include "mpi_middleware.h"
#include "mpi_mid_m.h"
#include "comm_man.h"

class MPIMiddlewareBcastSmTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MPIMiddlewareBcastSmTest);
    CPPUNIT_TEST(testInit);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void testInit();

private:
    MpiMiddleware mid;
};

void MPIMiddlewareBcastSmTest::setUp()
{
    
}

void MPIMiddlewareBcastSmTest::tearDown()
{
}

void MPIMiddlewareBcastSmTest::testInit()
{
    MPIMidBcastSM * bcastsm = new MPIMidBcastSM(&mid);
    spfsMPIMidBcastRequest *req = new spfsMPIMidBcastRequest(0, SPFS_MPIMID_BCAST_REQUEST);
    req->setRoot(0);
    req->setCommunicator(MPI_COMM_WORLD);
    bcastsm->handleMessage(req);
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
