#!/usr/bin/perl -w
#
# Copyright 2008 Brad Settlemyer
#
# Program for converting strace output into the serial HECIOS trace format
# Trace files must be generated with the following invocation:
#  strace -rT <execution cmd>
#
# Usage: strace2shtf.pl [-d mountDir] <trace_file>
#
#
use File::Basename;
use File::Temp qw/ tempfile tempdir /;
use Getopt::Std;
use strict;

#
# Global data
#
my $g_traceTimeStamp = 0.0;
my $g_recordCount = 0;

#
# Global metadata structures
#
my %g_descriptorToFilename;
my %g_descriptorToFilePointer;
my %g_filenameToSize;
my %g_ignoredDescriptors;

#
# Invoke Main
#
my $g_mainrc = main();
exit($g_mainrc);

#
# Open the input file for reading
#
sub openInputFile
{
    my $filename = shift;
    local *IN;

    # Open the input file
    my $isOpen = open(IN, "<$filename");
    if (!$isOpen)
    {
        die "Unable to open input file: $filename\n";
    }
    return *IN{IO};
}

#
# Open the output file for writing
#
sub openOutputFile
{
    my $filename = shift;
    local *OUT;

    # Open the output file
    my $isOpen = open(OUT, ">$filename");
    if (!$isOpen)
    {
        die "Unable to open output file: $filename\n";
    }
    return *OUT{IO};
}

#
# Add the filename and descriptor to the data structures
#
sub addFilenameAndDescriptor
{
    my $filename = shift;
    my $fd = shift;

    $g_descriptorToFilename{$fd} = $filename;
    $g_descriptorToFilePointer{$fd} = 0;

    # If the filename hasn't already been opened, set it's size to 0
    if (!$g_filenameToSize{$filename})
    {
        $g_filenameToSize{$filename} = 0;
    }
}

#
# Update the file pointer for a read of size extent
#
sub updateFilePointer
{
    my $fd = shift;
    my $extent = shift;
    $g_descriptorToFilePointer{$fd} += $extent;
}

#
# Update the maximum size for the file if neccesary
#
sub updateFileSize
{
    my $fd = shift;
    my $offset = shift;
    my $extent = shift;

    # Update the filesize to include this access if necessary
    my $filename = $g_descriptorToFilename{$fd};
    my $newSize = $offset + $extent;
    if ($g_filenameToSize{$filename} < $newSize)
    {
        $g_filenameToSize{$filename} = $newSize;
    }
}

#
# Ignore the descriptor
#
sub ignoreDescriptor
{
    my $fd = shift;
    $g_ignoredDescriptors{$fd} = 1;
}

#
# Remove the descriptor from the ignore list
#
sub removeIgnoredDescriptor
{
    my $fd = shift;
    delete($g_ignoredDescriptors{$fd});
}

#
# return true if the the descriptor is ingnored, otherwise false
#
sub isDescriptorIgnored
{
    my $fd = shift;
    return $g_ignoredDescriptors{$fd}
}

#
# return true if the file is on the mounted PFS volume
#
sub isFileInPFS
{
    my $mountDir = shift;
    my $filename = shift;

    # For now, we only need to handle the local directory
    my $match = substr($filename, 0, 5);
    if ($match eq "linux")
    {
        return 1;
    }
    else
    {
        return !1;
    }
}

#
# Parse a descriptor close call, updating the ignore list if this was an
# ignored descriptor
#
sub parseCloseCall
{
    my $line = shift;
    my $recordFile = shift;

    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my $rc = $line;
    $rc =~ s/.* = //;

    my $descriptor = $args[0];

    return "CLOSE $descriptor $rc";
}

#
# parse a chmod system call and return a trace record
#
sub parseChmodCall
{
    my $line = shift;
    print "Error: not parsing $line";
    return "Unparsed line: $line\n";
}

