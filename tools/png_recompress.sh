#!/bin/bash
# $Id$

#  Copyright (C) 2007-2008 Arvid Norlander <anmaster AT tele2 DOT se>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# This script recompresses .png files using optipng and
# advpng to get the smallest images. All recompression is
# lossless.
#
# This script needs at least bash3, bash2 will not work
#
# TODO:
#  * Make it work recursively on a directory.

# Check for new enough bash version
fail_old_bash() {
	echo "Sorry your bash version is too old!"
	echo "You need at least version 3.0 of bash."
	echo "Please install a newer version:"
	echo " * Either use your distro's packages."
	echo " * Or see http://www.gnu.org/software/bash/"
	exit 2
}

# Check bash version. We need at least 3.1
# Lets not use anything like =~ here because
# that may not work on old bash versions.
if [[ "${BASH_VERSINFO[0]}" -lt 3 ]]; then
	fail_old_bash
fi

if [[ -z "$1" ]] || [[ "$1" == "--help" ]]; then
	echo "Usage: $(basename $0) files..."
	exit 1
fi

if ! type optipng > /dev/null 2>&1; then
	echo "Can't find optipng!"
	echo "This script depends on the optipng tool to be in PATH."
	echo "Please install it or, if it is already installed add the"
	echo "directory it is in to PATH and try again."
	echo "Homepage of this tool is: http://optipng.sourceforge.net/"
	exit 1
fi

if ! type advpng > /dev/null 2>&1; then
	echo "Can't find advpng!"
	echo "This script depends on the advpng tool to be in PATH."
	echo "Please install it or, if it is already installed add the"
	echo "directory it is in to PATH and try again."
	echo "Homepage of this tool is: http://advancemame.sourceforge.net/comp-readme.html"
	echo "Hint: For package name in your distro, try looking for \"advancecomp\"."
	exit 1
fi

echo -e "Please wait, this can take a \e[1mlong\e[0m time."

dooptipng() {
	optipng -i 0 -o 7 "$@" | \
		awk '
			/^\*\* Processing:/ { print "\nFile:   " $3 }
			/^Input file size/ { print "Input:  " $5,$6 }
			/^Output file size/ { print "Output: " $5,$6,$7,$8,$9,$10,$11 }
			/is already optimized/ { print "Output: No change" }
		'
}

doadvpng() {
	echo "         In          Out   %  Filename"
	advpng -z -4 "$@"
}

echo -e "\n\n\n\e[1mPass 1: optipng\e[0m\n\n\n"
dooptipng "$@"

echo -e "\n\n\n\e[1mPass 2: advpng\e[0m\n\n\n"
doadvpng "$@"

echo -e "\n\n\n\e[1mPass 3: optipng again (as advpng often makes optipng more effective)\e[0m\n\n\n"
dooptipng "$@"
