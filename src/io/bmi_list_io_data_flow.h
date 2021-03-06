#ifndef BMI_LIST_IO_DATA_FLOW_H
#define BMI_LIST_IO_DATA_FLOW_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "data_flow.h"
#include "filename.h"
class spfsDataFlowStart;
class cSimpleModule;
class cMessage;
class FSMetaData;

/** A data flow using ListIO system calls to read and write data */
class BMIListIODataFlow : public DataFlow
{
public:
    /** List I/O data flow constructor */
    BMIListIODataFlow(const spfsDataFlowStart& flowStart,
                      std::size_t numBuffers,
                      FSSize bufferSize,
                      cSimpleModule* module);

    /** Destructor */
    virtual ~BMIListIODataFlow();

protected:
    /** Start the flow of data */
    virtual void startTransfer();

    /** Process BMI and Storage request and responses */
    virtual void processDataFlowMessage(cMessage* msg);

    /** Use standard BMI calls to send data over the network */
    virtual void pushDataToNetwork(FSSize pushSize);

    /** Use standard BMI calls to request data from the network */
    virtual void pullDataFromNetwork(FSSize pullSize);

    /** Use the ListIO interface to write data to disk */
    virtual void pushDataToStorage(FSSize pushSize);

    /** Use the ListIO interface to read data from disk */
    virtual void pullDataFromStorage(FSSize pullSize);

private:
    /** Hidden copy constructor */
    BMIListIODataFlow(const BMIListIODataFlow& other);

    /** Hidden assignment operator */
    BMIListIODataFlow& operator=(const BMIListIODataFlow& other);

    /** Send data receipt acknowledgement */
    void sendPushAck(FSSize amountRecvd);

    /** Update the bstream size to include the final byte */
    void updateBstreamSize(FSSize lastByteOffset);

    /** File used for I/O */
    Filename filename_;

    /** The module to send data over */
    cSimpleModule* module_;

    /** The BMI connection id */
    int bmiConnectionId_;

    /** The BMI tag to use for sending */
    int outboundBmiTag_;

    /** The offset into the layout subregions for push operations */
    FSOffset pullSubregionOffset_;

    /** The offset into the layout subregions for pull operations */
    FSOffset pushSubregionOffset_;

    /** Metadata for the pfs file being written to */
    FSMetaData* pfsMetaData_;

    /** The index into the metadata bstreamSize field */
    std::size_t bstreamSizeIdx_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
