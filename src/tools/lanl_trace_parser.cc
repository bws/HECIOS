#include "lanl_trace_parser.h"
#include <stdint.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#define SMMAP_BEGIN() do                        \
    {                                           \
        switch(smCurrentState_)

#define SMMAP_ITEM(value, func1, func2)         \
    case value:                                 \
    if(smCurrentAction_)                        \
    {                                           \
        func1();                                \
        smCurrentAction_ = ENTER;               \
    }                                           \
    else                                        \
    {                                           \
        func2();                                \
        smCurrentAction_ = EXIT;                \
    }                                           \
    break

#define SMMAP_END()                             \
    } while (smCurrentState_ < IDLE ||          \
             smCurrentAction_ != ENTER)


LanlTraceParser::LanlTraceParser(string fn, string dpath, string rank)
    : smCurrentState_(IDLE),
      smCurrentAction_(ENTER),
      smTriggerFlag_(0),
      fileName_(fn),
      nextStr_(""),
      time(0),
      duration(0),
      currentEventCount_(0)
{
    // Construct the file system configuration
    ostringstream fsIniFilename;
    fsIniFilename << dpath << "/" << "fs.ini";
    fs = new PHTFFs(fsIniFilename.str(), true);

    // Construct the trace event collector
    ostringstream eventFilename;
    eventFilename << dpath << "/" << "event." << rank;
    ostringstream runtimeFilename;
    runtimeFilename << dpath << "/" << "runtime." << rank;
    event = new PHTFEvent(eventFilename.str(), runtimeFilename.str());

    // Initialize the trace event collector
    event->open(true);
    PHTFEventRecord::buildOpMap();
}

void LanlTraceParser::startProc()
{
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
    fs->consts("MPI_COMM_WORLD", "91");
    fs->consts("MPI_COMM_SELF", "92");
    fs->consts("MPI_SEEK_SET", "600");
    fs->consts("MPI_SEEK_CUR", "602");
    fs->consts("MPI_SEEK_END", "604");

    ifs_.open(fileName_.c_str());
    if(!ifs_.is_open())
    {
        cerr << "Could not open trace file " << fileName_ << endl;
        exit(47);
    }

    // an whole operation, or the first part of a splitted operation
    if(regcomp(&regIdle_,
               "([^ ]+) +(MPI_FILE[^(]+|MPI_BARRIER|MPIO_WAIT|MPI_INIT|MPI_COMM_CREATE|MPI_COMM_DUP|MPI_COMM_SPLIT|MPI_COMM_RANK|MPI_TYPE_CONTIGUOUS|MPI_TYPE_VECTOR|MPI_TYPE_CREATE_SUBARRAY|MPI_TYPE_STRUCT)_*\\(([^[)]+)([)<])(.*)",
               REG_EXTENDED)
       )return;

    // an type definition block
    if(regcomp(&regIdle2_,
               "[^ ]+ +SYS_WRITE\\(1, \" *([^\"\\\\]*)",
               REG_EXTENDED)
       )return;

    if(regcomp(&regProcT_,
               "MPI_(.*) (STARTS|ENDS)",
               REG_EXTENDED)
      )return;

    // the second part of a splitted operation
    if(regcomp(&regProcF_,
               "[^ ]+ +<[^)]+\\) +(.*)",
               REG_EXTENDED)
       )return;

    // the thrid part of a splitted operation
    if(regcomp(&regProcL_,
               " *= *([^ ]*) *<([^>]*)>",
               REG_EXTENDED)
       )return;

    if (regcomp(&regProcL2_,
               "MPI_TYPE[^(]+",
               REG_EXTENDED)
           )return;

    if(regcomp(&regProcR_,
               "[^ ]+ +SYS_OPEN\\(\"([^\"]*)",
               REG_EXTENDED)
       )
        return;

    while(!ifs_.eof())
    {
        procNext();
    }

    regfree(&regIdle_);
    regfree(&regIdle2_);
    regfree(&regProcT_);
    regfree(&regProcF_);
    regfree(&regProcL_);
    regfree(&regProcL2_);
    regfree(&regProcR_);

    ifs_.close();

}

