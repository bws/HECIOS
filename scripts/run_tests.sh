#!/bin/sh

#
# Run test drivers nd log result to file and std out
#
echo "Running Client subsystem tests . . ."
bin/client_test 

echo "Running Common subsystem tests . . ."
bin/common_test

echo "Running Layout subsystem tests . . ."
bin/layout_test

echo "Running OS subsystem tests . . ."
bin/os_test

