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

// Abstract interface for all Hard Disks
simple HardDisk
    gates:
        in: in;
        out: out;
endsimple

//
// Hard Disk Model that uses the simplified model examined in the FSS
// simulator
//
simple BasicModelDisk
    parameters:
        fixedControllerReadOverheadSecs: const,
        fixedControllerWriteOverheadSecs: const,
        trackSwitchTimeSecs: const,
        averageReadSeekSecs: const,
        averageWriteSeekSecs: const,
        numCylinders: const,
        numHeads: const,
        numSectors: const,
        sectorsPerTrack: const,
        rpm: const;

    gates:
        in: in;
        out: out;
endsimple  

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
