//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


// Description of the interface for all FSClients
simple FSClient
    parameters:
        useCollectiveCreate: bool,
        useCollectiveGetAttr: bool,
        useCollectiveRemove: bool,
        clientOverheadDelaySecs: numeric const,
        directoryCreateProcessingDelaySecs: numeric const,
        directoryReadProcessingDelaySecs: numeric const,
        directoryRemoveProcessingDelaySecs: numeric const,
        fileCloseProcessingDelaySecs: numeric const,
        fileDeleteProcessingDelaySecs: numeric const,
        fileOpenProcessingDelaySecs: numeric const,
        fileReadProcessingDelaySecs: numeric const,
        fileStatProcessingDelaySecs: numeric const,
        fileUpdateTimeProcessingDelaySecs: numeric const,
        fileWriteProcessingDelaySecs: numeric const;

    gates:
        in: appIn, netIn;
        out: appOut, netOut;
endsimple
