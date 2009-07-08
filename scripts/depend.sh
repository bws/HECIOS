#/bin/sh
#
# This file is part of Hecios
#
# Copyright (C) 2007,2009 Brad Settlemyer
#
# ALL RIGHTS RESERVED
#

#
# Usage: depend.sh <file_dir> <compiler> <compiler arguments>
#

#
# Retrieve arguments
#
dir="$1"
shift

#
# Path complement the directory
#
if [ -n "$dir" ]; then
  dir="$dir"/
fi

#
# Run the dependency generator, but path complement items on the *left*
# side of the colon
#
msg_dir="src/messages/"
add_dir_re="s@^\(.*\)\.o:@$dir\1.d $dir\1.o:@"
msg_dir_re="s@ \([A-Za-z0-9_]*\)_m\.h\s@ $msg_dir\1_m.h @g"
exec "$@" | sed -e "$add_dir_re" -e "$msg_dir_re"
exit $?
