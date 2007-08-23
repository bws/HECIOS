#/bin/sh
#
# This file is part of Hecios
#
# Copyright (C) 2007 Brad Settlemyer
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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