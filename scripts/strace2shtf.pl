#!/usr/bin/perl -w
#
# This file is part of Hecios
#
# Copyright 2008,2009 Brad Settlemyer
#
# ALL RIGHTS RESERVED.
#

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
my $g_currentDir = "/mnt/pvfs2";
my $g_mountDir = "/mnt/pvfs2";

#
# Global metadata structures
#
my %g_descriptorToFilename;
my %g_descriptorToFilePointer;
my %g_filenameToSize;
my %g_ignoredDescriptors;
my %g_dirnameToEntries;

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
    print "This script constructs an SHTF file compatible with HECIOS for\n";
    print "use in simulation runs.  The trace file should be generated with\n";
    print "the following command: \n";
    print "    strace -rT -o <output_file> <execution cmd> \n";
    print "\n";

    # Usage
    my $progName = basename($0);
    print "Usage: $progName <options> strace_file\n";
    print "Options:\n";
    print "  -C <current_dir>     the current working directory when the\n";
    print "                         trace was collected\n";
    print "  -d <mount_dir>       the mount point of the PVFS file system\n";
    print "                         when the trace ran\n";
}

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
# Add the filename to the global data
#
sub addFilename
{
    my $filename = shift;

    # If the filename hasn't already been opened, set it's size to 0
    if (!(exists $g_filenameToSize{$filename}))
    {
        $g_filenameToSize{$filename} = 0;
    }    
}

#
# Add the directory name to the global data
#
sub addDirectory
{
    my $dirName = shift;
    if (!(exists $g_dirnameToEntries{$dirName}))
    {
        $g_dirnameToEntries{$dirName} = 0;
    }
}

#
# Add the filename and descriptor to the data structures
#
sub addFilenameAndDescriptor
{
    my $filename = shift;
    my $fd = shift;

    addFilename($filename);
    $g_descriptorToFilename{$fd} = $filename;
    $g_descriptorToFilePointer{$fd} = 0;
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
# Update the number of directory entries
#
sub updateDirectoryEntries
{
    my $fd = shift;
    my $numEntries = shift;

    my $dirName = $g_descriptorToFilename{$fd};
    #print "Diag: For $dirName adding $numEntries entries\n";
    if ($g_dirnameToEntries{$dirName})
    {
        addDirectory($dirName);
    }
    $g_dirnameToEntries{$dirName} += $numEntries;
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
    $mountDir = $mountDir . "/";
    my $match = substr($filename, 0, 11);
    if ($mountDir eq $match)
    {
        return 1;
    }
    else
    {
        return !1;
    }
}

#
# If the file is a relative path, prepend the trace's current working dir
#
sub fixRelativePath
{
    my $filename = shift;

    # Process out special path characters /., /.., ., and ..
    if ("." eq $filename)
    {
        $filename = $g_currentDir;
    }
    elsif (".." eq $filename)
    {
        #my $upOneDir = dirname($g_currentDir);
        #print "On receiving .. changing: $filename -> $upOneDir\n";
        $filename = dirname($g_currentDir);
    }
    elsif ($filename =~ m/.*\/\.$/)
    {
        my $oldName = $filename;
        $filename =~ s/\/\.$//;
        #print "Caught a trailing dot: $oldName $filename\n";
    }
    elsif ($filename =~ m/.*\/\.\.$/)
    {
        my $oldName = $filename;
        $filename =~ s/\/[^\/]+\/\.\.$//;
        #print "Caught a double trailing dot: $oldName $filename \n";
    }
    
    # If the path is relative, prepend the current directory
    if ("/" ne substr($filename, 0, 1))
    {
        $filename = $g_currentDir . "/" . $filename;
    }
    return $filename;
}

#
# Remove the mount point from the beginning of the path
#
sub fixPFSPath
{
    my $filename = shift;

    # Replace the PFS mount volume with a simple root directory
    #print stderr "Fixng PFS path: $filename $g_mountDir\n";
    my $pfsFilename = "/";
    if ($filename ne $g_mountDir)
    {
        $pfsFilename .= substr($filename, length($g_mountDir) + 1);
    }
    return $pfsFilename;
}

#
# Return a CPU Phase record
#
sub getCPUPhaseRecord
{
    return "CPU_PHASE";
}

#
# Parse a file access call
#
sub parseAccessCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my @toks = split(/([\[\]])/, $line);
    my $rc = $line;
    $rc =~ s/.* = //;

    # Remove the surrounding "" from the filename
    my $filename = substr($args[0], 1);
    chop($filename);
    $filename = fixRelativePath($filename);
    my $perms = $args[1];

    # Update meta data
    addFilename($filename);

    return "ACCESS $filename $perms $rc";
}

