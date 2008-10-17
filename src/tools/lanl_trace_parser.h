#ifndef LANL_TRACE_PARSER_H
#define LANL_TRACE_PARSER_H

#include <cstdio>
#include <string>
#include <fstream>
#include <regex.h>
#include "phtf_io_trace.h"


#define MAXPARA 20


class LanlTraceParser
{
public:
    LanlTraceParser(std::string fn, std::string dpath, std::string rank);

    ~LanlTraceParser() { delete event; delete fs; };

    void startProc();

protected:
    void getNext();
    void procNext();
    void enterIdle();
    void exitIdle();
    void enterProcT();
    void exitProcT();
    void enterType();
    void exitType();
    void enterProcP();
    void exitProcP();
    void enterProcF();
    void exitProcF();
    void enterHalf();
    void exitHalf();
    void enterProcL();
    void exitProcL();
    void enterProcR();
    void exitProcR();

    std::string createContext(std::string addr, std::string context) const;

    std::string createContext(std::string addr, long context) const;

    std::string findAlias(std::string id);

private:
    /** Modify the filename in place so that it conforms to our standards */
    static void modifyFilename(std::string& filename);

    enum State
    {
        IDLE = 0,   // initial state
        PROCF = -1, // transient state
        HALF = 2,   // steady state
        PROCL = -3, // transient state
        PROCR = -4,  // transient state
        PROCT = -5, // transient state
        TYPE = 6,
        PROCP = -7
    };

    enum Action
    {
        ENTER = 0,
        EXIT
    };

    State smCurrentState_;
    Action smCurrentAction_;
    PHTFDataType smCurrentDatatype_;
    int smTriggerFlag_;

    std::string fileName_;
    std::string nextStr_;
    std::string retdur_;
    std::ifstream ifs_;
    std::string context_;

    std::string datatypeparams;
    std::string newdatatype;
    std::string olddatatypesize;

    double time;
    double duration;
    std::string op;
    std::string params;
    std::string ret;
    std::string runtime;
    std::size_t currentEventCount_;

    PHTFEvent *event;
    PHTFFs *fs;

    regex_t regIdle_;
    regex_t regIdle2_;
    regex_t regProcT_;
    regex_t regProcF_;
    regex_t regProcL_;
    regex_t regProcL2_;
    regex_t regProcR_;

    regmatch_t regMatches[MAXPARA];

};

#endif
