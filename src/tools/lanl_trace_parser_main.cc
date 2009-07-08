//
// This file is part of Hecios
//
// Copyright (C) 2008 Wu Yang
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <iostream>
#include "lanl_trace_parser.h"
using namespace std;

int main(int argc, char** argv)
{
    // Do some basic validation
    if (argc != 4)
    {
        cerr << "Syntax: " << argv[0] << " <src_trace> <dst_dir> <rank>" << endl;
        return 1;
    }

    string traceFilename = argv[1];
    string outputDirectory = argv[2];
    string rank = argv[3];
    LanlTraceParser ltp(traceFilename, outputDirectory, rank);
    ltp.startProc();
    return 0;
}
