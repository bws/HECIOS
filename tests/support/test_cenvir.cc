//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
/**
 * This is a replacement for the standard OmNet++ environment so that we can
 * more easily test our code without all the gui and recording scaffolding
 * that is included in the OmNet environment.  By default, most of the
 * environment functions do not have a valid implementation.  This will
 * sometime mean that code will not work.  At that time, you will need
 * to supply a *working* implementation so that progress can continue.
 *
 * Sufficient implementation of most of these features should be relatively
 * trivial.
 *
 */

#include "cenvir.h"
#include "cdefaultlist.h"
#include "csimulation.h"
#include "onstartup.h"
using namespace std;

/** Global objects */
//cEnvir ev;
cStaticFlag* staticFlag;
static cDefaultList g_defaultList("globalTestingDefaultList");

/** Some weird dummy function that is apparently required */
void envirDummy()
{
}

/** Default constructor */
cEnvir::cEnvir()
//    : ostream(&ev_buf),
//    ev_buf(this)
{
    // Turn off garbage collection (see cdefaultlist.cc)
    //cDefaultList::doGC = false;

    // Set the static flag (see cobject.h)
    //
    // Necessary to create cDefaultList type required to construct modules
    // I am not using it the way the name implies it should work because
    // apparently it is named poorly
    //
    staticFlag = new cStaticFlag();

    // Initialize the simulation
//    simulation.init();

    // construct global lists
    ExecuteOnStartup::executeAll();
}

/** Destructor */
cEnvir::~cEnvir()
{
    delete staticFlag;
    staticFlag = 0;
}

/** */
void cEnvir::sputn(const char *s, int n)
{
    ::fwrite(s,1,n,stdout);
}

/** Provide a unique number implementation */
unsigned long cEnvir::getUniqueNumber()
{
    static int seed = 0u;
    return ++seed;
}

/** Not needed for test drivers */
void cEnvir::connectionCreated(cGate *srcgate)
{
    //cerr << "Connection created." << endl;
}

/** Not needed for test drivers */
bool cEnvir::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    return false;
}


/** Not needed for test drivers */
void cEnvir::moduleDeleted(cModule*)
{
}

/** Not needed for test drivers */
bool cEnvir::idle()
{
    return false;
}

/** Not needed for test drivers */
void cEnvir::deregisterOutputVector(void*)
{
}

/** Not needed for test drivers */
string cEnvir::gets(char const*, char const*)
{
    return "";
}

/** Not needed for test drivers */
void cEnvir::releaseStreamForSnapshot(basic_ostream<char>*)
{
}

/** Not needed for test drivers */
void cEnvir::printfmsg(char const*, ...)
{
}

/** Not needed for test drivers */
void cEnvir::moduleReparented(cModule*, cModule*)
{
}

/** Not needed for test drivers */
cXMLElement* cEnvir::getXMLDocument(char const*, char const*)
{
    return 0;
}

/** Add a module to the testing environment */
void cEnvir::moduleCreated(cModule* module)
{
}

/** Not needed for test drivers */
ostream* cEnvir::getStreamForSnapshot()
{
    return 0;
}

/** Not needed for test drivers */
bool cEnvir::isModuleLocal(cModule*, char const*, int)
{
    return true;
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
