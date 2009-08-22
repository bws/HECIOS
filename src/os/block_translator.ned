//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Module responsible for translating file system blocks to physical hardware
// addresses
//
moduleinterface BlockTranslator
{
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple NoTranslation like BlockTranslator
{
	@class(NoTranslation);
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple BasicTranslator like BlockTranslator
{
	@class(BasicTranslator);
    gates:
        input in;
        input response;
        output out;
        output request;
}



