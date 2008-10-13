#ifndef PARSER_SM_H
#define PARSER_SM_H

#include <string>
#include <fstream>
#include <regex.h>
#include <stdio.h>

#include "phtf_io_trace.h"

#define MAXINLINE 300
#define MAXPARA 20

using namespace std;

#define SMMAP_BEGIN() do                        \
    {                                           \
        switch(current_st_)

#define SMMAP_ITEM(value, func1, func2)         \
    case value:                                 \
    if(current_act_)                            \
    {                                           \
        func1();                                \
        current_act_ = ENTER;                   \
    }                                           \
    else                                        \
    {                                           \
        func2();                                \
        current_act_ = EXIT;                    \
    }                                           \
    break

#define SMMAP_END()                             \
    }while(current_st_ < IDLE                   \
           || current_act_ != ENTER)

class PaserSm
{
private:
    enum state
    {
        IDLE = 0,   // initial state
        PROCF = -1, // transient state
        HALF = 2,   // steady state
        PROCL = -3, // transient state
        PROCR = -4,  // transient state
        PROCT = -5, // transient state
        TYPE = 6,
        PROCP = -7
    } current_st_;

    enum action
    {
        ENTER = 0,
        EXIT
    } current_act_;

    PHTFDataType current_datatype_;

    int flag;

    string fileName_;
    string nextStr_;
    string retdur_;
    ifstream ifs_;
    string context_;

    string datatypeparams;
    string newdatatype;
    string olddatatypesize;

    double time;
    double duration;
    string op;
    string params;
    string ret;
    string runtime;
    long acc;

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

public:
    PaserSm(string fn, string dpath, string rank);
    ~PaserSm(){delete event;delete fs;};

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

    string createContext(string addr, string context) const;
    string createContext(string addr, long context) const;

    string findAlias(string id);
};

#endif
