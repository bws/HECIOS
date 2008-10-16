#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "phtf_io_trace.h"
#include "parser_sm.h"
using namespace std;

PaserSm::PaserSm(string fn, string dpath, string rank)
{
    current_st_ = IDLE;
    current_act_ = ENTER;
    nextStr_ = "";
    fileName_ = fn;
    flag = 0;
    time = 0;
    duration = 0;
    acc = 0;

    stringstream ss1("");
    stringstream ss2("");
    stringstream ss3("");

    ss1 << dpath << "event." << rank;
    ss2 << dpath << "runtime." << rank;
    ss3 << dpath << "fs.ini";

    event = new PHTFEvent(ss1.str(), ss2.str());
    fs = new PHTFFs(ss3.str(), true);
    event->open(true);
    PHTFEventRecord::buildOpMap();
}

void PaserSm::startProc()
{
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
    fs->consts("MPI_COMM_WORLD", "91");
    fs->consts("MPI_COMM_SELF", "92");
    fs->consts("MPI_SEEK_SET", "600");
    fs->consts("MPI_SEEK_CUR", "602");
    fs->consts("MPI_SEEK_END", "604");

    ifs_.open(fileName_.c_str());
    if(!ifs_.is_open())
    {
        cerr << "Could not open trace file " << fileName_ << endl;
        exit(0);
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

    if(regcomp(&regProcL2_,
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

void PaserSm::getNext()
{
    getline(ifs_, nextStr_);
    for(size_t i = 0; i < nextStr_.length(); i ++)
    {
        nextStr_[i] = toupper(nextStr_[i]);
    }
}

void PaserSm::procNext()
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

void PaserSm::enterIdle()
{
    if(!regexec(&regIdle_, nextStr_.c_str(), MAXPARA, regMatches, 0))
    {
        flag = 1; // a valid operation, goto ProcF
    }
    else
    {
        flag = 0;
    }
}

void PaserSm::exitIdle()
{
    if(flag == 1)
    {
        current_st_ = PROCF;
    }
}

void PaserSm::enterProcT()
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
            current_datatype_ = CONTIGUOUS;
        }
        else if(!type.compare("TYPE_STRUCT"))
        {
            current_datatype_ = STRUCT;
        }
        else if(!type.compare("TYPE_CREATE_SUBARRAY"))
        {
            current_datatype_ = SUBARRAY;
        }
        else if(!type.compare("TYPE_VECTOR"))
        {
            current_datatype_ = VECTOR;
        }
        else if(!type.compare("CONST"))
        {

        }
        else if(!type.compare("COMM_RANK") || !type.compare("COMM_SPLIT") || !type.compare("COMM_DUP"))
        {

        }
        else
        {
            cerr << "Error: unsupported datatype! " << type << endl;
            exit(0);
        }

        flag = 1;
    }
    else
    {
        flag = -1;
    }
}

void PaserSm::exitProcT()
{
    if(flag == -1)
    {
        current_st_ = HALF;
    }
    else
    {
        current_st_ = TYPE;
    }
}

void PaserSm::enterType()
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

            flag = -1;
        }
        else
        {
            flag = 1;
        }
    }
    else
    {
        flag = 0;
    }
}

void PaserSm::exitType()
{
    if(flag == 1)
    {
        current_st_ = PROCP;
    }
    else
    {
        current_st_ = HALF;
    }
}

string PaserSm::findAlias(string id)
{
    string alias = event->memValue("Alias", id);
    if(alias.compare(""))
        return findAlias(alias);
    else return id;
}

void PaserSm::enterProcP()
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
        {
            for(size_t i = 0; i < item.length(); i ++)
            {
                item[i] = toupper(item[i]);
            }
        }
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
        ss << data << "@" << acc;
        newdatatype = ss.str();
    }
    else if(!item.compare("pnewtype"))
    {
        string pnewtype = data;
        stringstream addr("");
        addr << pnewtype << "@" << acc;
        string newtype = newdatatype.substr(0, newdatatype.find("@"));
        event->memValue("Pointer", addr.str(), newtype);
        stringstream ss("");
        ss << current_datatype_;
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

void PaserSm::exitProcP()
{
    current_st_ = TYPE;
}

void PaserSm::enterProcF()
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
        stringstream ss("");
        ss << acc << " CPU_PHASE " << time+duration << " " << sec1 - time - duration;
        PHTFEventRecord re(ss.str());
        *event << re;
        acc ++;
    }

    time = sec1;

    index = regMatches[3].rm_so;
    len = regMatches[3].rm_eo - index;
    string param1 = nextStr_.substr(index, len);

    params = param1;

    for(size_t i = 0; i < param1.length(); i ++)
    {
        if (param1[i] == ',')
        {
            params = param1.erase(i--, 1);
        }
    }

    for(size_t i = 0; i < params.length(); i ++)
    {
        params[i] = tolower(params[i]);
    }

    index = regMatches[4].rm_so;
    if(nextStr_[index] == ')') // operation finished
        flag = -1;
    else
        flag = 1;

    if(flag == -1)
    {
        index = regMatches[5].rm_so;
        len = regMatches[5].rm_eo - index;
        retdur_ = nextStr_.substr(index, len);
    }

}