#
# Parse a chdir call.  This one isn't like the rest.  It changes the parser
# state, and then emits a CPU PHASE record.
#
sub parseChdirCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my @toks = split(/([\[\]])/, $line);
    my $rc = $line;
    $rc =~ s/.* = //;

    # Remove the surrounding "" from the filename
    my $filename = substr($args[0], 1);
    chop($filename);
    $filename = fixRelativePath($filename);

    # Add the directory
    addFilename($filename);
    addDirectory($filename);

    # Update the current working directory
    $g_currentDir = $filename;
    #print "WARNING: Set the current working dir to $g_currentDir\n";

    return getCPUPhaseRecord();
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
# Parse a fchdir call.  This one isn't a typical command record.  It changes 
# the parser state (current directory), and then emits a CPU PHASE record.
#
sub parseFchdirCall
{
    my $line = shift;
    my $recordFile = shift;
    my $mountDir = shift;

    # Parse the record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);

    # Remove the surrounding "" from the filename
    my $descriptor = $args[0];

    # Update the current working directory
    $g_currentDir = $g_descriptorToFilename{$descriptor};
    #print "WARNING: Set the current working dir to $g_currentDir\n";
    
    # Add the direcotry to the metadata
    addDirectory($g_currentDir);

    return getCPUPhaseRecord();
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
    #print "Type: $ioType Desc: $descriptor Off: $offset  Ext: $extent\n";
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
    $filename = fixRelativePath($filename);
    my $perms = $args[2];

    # Update meta data
    addFilename($filename);
    addDirectory($filename);

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
    $filename = fixRelativePath($filename);

    my $openFlags = $args[2];
    my $descriptor = $rc;

    # Associate the filename and descriptor
    if ("-1 ENOENT (No such file or directory)" ne $descriptor)
    {
        addFilenameAndDescriptor($filename, $descriptor);
    }

    return "OPEN $filename $openFlags $descriptor";
}

#
# Parse a Readdir call and construct a trace record
#
sub parseReaddirCall
{
    my $line = shift;
    my $recordFile = shift;

    # Parse the read record
    my @stuff = split(/\(/, $line);
    my @args = split(/([,\)]) /, $stuff[1]);
    my $rc = $line;
    $rc =~ s/.* = //;

    my $cmd = uc($stuff[0]);
    my $descriptor = $args[0];
    my $count = $args[4] / 1048576;

    # Update the number of directory entries
    updateDirectoryEntries($descriptor, $count);

    return "READDIR $descriptor $count $rc";
}

#
# Parse an rmdir system call and construct a trace record
#
sub parseRmdirCall
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
    $filename = fixRelativePath($filename);

    # Update meta data
    addFilename($filename);
    addDirectory($filename);

    return "RMDIR $filename $rc";
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
    $filename = fixRelativePath($filename);

    # Update meta data
    addFilename($filename);

    return "STAT $filename $rc";
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
    $filename = fixRelativePath($filename);
    my $time = qq("$toks[2]");

    # Update meta data
    addFilename($filename);

    return "UTIME $filename $time";
}

