#!/bin/sh
#
# This file is part of Hecios
#
# Copyright (C) 2007,2008,2009 Brad Settlemyer
#
# ALL RIGHTS RESERVED.
#

now=`date +"%b-%d-%y"`
hecios_dir=hecios-$now

#
# Perform checkout
#
cd /tmp
#cvs -d :pserver:anonymous:@cvs.parl.clemson.edu:/anoncvs login
cvs co -d $hecios_dir hecios

#
# Perform build and log to file
#
build_log=$hecios_dir/build_log.txt
export PATH=$PATH:/opt/omnetpp-3.3/bin
cd $hecios_dir
./configure --with-omnet=/opt/omnetpp-3.3 >$build_log
echo "\n\n================ Begin Toplevel Make ===============\n\n">>$build_log
make >$build_log
echo "\n\n================ Begin Make Test ===============\n\n">>$build_log
make test >>$build_log

#
# Run test drivers
#
test_log=$hecios_dir/test_log.txt
bin/client_test >$test_log
bin/common_test >>$test_log
bin/layout_test >>$test_log
bin/os_test >>$test_log

#
# Send mail with log files attached
#