#
# Parse an fcntl or ioctl call and construct a trace record
#
sub parseFcntlCall
{
    my $line = shift;
    my $recordFile = shift;

    # Parse the read record
    my @stuff = split(/\(/, $line);
    my @args = split(/, /, $stuff[1]);
    my $rc = $line;
    $rc =~ s/.* = //;

    my $cmd = uc($stuff[0]);
    my $descriptor = $args[0];

    return "$cmd $descriptor $rc";
}

#
# Parse an fstat call and construct a trace record
#
sub parseFstatCall
{
    my $line = shift;
    my $recordFile = shift;

    # Parse the read record
    my @stuff = split(/\(/, $line);
    my @args = split(/, /, $stuff[1]);
    my $rc = $line;
    $rc =~ s/.*\".*\".* = //;

    my $cmd = uc($stuff[0]);
    my $descriptor = $args[0];

    return "$cmd $descriptor $rc";
}

#
# Parse a read or write record and constrct a trace record if the 
#
sub parseIOCall
{
    my $line = shift;
    my $recordFile = shift;

    # Parse the read record
    my @stuff = split(/\(/, $line);
    my @args = split(/, /, $stuff[1]);
    my $rc = $line;
    $rc =~ s/.*\".*\".* = //;

    my $ioType = uc($stuff[0]);
    my $descriptor = $args[0];
    my $offset = $g_descriptorToFilePointer{$descriptor};
    my $extent = $rc;

    # Update the global trace metadata
    #print "Desc: $descriptor Off: $offset  Ext: $extent\n";
    updateFileSize($descriptor, $offset, $extent);

    # Update the file pointer
    updateFilePointer($descriptor, $extent);

    return "$ioType $descriptor $offset $extent";
}

#
# Parse a mkdir call and construct a trace record
#
sub parseMkdirCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my @toks = split(/([\[\]])/, $line);

    # Remove the surrounding "" from the filename
    my $filename = substr($args[0], 1);
    chop($filename);
    my $perms = $args[2];

    return "MKDIR $filename $perms"
}

#
# Parse a positioned read or write record and constrct a trace record if the 
#
sub parsePIOCall
{
    my $line = shift;
    my $recordFile = shift;

    # Parse the read record
    my @stuff = split(/\(/, $line);
    my @args = split(/, /, $stuff[1]);

    my $ioType = uc($stuff[0]);
    my $descriptor = $args[0];

    $line =~ /(\d+)\)/;
    my $offset = $1; 

    my $rc = $line;
    $rc =~ s/.*\".*\".* = //;
    my $extent = $rc;

    # Update the global trace metadata
    #print "Desc: $descriptor Off: $offset  Ext: $extent\n";
    updateFileSize($descriptor, $offset, $extent);

    return "$ioType $descriptor $offset $extent";
}

#
# Parse an open call to create an open trace record.  If the file being
# opened is not part of the parallel file system, add it to the ignore list
#
sub parseOpenCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my $rc = $line;
    $rc =~ s/.* = //;

    # Remove the surrounding "" from the filename
    my $filename = substr($args[0], 1);
    chop($filename);
    my $openFlags = $args[2];
    my $descriptor = $rc;

    # Associate the filename and descriptor
    addFilenameAndDescriptor($filename, $descriptor);

    return "OPEN $filename $openFlags $descriptor";
}

#
# Parse a socket call in order to ignore the socket descriptor
#
sub parseSocketCall
{
    my $line = shift;

    # Determine the socket descriptor
    my $rc = $line;
    $rc =~ s/.* = //;
    my $socketDescriptor = $rc;
    my $socketName = "/tmp/$socketDescriptor";

    # Associate the filename and descriptor
    addFilenameAndDescriptor($socketName, $socketDescriptor);

    return "SOCKET $socketName NULL $socketDescriptor";
}

#
# Parse a utime call and add a trace record if the file is on a PFS volume
#
sub parseUtimeCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my @toks = split(/([\[\]])/, $line);

    # Remove the surrounding "" from the filename
    my $filename = substr($args[0], 1);
    chop($filename);
    my $time = qq("$toks[2]");

    return "UTIME $filename $time";
}