#
# Parse an unlink system call and construct a trace record
#
sub parseUnlinkCall
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
    $filename = fixRelativePath($filename);

    # Update meta data
    addFilename($filename);

    return "UNLINK $filename $rc";
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

    my $processedRecord = getCPUPhaseRecord();
    if ($cmd =~ /OPEN|SOCKET/)
    {
        my $filename = $fields[1];
        my $fd = $fields[3];
        if (isFileInPFS($g_mountDir, $filename))
        {
            # Rewrite the trace record for our purposes
            my $pfsFilename = fixPFSPath($filename);
            my $newRecord = "$fields[0] $pfsFilename $fields[2] $fields[3]";
            $processedRecord = $newRecord;
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
    elsif ($cmd =~ /ACCESS|MKDIR|RMDIR|STAT|UNLINK|UTIME/)
    {
        my $filename = $fields[1];
        if (isFileInPFS($g_mountDir, $filename))
        {
            # Rewrite the trace record for our purposes
            my $pfsFilename = fixPFSPath($filename);
            my $newRecord = "$fields[0] $pfsFilename $fields[2]";
            $processedRecord = $newRecord;
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
    elsif ("CPU_PHASE" eq $cmd)
    {
        # Do nothing
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

    # Print the data to the record file
    print $recordFile "$startTime $duration $traceRecord\n";
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

    # Extract the call time field (duration) from the end of the line
    $line =~ m/<(\d+\.\d+)>/;
    my $callTime = $1;

    # Extract the system call from the line
    my $syscall = substr($line, 14);
    $syscall =~ s/ <\d+\.\d+>//;

    # The list of tokens to emit CPU time rather than a record for
    my $ignoredCommands = 
        "_sysctl|arch_prctl|bind|brk|" .
        "clock_gettime|connect|" .
        "execve|futex|" .
        "geteuid|getrlimit|getpid|getxattr|" .
        "mmap|mprot|munmap|poll|recvfrom|" .
        "rt_sigaction|rt_sigprocmask|" .
        "sendto|set_tid_address|setsockopt|umask|uname";

    # Possible interesting operations of open, read, write, close, and utime
    my @cmd = split(/\(/, $syscall);
    my $token = $cmd[0];
    my $traceRecord;
    if ($token =~ /$ignoredCommands/)
    {
        $traceRecord = getCPUPhaseRecord();
    }
    elsif ("access" eq $token)
    {
        $traceRecord = parseAccessCall($syscall);
    }
    elsif ("chdir" eq $token)
    {
        $traceRecord = parseChdirCall($syscall);
    }
    elsif ("chmod" eq $token)
    {
        $traceRecord = parseChmodCall($syscall);
    }
    elsif ("close" eq $token)
    {
        $traceRecord = parseCloseCall($syscall);
    }
    elsif ("exit_group" eq $token)
    {
        $traceRecord = getCPUPhaseRecord();
        $callTime = 0.0;
    }
    elsif ("fchdir" eq $token)
    {
        $traceRecord = parseFchdirCall($syscall);
    }
    elsif ("fcntl" eq $token || "ioctl" eq $token)
    {
        $traceRecord = parseFcntlCall($syscall);
    }
    elsif ("fstat" eq $token)
    {
        $traceRecord = parseFstatCall($syscall);
    }
    elsif ("getdents64" eq $token)
    {
        $traceRecord = parseReaddirCall($syscall);
    }
    elsif ("lstat" eq $token)
    {
        $traceRecord = parseStatCall($syscall);
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
    elsif ("readdir" eq $token)
    {
        $traceRecord = parseReaddirCall($syscall);
    }
    elsif ("rmdir" eq $token)
    {
        $traceRecord = parseRmdirCall($syscall);
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
    else
    {
        # Print out the unidentified token
        print "Unable to identify: $token\n";
        
    }

    my $returnCode = !1;
    if ($traceRecord)
    {
        my $processedRecord = processTraceRecord($traceRecord);
        if ($processedRecord)
        {
            $g_recordCount++;
            emitTraceRecord($recordDataFile, 
                            $processedRecord, 
                            $g_traceTimeStamp, 
                            $callTime);
        }
        $returnCode = 1;
    }
    else
    {
        print "ERROR: Could not parse: $syscall\n";
    }

    return $returnCode;
}

#
# process the trace meta data to cull files not in the PFS volume
#
sub processTraceMetaData
{
    # Purge files that are not in the PFS volume
    my $filename;
    foreach $filename (keys %g_filenameToSize)
    {
        if (! isFileInPFS($g_mountDir, $filename))
        {
            print "Ignoring file: $filename \n";
            delete $g_filenameToSize{$filename};
        }
    }    
}

#
# Write the trace metadata to $outFile
#
sub emitTraceMetaData
{
    my $outFile = shift;

    # Array to hold the meta entries
    my @metaEntries;
    
    # Extract the directories and the number of entries
    my $dirName;
    foreach $dirName (sort(keys %g_dirnameToEntries))
    {
        my $numEntries = $g_dirnameToEntries{$dirName};
        my $pfsDirName = fixPFSPath($dirName);
        push(@metaEntries, "$pfsDirName D $numEntries\n");
    }

    #  Extract the file names and their sizes
    my $i = 0;
    my $filename;
    foreach $filename (sort(keys %g_filenameToSize))
    {
        if (!(exists $g_dirnameToEntries{$filename}))
        {
            my $fileSize = $g_filenameToSize{$filename};
            my $pfsFilename = fixPFSPath($filename);
        
            push(@metaEntries, "$pfsFilename F $fileSize\n");
        }
    }

    # Write out the format metadata
    print $outFile "# Serial HECIOS Trace Format Version 0.0.1.0\n";
    
    # Write the metadata header to file
    my $numMetaEntries = @metaEntries;
    print $outFile "$numMetaEntries $g_recordCount\n";
    
    # Write the metadata entries to file
    my $entryLine;
    foreach $entryLine (@metaEntries)
    {
        print $outFile $entryLine;
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
        printUsage();
        exit 1;
    }

    # Construct the input filename
    my $inputFilename = $ARGV[0];

    # Construct the output filename
    my $outputFilename = basename($inputFilename);
    $outputFilename =~ s/\..*/\.shtf/;

    # Parse command line arguments
    my %options = ();
    getopts("C:d:h", \%options);

    # Print the help information if requested
    if ($options{"h"})
    {
        printUsage();
        exit 1;
    }

    # Determine the trace's current working directory
    if ($options{"C"})
    {
        $g_currentDir = $options{"C"};
    }

    # Determine the parallel file system mount point
    if ($options{"d"})
    {
        $g_mountDir = $options{"d"};
    }

    # Print out the settings for this invocation
    print "Input File: $inputFilename\n"
        . "Output File: $outputFilename\n"
        . "Trace Relative Path: $g_currentDir\n"
        . "PFS Mountpoint: $g_mountDir\n";
    
    # Ignore stdin, stdout, and stderr descriptors
    addFilenameAndDescriptor("stdin", 0);
    ignoreDescriptor(0);
    addFilenameAndDescriptor("stdout", 1);
    ignoreDescriptor(1);
    addFilenameAndDescriptor("stderr", 2);
    ignoreDescriptor(2);
    print "Ignoring default descriptors: stdout(0) stdin(1) stderr(2)\n";

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
    processTraceMetaData();
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

