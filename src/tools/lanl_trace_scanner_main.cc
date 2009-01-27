//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "lanl_trace_scan_actions.h"
#include "phtf_io_trace.h"
using namespace std;

// Extern declarations to invoke lex
extern "C" FILE* yyin;
extern "C" void yylex();

void emitRuntimeFile(const string& runtimeFilename)
{
    //ofstream runtimeFile(runtimeFilename.c_str());
    //runtimeFile.close();
}

void emitFileSystemConfigFile(const string& configFilename,
                              const set<string>& filenames)
{
    // Construct the file system configuration
    PHTFFs* fs = new PHTFFs(configFilename, true);

    // Constant values representing Builtin MPI Datatypes
    fs->consts("0x4c000010", "0");
    fs->consts("0x4c000011", "0");
    fs->consts("0x4c000101", "1");
    fs->consts("0x4c000102", "1");
    fs->consts("0x4c00010d", "1");
    fs->consts("0x4c00010f", "1");
    fs->consts("0x4c000118", "1");
    fs->consts("0x4c00011a", "1");
    fs->consts("0x4c00012d", "1");
    fs->consts("0x4c000203", "2");
    fs->consts("0x4c000204", "2");
    fs->consts("0x4c00022f", "2");
    fs->consts("0x4c000405", "4");
    fs->consts("0x4c000406", "4");
    fs->consts("0x4c00040a", "4");
    fs->consts("0x4c00040e", "4");
    fs->consts("0x4c00041b", "4");
    fs->consts("0x4c00041c", "4");
    fs->consts("0x4c00041d", "4");
    fs->consts("0x4c000427", "4");
    fs->consts("0x4c000430", "4");
    fs->consts("0x4c000807", "8");
    fs->consts("0x4c000808", "8");
    fs->consts("0x4c000809", "8");
    fs->consts("0x4c00080b", "8");
    fs->consts("0x4c000816", "8");
    fs->consts("0x4c000819", "8");
    fs->consts("0x4c00081e", "8");
    fs->consts("0x4c00081f", "8");
    fs->consts("0x4c000820", "8");
    fs->consts("0x4c000821", "8");
    fs->consts("0x4c000828", "8");
    fs->consts("0x4c000829", "8");
    fs->consts("0x4c000831", "8");
    fs->consts("0x4c00100c", "16");
    fs->consts("0x4c001022", "16");
    fs->consts("0x4c001023", "16");
    fs->consts("0x4c001024", "16");
    fs->consts("0x4c00102a", "16");
    fs->consts("0x4c00102b", "16");
    fs->consts("0x4c002025", "32");
    fs->consts("0x4c00202c", "32");
    fs->consts("0x8c000000", "8");
    fs->consts("0x8c000001", "12");
    fs->consts("0x8c000002", "12");
    fs->consts("0x8c000003", "6");
    fs->consts("0x8c000004", "20");

    // Other global constants
    fs->consts("MPI_COMM_WORLD", "0x44000000");
    fs->consts("MPI_COMM_SELF", "0x44000001");
    fs->consts("MPI_SEEK_SET", "600");
    fs->consts("MPI_SEEK_CUR", "602");
    fs->consts("MPI_SEEK_END", "604");

    // Add filenames to the configuration
    set<string>::const_iterator first = filenames.begin();
    set<string>::const_iterator last = filenames.end();
    while (first != last)
    {
        // Make up an arbitrary size for the file
        // (16 MB * 128 clients to start with)
        uint64_t fileSize = uint64_t(16777216) * uint64_t(128);
        ostringstream oss;
        oss << fileSize;

        fs->addFile(*first, oss.str());
        first++;
    }
}

int main(int argc, char** argv)
{
    int rc = 0;
    // Do some basic validation
    if (argc != 4)
    {
        cerr << "ERROR: Invalid arguments." << endl;
        cerr << "Usage: " << argv[0] << " <src_trace> <dst_dir> <rank>" << endl;
        return 1;
    }

    // Retrieve arguments
    string traceFilename = argv[1];
    string outputDirectory = argv[2];
    string rankString = argv[3];

    // Open the trace file and extract the epoch time
    yyin = fopen(traceFilename.c_str(), "r");
    if (0 == yyin)
    {
        cerr << "ERROR: Unable to open trace file: " << traceFilename << endl;
        _Exit(2);
    }

    char timestamp[16] = {0};
    fread(timestamp, 15, 1, yyin);
    timestamp[15] = '\0';
    LanlTraceScanActions::instance().setEpochTime(timestamp);

    // Reset the file pointer to first location and begin scan
    fseek(yyin, 0, SEEK_SET);
    yylex();

    // Create the output directory if necessary
    int success = access(outputDirectory.c_str(), X_OK);
    if (0 != success)
    {
        cerr << "WARNING: Creating output directory: " << outputDirectory << endl;
        success = mkdir(outputDirectory.c_str(), 755);
        if (0 != success)
        {
            cerr << "ERROR: Unable to create directory: "
                 << outputDirectory << endl;
            _Exit(3);
        }
    }

    // Create the output event file
    string eventFilename = outputDirectory + "/event." + rankString;
    ofstream eventFile(eventFilename.c_str());
    LanlTraceScanActions::instance().emitTraceCalls(eventFile);
    eventFile.close();

    // Create the runtime tract file
    string runtimeFilename = outputDirectory + "/runtime." + rankString;
    emitRuntimeFile(runtimeFilename);

    // Create the output file system configuration file
    emitFileSystemConfigFile(outputDirectory + "/fs.ini",
                             LanlTraceScanActions::instance().getFilenames());

    return rc;
}
