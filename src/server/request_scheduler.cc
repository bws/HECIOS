#include "request_scheduler.h"
#include <cassert>
#include <iostream>
#include "pvfs_proto_m.h"
using namespace std;

// OMNet Registration Method
Define_Module(RequestScheduler);

/**
 * Initialization
 */
void RequestScheduler::initialize()
{
}

/**
 *
 */
void RequestScheduler::handleMessage(cMessage* msg)
{
    // For now, construct the appropriate response and simply send it back
    switch(msg->kind())
    {
        case SPFS_CREATE_REQUEST:
        {
            // Build a queue for this handle
            spfsCreateRequest create* = static_cast<spfsCreateRequest>(msg);
            FSHandle createHandle = create->gethandle();
            list<PendingOperation> opList;

            // PendingOperation op(true, false, create);
            // opList.push_front(op);
            // operationsByHandle_[createHandle] = opList;
            // send(msg, serverOut);
            
            /**
               opList = operationsByHandle_[createHandle];
               if(!opList.empty())
               {
                  PendingOperation op1(true, false, create);     
                  opList.push_front(op1);
                  send(msg, serverOut);
               }
               else
               {
                  PendingOperation op2(false, false, create);
                  opList.push_front(op2);
                  send(msg, serverOut);
               }
            */
           
            break;
        }
        case SPFS_LOOKUP_PATH_REQUEST:
        case SPFS_GET_ATTR_REQUEST:
        case SPFS_READ_REQUEST:
        case SPFS_WRITE_REQUEST:
        {
            break;
        }
        case SPFS_READ_RESPONSE:
        {

            // complete the operation
            // Pop operations out of queue
            // Look at the handle
            // And see if another pending op exists that can now proceed
            // operationsByHandle_.find(handle);

            
            /**
               spfsReadRequest read* = static_cast<spfsReadRequest>(msg);
               FSHandle handle = read->gethandle();
               list<PendingOperation> chkList;
               chkList = operationsByHandle_.find(handle);
               list<PendingOperation>::iterator listItr;
               if(!chkList.empty())
               {
                 while(listItr != chkList.end()){
                   if( *listItr->op == read)
                   {
                      *listItr->isPending = false;
                      *listItr->isComplete = true;
                      // chkList.erase(*listItr);
                      // send(msg, serverIn );
                   }
               }
            */
        }
        default:
        {
            cerr << "Error: Unknown server message type" << endl;
        }
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
