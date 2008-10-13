#include <iostream>
#include "parser_sm.h"
using namespace std;

int main(int argc, char** argv)
{
    int rc = 0;

    if (argc != 4)
    {
        cerr << "Syntax: " << argv[0] << " <src_trace> <dst_dir> <rank>" << endl;
        return -1;
    }

    PaserSm psm(argv[1], argv[2], argv[3]);
    psm.startProc();
    return rc;
}