#
# Parse an unlink system call and construct a trace record
#
sub parseUnlinkCall
{
    my $line = shift;
    my $recordFile = shift;
    print "ERROR: Don't handle: $line";
    return "$line";
}

#
# Parse a file stat system call and construct a trace record
#
sub parseStatCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my @toks = split(/([\[\]])/, $line);
    my $rc = $line;
    $rc =~ s/.*\".*\".* = //;

    # Remove the surrounding "" from the filename
    my $filename = substr($args[0], 1);
    chop($filename);

    return "STAT $filename $rc";
}

#
# Process the trace record and only return it if it is interesting
#
sub processTraceRecord
{
    my $traceRecord = shift;
    my $mountDir = shift;

    my $output;
    my @fields = split(/ /, $traceRecord);
    my $cmd = $fields[0];

    my $processedRecord;
    if ($cmd =~ /OPEN|SOCKET/)
    {
        my $filename = $fields[1];
        my $fd = $fields[3];
        if (isFileInPFS($mountDir, $filename))
        {
            $processedRecord = $traceRecord;
        }
        else
        {
            ignoreDescriptor($fd);
        }
    }
    elsif ("CLOSE" eq $cmd)
    {
        my $fd = $fields[1];
        if (isDescriptorIgnored($fd))
        {
            removeIgnoredDescriptor($fd);
        }
        else
        {
            $processedRecord = $traceRecord;
        }
    }
    elsif ($cmd =~ /MKDIR|STAT|UTIME/)
    {
        my $filename = $fields[1];
        if (isFileInPFS($mountDir, $filename))
        {
            $processedRecord = $traceRecord;
        }
    }
    elsif ($cmd =~ /FCNTL|IOCTL|READ|SEEK|WRITE/)
    {
        my $fd = $fields[1];
        if (!isDescriptorIgnored($fd))
        {
            $processedRecord = $traceRecord;
        }
    }
    else
    {
        print "ERROR: Unable to process: $traceRecord\n";
    }
    
    return $processedRecord;
}

#
# Emit record data
#
sub emitTraceRecord
{
    my $recordFile = shift;
    my $traceRecord = shift;
    my $startTime = shift;
    my $duration = shift;

    print $recordFile "$traceRecord $startTime $duration\n";
}