void LanlTraceParser::getNext()
{
    getline(ifs_, nextStr_);
    for(size_t i = 0; i < nextStr_.length(); i ++)
    {
        nextStr_[i] = toupper(nextStr_[i]);
    }
}

void LanlTraceParser::procNext()
{
    getNext();
    SMMAP_BEGIN()
    {
        SMMAP_ITEM(IDLE, exitIdle, enterIdle);
        SMMAP_ITEM(PROCT, exitProcT, enterProcT);
        SMMAP_ITEM(TYPE, exitType, enterType);
        SMMAP_ITEM(PROCP, exitProcP, enterProcP);
        SMMAP_ITEM(PROCF, exitProcF, enterProcF);
        SMMAP_ITEM(HALF, exitHalf, enterHalf);
        SMMAP_ITEM(PROCL, exitProcL, enterProcL);
        SMMAP_ITEM(PROCR, exitProcR, enterProcR);
    }SMMAP_END();
}

void LanlTraceParser::enterIdle()
{
    if(!regexec(&regIdle_, nextStr_.c_str(), MAXPARA, regMatches, 0))
    {
        smTriggerFlag_ = 1; // a valid operation, goto ProcF
    }
    else
    {
        smTriggerFlag_ = 0;
    }
}

void LanlTraceParser::exitIdle()
{
    if(smTriggerFlag_ == 1)
    {
        smCurrentState_ = PROCF;
    }
}

void LanlTraceParser::enterProcT()
{
    int index = regMatches[1].rm_so;
    int len = regMatches[1].rm_eo - index;
    datatypeparams = nextStr_.substr(index, len);

    if(!regexec(&regProcT_, datatypeparams.c_str(), MAXPARA, regMatches, 0))
    {
        index = regMatches[1].rm_so;
        len = regMatches[1].rm_eo - index;
        string type = datatypeparams.substr(index, len);
        index = regMatches[2].rm_so;
        len = regMatches[2].rm_eo - index;
        string state = datatypeparams.substr(index, len);

        if(!type.compare("TYPE_CONTIGUOUS"))
        {
            smCurrentDatatype_ = CONTIGUOUS;
        }
        else if(!type.compare("TYPE_STRUCT"))
        {
            smCurrentDatatype_ = STRUCT;
        }
        else if(!type.compare("TYPE_CREATE_SUBARRAY"))
        {
            smCurrentDatatype_ = SUBARRAY;
        }
        else if(!type.compare("TYPE_VECTOR"))
        {
            smCurrentDatatype_ = VECTOR;
        }
        else if(!type.compare("CONST"))
        {

        }
        else if(!type.compare("COMM_RANK") ||
                !type.compare("COMM_SPLIT") ||
                !type.compare("COMM_DUP") ||
                !type.compare("COMM_CREATE"))
        {
            // Do nothing in order to prevent bogus error
        }
        else
        {
            cerr << "Error: unsupported datatype! " << type << endl;
            exit(29);
        }

        smTriggerFlag_ = 1;
    }
    else
    {
        smTriggerFlag_ = -1;
    }
}

void LanlTraceParser::exitProcT()
{
    if(smTriggerFlag_ == -1)
    {
        smCurrentState_ = HALF;
    }
    else
    {
        smCurrentState_ = TYPE;
    }
}

