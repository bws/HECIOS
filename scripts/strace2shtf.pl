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

    # If the descriptor is not ignored, emit the trace record
    # Else remove the closed descriptor from the ignore list
    my $descriptor = $args[0];
    if (!isDescriptorIgnored($descriptor))
    {
        print $recordFile "CLOSE $descriptor $rc\n";
    }
    else
    {
        removeIgnoredDescriptor($descriptor);
        #print "Ignoring descriptor: $line\n";
    }
}

sub parseChmodCall
{
    my $line = shift;
    my $recordFile = shift;
    print "Unparsed line: $line\n";
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

    # If the descriptor is not ignored, emit the trace record
    if (!isDescriptorIgnored($descriptor))
    {
        print $recordFile "$cmd $descriptor $rc\n";
    }
    else
    {
        #print "Ignoring descriptor: $line\n";
    }
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
    print "Desc: $descriptor Off: $offset  Ext: $extent\n";
    updateFileSize($descriptor, $offset, $extent);

    # Update the file pointer
    updateFilePointer($descriptor, $extent);

    # If the descriptor is not ignored, emit the trace record
    if (!isDescriptorIgnored($descriptor))
    {
        print $recordFile "$ioType $descriptor $offset $extent\n";
    }
    else
    {
        #print "Ignoring descriptor: $line\n";
    }
}

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
    my $perms = $args[1];

    # If the file is a PFS file, record it
    # Otherwise make sure it is ignored
    if (isFileInPFS($mountDir, $filename))
    {
        # Emit the trace record
        print $recordFile "MKDIR $filename $perms\n";
    }
    else
    {
        #print "Ignoring file: $line\n";
    }
}

#
# Parse a positioned read or write record and constrct a trace record if the 
#
sub parsePIOCall
{
    my $line = shift;
    my $recordFile = shift;
    print "Unparsed record: $line\n";
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

    # Update the global trace metadata
    addFilenameAndDescriptor($filename, $descriptor);

    # If the file is a PFS file, record it
    # Otherwise make sure it is ignored
    if (isFileInPFS($mountDir, $filename))
    {
        # Emit the trace record
        print $recordFile "OPEN $filename $openFlags $descriptor\n";
    }
    else
    {
        ignoreDescriptor($descriptor);
        #print "Ignoring descriptor: $line\n";
    }
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

    ignoreDescriptor($rc);
    #print "Ignoring descriptor: $line\n";
    
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

    # If the file is a PFS file, record it
    # Otherwise make sure it is ignored
    if (isFileInPFS($mountDir, $filename))
    {
        # Emit the trace record
        print $recordFile "UTIME $filename $time\n";
    }
    else
    {
        #print "Ignoring file: $line\n";
    }
}

sub parseUnlinkCall
{
    my $line = shift;
    my $recordFile = shift;
    print "$line\n";
}

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

    # If the file is a PFS file, record it
    # Otherwise make sure it is ignored
    if (isFileInPFS($mountDir, $filename))
    {
        # Emit the trace record
        print $recordFile "STAT $filename $rc\n";
    }
    else
    {
        #print "Ignoring file: $line\n";
    }
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

    # Set whether the line was successfully parsed or not
    my $isParsed = 1;

    # Extract the relative time from the record beginning
    my $relativeTime = substr($line, 0, 13);
    $g_traceTimeStamp += $relativeTime;

    # Remove the call length field from the end of the system call
    my $syscall = substr($line, 14);
    $syscall =~ s/ <\d+\.\d+>//;

    # Possible interesting operations of open, read, write, close, and utime
    my @cmd = split(/\(/, $syscall);
    my $token = $cmd[0];
    if ("chmod" eq $token)
    {
        parseChmodCall($syscall, $recordDataFile);
    }
    elsif ("close" eq $token)
    {
        parseCloseCall($syscall, $recordDataFile);
    }
    elsif ("fcntl" eq $token || "ioctl" eq $token)
    {
        print "Do ioctl son\n";
    }
    elsif ("fstat" eq $token)
    {
        parseFstatCall($syscall, $recordDataFile);
    }
    elsif ("mkdir" eq $token)
    {
        parseMkdirCall($syscall, $recordDataFile);
    }
    elsif ("open" eq $token)
    {
        parseOpenCall($syscall, $recordDataFile);
    }
    elsif ("pread" eq $token || "pwrite" eq $token)
    {
        parsePIOCall($syscall, $recordDataFile);
    }
    elsif ("read" eq $token || "write" eq $token)
    {
        parseIOCall($syscall, $recordDataFile);
    }
    elsif ("socket" eq $token)
    {
        parseSocketCall($syscall);
    }
    elsif ("stat" eq $token)
    {
        parseStatCall($line, $recordDataFile);
    }
    elsif ("unlink" eq $token)
    {
        parseUnlinkCall($syscall, $recordDataFile);
    }
    elsif ("utime" eq $token)
    {
        parseUtimeCall($syscall, $recordDataFile);
    }
    else
    {
        # The following commands are ignored and should be considered
        # successfully parsed
        my $ignoredCommands = "access|arch_prctl|bind|brk|connect|execve" .
            "|exit_group|geteuid|getpid|mmap|mprot|munmap|poll|recvfrom" .
            "|rt_sigaction|sendto|setsockopt|umask|uname";

        if (!($token =~ /$ignoredCommands/))
        {
            $isParsed = !1;
        }
    }

    return $isParsed;
}

#
# Write the trace metadata to $outFile
#
sub emitTraceMetaData
{
    my $outFile = shift;

    my $i = 0;
    my $filename;
    foreach $filename (keys %g_filenameToSize)
    {
        my $fileSize = $g_filenameToSize{$filename};
	print $outFile "$i $filename $fileSize\n";
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

