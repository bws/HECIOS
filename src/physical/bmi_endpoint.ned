//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Model of a BMI endpoint
//
simple BMIEndpoint

{
    parameters:
        volatile double fixedOverheadSecs;
        volatile double scaledOverheadSecs;

    gates:
        input appIn;
        output appOut;

}
