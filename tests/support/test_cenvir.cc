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
#include "csimul.h"
#include "onstartup.h"
using namespace std;

/** Global objects */
cEnvir ev;
cStaticFlag* staticFlag;
static cDefaultList g_defaultList("globalTestingDefaultList");

/** Some weird dummy function that is apparently required */
void envirDummy()
{
}

/** Default constructor */
cEnvir::cEnvir() :
    ostream(&ev_buf),
    ev_buf(this)
{
    // Turn off garbage collection (see cdefaultlist.cc)
    cDefaultList::doGC = false;

    // Set the static flag (see cobject.h)
    //
    // Necessary to create cDefaultList type required to construct modules
    // I am not using it the way the name implies it should work because
    // apparently it is named poorly
    //
    staticFlag = new cStaticFlag();

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
void cEnvir::displayStringChanged(cModule *submodule)
{
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
bool cEnvir::recordInOutputVector(void *vechandle, simtime_t t,
                                  double value1, double value2)
{
    return false;
}

/** Not needed for test drivers */
void cEnvir::undisposedObject(cObject *obj)
{
}

/** Not needed for test drivers */
void cEnvir::moduleMethodCalled(cModule*, cModule*, char const*)
{
}

/** Not needed for test drivers */
void cEnvir::bubble(cModule*, char const*)
{
}

/** Not needed for test drivers */
void cEnvir::messageSent(cMessage*, cGate*)
{
}

/** Not needed for test drivers */
int cEnvir::argCount()
{
    return 0;
}

/** Not needed for test drivers */
char** cEnvir::argVector()
{
    return 0;
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
void cEnvir::connectionRemoved(cGate*)
{
}

/** Not needed for test drivers */
void cEnvir::deregisterOutputVector(void*)
{
}

/** Not needed for test drivers */
void cEnvir::objectDeleted(cObject*)
{
}

/** Not needed for test drivers */
void cEnvir::messageDelivered(cMessage*)
{
}

/** Not needed for test drivers */
unsigned cEnvir::extraStackForEnvir()
{
    return 0u;
}

/** Not needed for test drivers */
string cEnvir::gets(char const*, char const*)
{
    return "";
}

/** Not needed for test drivers */
cConfiguration* cEnvir::config()
{
    return 0;
}

/** Not needed for test drivers */
void cEnvir::displayStringChanged(cGate*)
{
}

/** Not needed for test drivers */
void* cEnvir::registerOutputVector(char const*, char const*, int)
{
    return 0;
}

/** Not needed for test drivers */
void cEnvir::printf(char const*, ...)
{
}

/** Not needed for test drivers */
void cEnvir::releaseStreamForSnapshot(basic_ostream<char>*)
{
}

/** Not needed for test drivers */
cRNG* cEnvir::rng(int)
{
    cerr << "cEnvir::rng unavailable during testing" << endl; 
    return 0;
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

/** Not needed for test drivers */
void cEnvir::getRNGMappingFor(cModule*)
{
}

/** Not needed for test drivers */
bool cEnvir::getParameterUseDefault(int, char const*)
{
    return true;
}

/** Not needed for test drivers */
string cEnvir::getParameter(int, char const*)
{
    return "";
}

/** Add a module to the testing environment */
void cEnvir::moduleCreated(cModule* module)
{
}

/** Not needed for test drivers */
void cEnvir::recordScalar(cModule*, char const*, double)
{
}

/** Not needed for test drivers */
void cEnvir::backgroundDisplayStringChanged(cModule*)
{
}

/** Not needed for test drivers */
ostream* cEnvir::getStreamForSnapshot()
{
    return 0;
}

/** Not needed for test drivers */
void cEnvir::breakpointHit(char const*, cSimpleModule*)
{
}

/** Not needed for test drivers */
bool cEnvir::isModuleLocal(cModule*, char const*, int)
{
    return true;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
