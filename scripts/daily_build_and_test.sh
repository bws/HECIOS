#!/bin/sh

now=`date +"%b-%d-%y"`
hecios_dir=hecios-$now

#
# Perform checkout
#
cd /tmp
cvs -d :pserver:anonymous:@cvs.parl.clemson.edu:/projects/cvsroot login
cvs co -d $hecios_dir hecios

#
# Perform build and log to file
#
build_log=$hecios_dir/build_log.txt
export PATH=$PATH:/opt/omnetpp-3.3
cd $hecios_dir
./configure --with-omnet=/opt/omnetpp-3.3
make >$build_log
echo "\n\n================ Begin Make Test ===============\n\n">>$build_log
make test >>$build_log

#
# Run test drivers
#
test_log=$hecios_dir/test_log.txt
bin/common_test >$test_log

#
# Send mail with log files attached
#

