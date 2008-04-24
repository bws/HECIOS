//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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
#include <cppunit/TextTestRunner.h>
#include "bmi_direct_endpoint_test.h"
#include "bmi_tcp_client_test.h"
#include "bmi_tcp_endpoint_test.h"
#include "bmi_tcp_server_test.h"
#include "mpi_tcp_client_test.h"

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;

    // Add all of the subsystem tests
    runner.addTest( BMIDirectEndpointTest::suite() );
    runner.addTest( BMITcpClientTest::suite() );
    //runner.addTest( BMITcpEndpointTest::suite() );
    runner.addTest( BMITcpServerTest::suite() );
    runner.addTest( MPITcpClientTest::suite() );

    bool success = runner.run();
    return (success ? 0 : 1);
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
