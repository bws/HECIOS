#!/usr/bin/perl
#
# Copyright 2008 Wu Yang, Brad Settlemyer
#
# Parses LANL formatted trace files.  Requires you to be in a directory
# full of LANL trace information, and the lanl_trace_parser executable
# to be in your path.  Also probably requires some configuration files.
# I do not know all of the requirements.
#
# Usage: lanl2phtf.pl
#
#
use File::Basename;
use File::Temp qw/ tempfile tempdir /;
use Getopt::Std;
use strict;

#
# Global metadata structures
#
my %g_traceFilenameToRank;

#
# Invoke Main
#
my $g_mainrc = main();
exit($g_mainrc);

#
# Print out the usage statement
#
sub printUsage
{
    # Description
    print "This script constructs an PHTF file compatible with HECIOS for\n";
    print "use in simulation runs.  The trace file should be generated with\n";
    print "the following command: \n";
    print "\n";

    # Usage
    my $progName = basename($0);
    print "Usage: $progName <options> strace_file\n";
    print "Options:\n";
}

#
# Map the trace filenames to a process rank
#
sub mapTraceFilenamesToRank
{
    my $traceDir = shift;

    # Open the timing file
    my $timingFile = "$traceDir/timing";
    my $isOpen = open(TIMING, $timingFile);
    if (!$isOpen)
    {
        die("Unable to open timing file: $timingFile");
    }

    # Parse the timing file into an associative array
    my %nodeNameToRankPidMap = ();
    my $line;
    foreach $line (<TIMING>)
    {
        if ($line =~ /(\d+): ([\w\.]+) \((\d+)\)/ )
        {
            # Retrive the fields
            my $rank = $1;
            my $host = $2;
            my $pid = $3;
            
            # Organize unique rank mapping
            $nodeNameToRankPidMap{$host}{$rank} = $pid;                
        }
    }
    close TIMING;
    
    # Construct a mapping of tracefiles to ranks
    my @traceFilenames = <$traceDir/*.trace>;
    foreach my $fullFilename (@traceFilenames)
    {
        my $traceFilename = basename($fullFilename);
        my ($host, $pid, $ext) = split(/\./, $traceFilename);
        
        # Retrieve the next rank for this host
        for my $rank (sort(keys %{$nodeNameToRankPidMap{$host}}))
        {
            $g_traceFilenameToRank{$traceFilename} = $rank;
            last;
        }
        
        my $rank = $g_traceFilenameToRank{$traceFilename};
        # Remove this rank from the set
        delete($nodeNameToRankPidMap{$host}{$rank});
    }
    
    # Print out the final mapping
    #foreach my $traceFilename (sort(keys %g_traceFilenameToRank))
    #{
    #    print "Final Mapping: $traceFilename -> $g_traceFilenameToRank{$traceFilename} \n";
    #}
}

#
# Main
#
sub main
{
    # Validate the program input
    my $numArgs = $#ARGV + 1;
    if (0 > $numArgs)
    {
        printUsage();
        exit 1;
    }

    # Parse command line arguments
    my %options = ();
    getopts("C:d:h", \%options);

    # Print the help information if requested
    if ($options{"h"})
    {
        printUsage();
        exit 1;
    }

    # Determine the trace directory
    my $traceDir = ".";
    
    # Create the output directory
    my $outputDir = "./phtf";
    mkdir($outputDir, 0755);
    if ($!)
    {
        print "Output directory, phtf, already exists.\n";
        #exit 2;
    }
    
    # Perform the preprocessing
    mapTraceFilenamesToRank($traceDir);
    
    # Translate the trace files
    my @filenames = <$traceDir/*.trace>;
    for my $filename (@filenames)
    {
        my $traceFilename = basename($filename);
        my $rank = $g_traceFilenameToRank{$traceFilename};
        print "System Call(lanl_trace_scanner, $filename, $outputDir, $rank)\n";
        my $rc = system("lanl_trace_scanner", $filename, "$outputDir", $rank);
        if (-1 eq $rc)
        {
            print "The executable: lanl_trace_scanner was not in the PATH.\n";
        }
        elsif (0 ne $rc)
        {
            my $exitValue = $? >> 8;
            my $signalNum = $? & 127;
            my $dumpedCore = $? & 128;
            print "ERROR: Scanner returned: $rc Exit: $exitValue Sig: $signalNum Core: $dumpedCore\n";
        }
    }
    
    return 0;
}