void LanlTraceParser::enterType()
{
    if(!regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
    {
        int index = regMatches[1].rm_so;
        int len = regMatches[1].rm_eo - index;
        datatypeparams = nextStr_.substr(index, len);

        if(!regexec(&regProcT_, datatypeparams.c_str(), MAXPARA, regMatches, 0))
        {
            index = regMatches[1].rm_so;
            len = regMatches[1].rm_eo - index;
            string type = datatypeparams.substr(index, len);
            index = regMatches[2].rm_so;
            len = regMatches[2].rm_eo - index;
            string state = datatypeparams.substr(index, len);

            smTriggerFlag_ = -1;
        }
        else
        {
            smTriggerFlag_ = 1;
        }
    }
    else
    {
        smTriggerFlag_ = 0;
    }
}

void LanlTraceParser::exitType()
{
    if(smTriggerFlag_ == 1)
    {
        smCurrentState_ = PROCP;
    }
    else
    {
        smCurrentState_ = HALF;
    }
}

string LanlTraceParser::findAlias(string id)
{
    string alias = event->memValue("Alias", id);
    if(alias.compare(""))
        return findAlias(alias);
    else return id;
}

void LanlTraceParser::enterProcP()
{
    for(size_t i = 0; i < datatypeparams.length(); i ++)
    {
        datatypeparams[i] = tolower(datatypeparams[i]);
    }

    int pos = datatypeparams.find('=');
    string item = datatypeparams.substr(0, pos);
    string data = datatypeparams.substr(pos + 1);

    if(op.find("MPI_INIT") == 0)
    {
        if(item.find("mpi") == 0)
            for(size_t i = 0; i < item.length(); i ++)
                item[i] = toupper(item[i]);
        fs->consts(item, data);
        return;
    }

    if(op.find("MPI_COMM") == 0)
    {
        if(op.find("MPI_COMM_SPLIT") == 0)
        {
            int pos1 = params.rfind(' ');
            pos1 = params.rfind(' ', pos1 - 1);
            string commid = params.substr(pos1 + 1, params.length() - pos1 - 2);

            if(!item.compare("comm"))
            {
                newdatatype = data;
                string aa = findAlias(newdatatype);
                if(aa.compare(commid))
                    event->memValue("Alias", commid, aa);
            }
            else if(!item.compare("ranks"))
            {
                string olddata = event->memValue(newdatatype, item);
                stringstream ss("");
                if(olddata.compare(""))
                {
                    ss << olddata << " ";
                }
                ss << data;

                event->memValue(newdatatype, item, ss.str());
            }
            else
            {
                event->memValue(newdatatype, item, data);
            }

        }
        else if(op.find("MPI_COMM_RANK") == 0)
        {
            int pos1 = params.find(' ');
            string alias = params.substr(0, pos1);
            if(!item.compare("comm"))
            {
                string aa = findAlias(data);
                if(aa.compare(alias))
                    event->memValue("Alias", alias, aa);
            }
        }
        else if(op.find("MPI_COMM_DUP") == 0)
        {
            if(!item.compare("comm"))
            {
                newdatatype = data;
            }
            else if(!item.compare("newcomm") || !item.compare("pnewcomm"))
            {
                string aa = findAlias(newdatatype);
                if(aa.compare(data))
                    event->memValue("Alias", data, newdatatype);
            }
        }
        return;
    }

    if(!item.compare("newtype"))
    {
        stringstream ss("");
        ss << data << "@" << currentEventCount_;
        newdatatype = ss.str();
    }
    else if(!item.compare("pnewtype"))
    {
        string pnewtype = data;
        stringstream addr("");
        addr << pnewtype << "@" << currentEventCount_;
        string newtype = newdatatype.substr(0, newdatatype.find("@"));
        event->memValue("Pointer", addr.str(), newtype);
        stringstream ss("");
        ss << smCurrentDatatype_;
        event->memValue(newdatatype, "type", ss.str());
    }
    else if(!item.compare("oldtypesize") || !item.compare("oldtypessize"))
    {
        olddatatypesize = data;
    }
    else
    {
        if(!item.compare("oldtype") || !item.compare("oldtypes"))
        {
            if(!event->memValue(data, "type").compare(""))
            {
                event->memValue(data, "type", "0");
                event->memValue(data, "size", olddatatypesize);
            }
        }

        string olddata = event->memValue(newdatatype, item);
        stringstream ss("");
        if(olddata.compare(""))
        {
            ss << olddata << " ";
        }
        ss << data;

        event->memValue(newdatatype, item, ss.str());
    }

}

void LanlTraceParser::exitProcP()
{
    smCurrentState_ = TYPE;
}

void LanlTraceParser::enterProcF()
{
    // proc first part
    int index = regMatches[1].rm_so;
    int len = regMatches[1].rm_eo - index;
    string timing = nextStr_.substr(index, len);

    static double starttime = -1;

    double hr1, min1, sec1;

    sscanf(timing.c_str(), "%lf:%lf:%lf", &hr1, &min1, &sec1);

    sec1 = hr1 * 3600 + min1 * 60 + sec1;
    if(starttime == -1) starttime = sec1;

    sec1 -= starttime;

    if(time == 0)time = sec1;

    index = regMatches[2].rm_so;
    len = regMatches[2].rm_eo - index;
    op = nextStr_.substr(index, len);
    while(op[op.length() - 1] == '_')op.erase(op.length() -1, 1);

    if(sec1 > time + duration)// && regexec(&regProcL2_, op.c_str(), 0, NULL, 0))
    {
        ostringstream ss;
        ss << currentEventCount_ << " CPU_PHASE " << time+duration << " " << sec1 - time - duration;
        string recordString = ss.str();
        PHTFEventRecord re(recordString);
        *event << re;
        currentEventCount_ ++;
    }

    time = sec1;

    index = regMatches[3].rm_so;
    len = regMatches[3].rm_eo - index;
    string param1 = nextStr_.substr(index, len);

    params = param1;

    for(size_t i = 0; i < param1.length(); i ++)
    {
        if(param1[i] == ',')params = param1.erase(i--, 1);
    }

    for(size_t i = 0; i < params.length(); i ++)
    {
        params[i] = tolower(params[i]);
    }

    index = regMatches[4].rm_so;
    if(nextStr_[index] == ')') // operation finished
        smTriggerFlag_ = -1;
    else
        smTriggerFlag_ = 1;

    if(smTriggerFlag_ == -1)
    {
        index = regMatches[5].rm_so;
        len = regMatches[5].rm_eo - index;
        retdur_ = nextStr_.substr(index, len);
    }

}

void LanlTraceParser::exitProcF()
{
    if(smTriggerFlag_ == -1)
    {
        smCurrentState_ = PROCL;
    }
    else
    {
        smCurrentState_ = HALF;
    }
}

void LanlTraceParser::enterHalf()
{
    if(!regexec(&regProcF_, nextStr_.c_str(), MAXPARA, regMatches, 0))
    {
        smTriggerFlag_ = -1; // a valid second part, goto ProcL
    }
    else
    {
        smTriggerFlag_ = 0; // stay here
        if(!op.compare("MPI_FILE_OPEN"))
        {
            if(!regexec(&regProcR_, nextStr_.c_str(), MAXPARA, regMatches, 0))
            {
                smTriggerFlag_ = 1; // filename string, goto ProcR
            }
        }
        else if(op.find("MPI_TYPE_") == 0  && op.find("MPI_TYPE_COMMIT") != 0 && !regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
        {
            smTriggerFlag_ = 2; // datatype def block
        }
        else if(op.find("MPI_INIT") == 0 && !regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
        {
            smTriggerFlag_ = 2; // mpi const def block
        }
        else if(op.find("MPI_COMM") == 0 && !regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
        {
            smTriggerFlag_ = 2; // mpi comm def block
        }
    }

    if(smTriggerFlag_ == -1)
    {
        int index = regMatches[1].rm_so;
        int len = regMatches[1].rm_eo - index;
        retdur_ = nextStr_.substr(index, len);
    }
}

void LanlTraceParser::exitHalf()
{
    if(smTriggerFlag_ == -1)
    {
        smCurrentState_ = PROCL;
    }
    else if(smTriggerFlag_ == 1)
    {
        smCurrentState_ = PROCR;
    }
    else if(smTriggerFlag_ == 2)
    {
        smCurrentState_ = PROCT;
    }
}

void LanlTraceParser::enterProcL()
{
    if(!regexec(&regProcL_, retdur_.c_str(), MAXPARA, regMatches, 0))
    {
        int index = regMatches[1].rm_so;
        int len = regMatches[1].rm_eo - index;
        ret = retdur_.substr(index, len);

        for(size_t i = 0; i < ret.length(); i ++)
        {
            ret[i] = tolower(ret[i]);
        }

        index = regMatches[2].rm_so;
        len = regMatches[2].rm_eo - index;
        string dur = retdur_.substr(index, len);

        sscanf(dur.c_str(), "%lf", &duration);
    }

    stringstream ss("");

    ss << currentEventCount_ << " " << op << " " << time << " " << duration << " " << ret << " " << params;

    PHTFEventRecord re(ss.str());

    if(re.recordOp() == INVALID && op.find("MPI_INIT") != 0)
    {
        cerr << "Error: unsupported operations: " << op << endl;
        exit(25);
    }
    else if (OPEN == re.recordOp())
    {
        // Modify the filename in the parameters
        string filename = re.paramAt(1);
        modifyFilename(filename);

        // Create a new record event with the correct data
        ostringstream recordData;
        recordData << currentEventCount_ << " "
                   << op << " "
                   << time << " "
                   << duration << " "
                   << ret << " "
                   << re.paramAt(0) << " "
                   << filename << " "
                   << re.paramAt(2) << " "
                   << re.paramAt(3) << " "
                   << re.paramAt(4);

        // Output the record
        PHTFEventRecord openRecord(recordData.str());
        (*event) << openRecord;
        currentEventCount_++;
    }
    else if(op.find("MPI_INIT") != 0)
    {
        currentEventCount_++;
        *event << re;
    }

    if (re.recordOp() == SEEK ||
        re.recordOp() == WRITE_AT ||
        re.recordOp() == READ_AT ||
        re.recordOp() == WRITE_AT_ALL ||
        re.recordOp() == READ_AT_ALL)
    {
        event->memValue("Pointer", currentEventPtrAddr_, re.paramAt(0));
    }
}

void LanlTraceParser::exitProcL()
{
    smCurrentState_ = IDLE;
}

void LanlTraceParser::enterProcR()
{
    int index = regMatches[1].rm_so;
    int len = regMatches[1].rm_eo - index;
    runtime = nextStr_.substr(index, len);

    PHTFEventRecord re;
    re.params(params);

    // Regularize the filename to our standards, and update the event
    string filename = re.paramAt(1);
    modifyFilename(filename);
    re.paramAt(1, filename);

    // Determine a size for the file (16 MB * 128 clients to start with)
    uint64_t fileSize = uint64_t(16777216) * uint64_t(128);
    ostringstream oss;
    oss << fileSize;

    // Add the file
    fs->addFile(filename, oss.str());

    // Set the address of the pointer to the handle
    currentEventPtrAddr_ = createContext(re.paramAt(4), currentEventCount_);
}

void LanlTraceParser::exitProcR()
{
    smCurrentState_ = HALF;
}

string LanlTraceParser::createContext(string addr, string context) const
{
    stringstream ss("");
    ss << addr << "@" << context;
    return ss.str();
}

string LanlTraceParser::createContext(string addr, long context) const
{
    stringstream ss("");
    ss << addr << "@" << context;
    return ss.str();
}

void LanlTraceParser::modifyFilename(string& filename)
{
    assert(2 < filename.size());

    // Remove the closing quote if it exists
    if ('"' == filename[filename.size() - 1])
    {
        filename.erase(filename.size() - 1);
    }

    // Remove the opening quote if it exists
    if ('"' == filename[0])
    {
        filename.erase(0, 1);
    }

    // If the first character isn't a leading slash, modify the path
    if ('/' != filename[0])
    {
        filename.insert(0, "/autopath/");
    }
}
