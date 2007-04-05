//-------------------------------------------------------------
// file: blocktrans.ned
//        (part of File System Simulator)
//
//  $Joel-Id: blocktrans.ned,v 1.3 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------


//
// Abstract interface for Block Translators
//
// Implemented Block Translators:
// -  NoTranslation: Simply pass through the request
//
simple ABlockTranslator
  gates:
    in: in;
    out: out;
endsimple



