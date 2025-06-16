#!/opt/local/bin/perl

#
# makebundle.pl - copies and makes portable a binary and all its dependencies
# Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

use strict;
use warnings;

# Script does some dangerous stuff - make sure a user will double-check everything before running it. 
print "This script is highly experimental. Modify it to suit your needs and take care to run it only in controlled environments.\n";

# This line needs to be removed, the following lines adjusted.
exit 1;

my $OUTPUTDIR='/tmp/SuperTux.app/Contents/MacOS';
my $EXECUTABLE='./supertux2';

my %COPIED;
my @TOCOPY;
my %PATCHED;
my @TOPATCH;

system('mkdir '.$OUTPUTDIR);

push (@TOCOPY, $EXECUTABLE);

# first, copy all direct and indirect dependencies to OUTPUTDIR
while (my $EXECUTABLE = pop @TOCOPY) {

	next if $COPIED{$EXECUTABLE};
	$COPIED{$EXECUTABLE} = 1;

	next unless $EXECUTABLE =~ /^(.*)\/(.*)$/;
	my $PATH = $1;
	my $FNAME = $2;

	print("copying ".$EXECUTABLE."\n");

	system('cp '.$EXECUTABLE.' '.$OUTPUTDIR.'/'.$FNAME);
	system('chmod u+w '.$OUTPUTDIR.'/'.$FNAME);

	# mark the copy as to-be-patched
	push(@TOPATCH, $OUTPUTDIR.'/'.$FNAME);

	$EXECUTABLE = $OUTPUTDIR.'/'.$FNAME;

	my @LIBS = qx{/usr/bin/otool -L $EXECUTABLE};
	while ($_ = pop @LIBS) {

		# parse output of otool
		next unless /^\t(.*) \(.*\).*$/;
		my $FULLPATH = $1;

		# skip dependencies on things that are no .dylib
		next unless $FULLPATH =~ /\.dylib$/;

		# skip dependencies that are already dynamic
		next if $FULLPATH =~ /^@/;

		# skip dependencies that don't have an absolute path
		next unless $FULLPATH =~ /^(.*)\/(.*)$/;
		my $PATH = $1;
		my $FNAME = $2;

		# skip dependencies on anything in /usr/lib
		next if $PATH =~ q{^/usr/lib$};

		# mark dependency as to-be-copied
		push(@TOCOPY, $FULLPATH);

	}

}

# now patch all binaries we copied
while (my $EXECUTABLE = pop @TOPATCH) {

	next if $PATCHED{$EXECUTABLE};
	$PATCHED{$EXECUTABLE} = 1;

	print "patching ".$EXECUTABLE."\n";

	my @LIBS = qx{/usr/bin/otool -L $EXECUTABLE};
	while ($_ = pop @LIBS) {

		# parse output of otool
		next unless /^\t(.*) \(.*\).*$/;
		my $FULLPATH = $1;

		# skip dependencies that are not .dylibs
		next unless $FULLPATH =~ /\.dylib$/;

		# skip dependencies that are already dynamic
		next if $FULLPATH =~ /^@/;

		# skip dependencies without absolute paths
		next unless $FULLPATH =~ /^(.*)\/(.*)$/;
		my $PATH = $1;
		my $FNAME = $2;

		# skip dependencies on stuff in /usr/lib
		next if $PATH =~ q{^/usr/lib$};

		# patch binary to depend on copy in @executable_path instead
		system('install_name_tool -change '.$FULLPATH.' @executable_path/'.$FNAME.' '.$EXECUTABLE);

	}

}