void PaserSm::exitProcF()
{
    if(flag == -1)
    {
        current_st_ = PROCL;
    }
    else
    {
        current_st_ = HALF;
    }
}

void PaserSm::enterHalf()
{
    if(!regexec(&regProcF_, nextStr_.c_str(), MAXPARA, regMatches, 0))
    {
        flag = -1; // a valid second part, goto ProcL
    }
    else
    {
        flag = 0; // stay here
        if(!op.compare("MPI_FILE_OPEN"))
        {
            if(!regexec(&regProcR_, nextStr_.c_str(), MAXPARA, regMatches, 0))
            {
                flag = 1; // filename string, goto ProcR
            }
        }
        else if(op.find("MPI_TYPE_") == 0  && op.find("MPI_TYPE_COMMIT") != 0 && !regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
        {
            flag = 2; // datatype def block
        }
        else if(op.find("MPI_INIT") == 0 && !regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
        {
            flag = 2; // mpi const def block
        }
        else if(op.find("MPI_COMM") == 0 && !regexec(&regIdle2_, nextStr_.c_str(), MAXPARA, regMatches, 0))
        {
            flag = 2; // mpi comm def block
        }
    }

    if(flag == -1)
    {
        int index = regMatches[1].rm_so;
        int len = regMatches[1].rm_eo - index;
        retdur_ = nextStr_.substr(index, len);
    }
}

void PaserSm::exitHalf()
{
    if(flag == -1)
    {
        current_st_ = PROCL;
    }
    else if(flag == 1)
    {
        current_st_ = PROCR;
    }
    else if(flag == 2)
    {
        current_st_ = PROCT;
    }
}

void PaserSm::enterProcL()
{
    if(!regexec(&regProcL_, retdur_.c_str(), MAXPARA, regMatches, 0))
    {
        int index = regMatches[1].rm_so;
        int len = regMatches[1].rm_eo - index;
        ret = retdur_.substr(index, len);

        for (size_t i = 0; i < ret.length(); i++)
        {
            ret[i] = tolower(ret[i]);
        }

        index = regMatches[2].rm_so;
        len = regMatches[2].rm_eo - index;
        string dur = retdur_.substr(index, len);

        sscanf(dur.c_str(), "%lf", &duration);
    }

    stringstream ss("");

    ss << acc << " " << op << " " << time << " " << duration << " " << ret << " " << params;

    PHTFEventRecord re(ss.str());

    if (re.recordOp() == INVALID && op.find("MPI_INIT") != 0)
    {
        cerr << "Error: unsupported operations: " << op << endl;
        exit(0);
    }
    else if(op.find("MPI_INIT") != 0)
    {
        acc ++;
        *event << re;
    }

    if (re.recordOp() == SEEK ||
        re.recordOp() == WRITE_AT ||
        re.recordOp() == READ_AT ||
        re.recordOp() == WRITE_AT_ALL ||
        re.recordOp() == READ_AT_ALL)
    {
        string val = re.paramAt(0);
        event->memValue("Pointer", context_, val);
    }
}

void PaserSm::exitProcL()
{
    current_st_ = IDLE;
}

void PaserSm::enterProcR()
{
    int index = regMatches[1].rm_so;
    int len = regMatches[1].rm_eo - index;

    runtime = nextStr_.substr(index, len);

    PHTFEventRecord re;
    re.params(params);

    stringstream ss("");
    if(runtime[0] != '/')
    {
        ss << "/tmp/";
    }
    ss << runtime;

    context_ = createContext(re.paramAt(4), acc);
    string strpt = re.paramAt(1);
    string straddr = createContext(re.paramAt(1), acc);

    if(!event->memValue("String", strpt).compare(""))
    {
        event->memValue("String", strpt, ss.str());
    }
    else if(!event->memValue("String", strpt).compare(ss.str()))
    {
        event->memValue("String", straddr, ss.str());
    }
    else
    {
        event->memValue("String", strpt, ss.str());
    }
    //event->memValue("Pointer", context_, "");
    fs->addFile(ss.str(), "1");
}

void PaserSm::exitProcR()
{
    current_st_ = HALF;
}

string PaserSm::createContext(string addr, string context) const
{
    stringstream ss("");
    ss << addr << "@" << context;
    return ss.str();
}

string PaserSm::createContext(string addr, long context) const
{
    stringstream ss("");
    ss << addr << "@" << context;
    return ss.str();
}