#
# return true if able to process $line, false if $line was in an unknown
#  format
#
sub processStraceLine
{
    my $line = shift;
    my $metaDataFile = shift;
    my $recordDataFile = shift;

    # Extract the relative time from the record beginning
    my $relativeTime = substr($line, 0, 13);
    $g_traceTimeStamp += $relativeTime;

    # Extract the system call from the line
    my $syscall = substr($line, 14);
    $syscall =~ s/ <\d+\.\d+>//;

    # Possible interesting operations of open, read, write, close, and utime
    my @cmd = split(/\(/, $syscall);
    my $token = $cmd[0];
    my $traceRecord;
    if ("chmod" eq $token)
    {
        $traceRecord = parseChmodCall($syscall);
    }
    elsif ("close" eq $token)
    {
        $traceRecord = parseCloseCall($syscall);
    }
    elsif ("fcntl" eq $token || "ioctl" eq $token)
    {
        $traceRecord = parseFcntlCall($syscall);
    }
    elsif ("fstat" eq $token)
    {
        $traceRecord = parseFstatCall($syscall);
    }
    elsif ("mkdir" eq $token)
    {
        $traceRecord = parseMkdirCall($syscall);
    }
    elsif ("open" eq $token)
    {
        $traceRecord = parseOpenCall($syscall);
    }
    elsif ("pread" eq $token || "pwrite" eq $token)
    {
        $traceRecord = parsePIOCall($syscall);
    }
    elsif ("read" eq $token || "write" eq $token)
    {
        $traceRecord = parseIOCall($syscall);
    }
    elsif ("socket" eq $token)
    {
        $traceRecord = parseSocketCall($syscall);
    }
    elsif ("stat" eq $token)
    {
        $traceRecord = parseStatCall($syscall);
    }
    elsif ("unlink" eq $token)
    {
        $traceRecord = parseUnlinkCall($syscall);
    }
    elsif ("utime" eq $token)
    {
        $traceRecord = parseUtimeCall($syscall);
    }

    # Emit the trace record if necessary
    my $ignoredCommands = "access|arch_prctl|bind|brk|connect|execve" .
        "|exit_group|geteuid|getpid|mmap|mprot|munmap|poll|recvfrom" .
        "|rt_sigaction|sendto|setsockopt|umask|uname";
    my $returnCode = !1;
    if ($traceRecord)
    {
        # Extract the call time field from the end of the line
        $line =~ m/<(\d+\.\d+)>/;
        my $callTime = $1;
        if (processTraceRecord($traceRecord))
        {
            $g_recordCount++;
            emitTraceRecord($recordDataFile, 
                            $traceRecord, 
                            $g_traceTimeStamp, 
                            $callTime);
        }
        $returnCode = 1;
    }
    elsif ($token =~ /$ignoredCommands/)
    {
        # The following commands are ignored and should be considered
        # successfully parsed
        $returnCode = 1;
    }
    else
    {
        print "ERROR: Could not identify: $token\n";
    }

    return $returnCode;
}

#
# Write the trace metadata to $outFile
#
sub emitTraceMetaData
{
    my $outFile = shift;

    # Write out the format metadata
    print $outFile "# Serial HECIOS Trace Format Version 0.0.0.0\n";
    
    # Write out the number of files and records
    my $numFiles = scalar(keys %g_filenameToSize);
    print $outFile "$numFiles $g_recordCount\n";

    # Write out the file names and their sizes
    my $i = 0;
    my $filename;
    foreach $filename (keys %g_filenameToSize)
    {
        my $fileSize = $g_filenameToSize{$filename};
	print $outFile "$filename $fileSize\n";
        $i++;
    }    
}

#
# Main
#
sub main
{
    # Validate the program input
    my $numArgs = $#ARGV + 1;
    if (1 > $numArgs)
    {
        print "Usage: $0 [-m dir] <strace file>\n";
        exit 1;
    }

    # Construct the input filename
    my $inputFilename = $ARGV[0];

    # Construct the output filename
    my $outputFilename = basename($inputFilename);
    $outputFilename =~ s/\..*/\.shtf/;

    # Determine the parallel file system mount point
    my %options = ();
    getopts("d:", \%options);
    my $mountDir = $options{"d"};
    if (!$mountDir)
    {
        $mountDir = "./";
    }

    # Print out the settings for this invocation
    print "Input File: $inputFilename\n"
        . "Output File: $outputFilename\n"
        . "PFS Mountpoint: $mountDir\n";
    
    # Open the input file
    my $inputFile = openInputFile($ARGV[0]);

    # Open the temp file used to hold record data
    my $tempFile = tempfile();

    # Open the output file for storing trace output
    my $outputFile = openOutputFile($outputFilename);

    my $lineNum = 1;
    while (<$inputFile>)
    {
        my $line = $_;
        chomp($line);
        my $rc = processStraceLine($line, $outputFile, $tempFile);
        if (!$rc)
        {
            print "Unable to parse line $lineNum: $line\n";
        }
        $lineNum++;
    }

    # Add all of the trace metadata to the output file
    emitTraceMetaData($outputFile);

    # Move the file pointer to the beginning of the temp file, and append it
    # to the output file
    seek($tempFile, 0, 0);
    while (<$tempFile>)
    {
        print $outputFile $_;
    }

    # Close the files
    close($inputFile);
    close($tempFile);
    close($outputFile);
    return 0;
}

