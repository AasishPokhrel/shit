#!/usr/bin/perl

#
# $Id$
#
# SuperTux - Level conversion helper
# Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
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
# This conversion helper aids in the conversion of SuperTux level from
# 0.1.3 level format to the one used by SuperTux 0.2.0.
#
# Usage:
#   levelconverter-0.1.3_0.2.0.pl < oldformat.stl > newformat.stl
#
# Note that the script makes some unreasonable assumptions about where
# newlines occur in level files. It does work for most levels created
# in SuperTux 0.1.3 and Flexlay, though.
#

## Helper function: Extracts contents of ($token ...) leaf list (must not contain any other lists)
sub lispContents
{
	$token = shift();
	$haystack = shift();
	if ($haystack =~ m{\($token\s+(.*?)\s*\)}s) {
		return $1;
	}
	return;
}

# extract (supertux-level ...) list from stdin
$all = join("", <STDIN>);
if ($all !~ m{\(supertux-level\s+(.*)\s*\)}s) { die("Not a supertux level"); }
$level = $1;

# make sure we deal with a (version 1) level
$version = lispContents("version", $level) or die("no version tag found");
if ($version != "1") { die("not a version 1 level"); }

# extract various properties
$author = lispContents("author", $level) or $author = "Anonymous";
$name = lispContents("name", $level) or $name = "Unnamed";
$width = lispContents("width", $level) or die("no level width definition found");
$height = lispContents("height", $level) or $height = "15";
$start_pos_x = lispContents("start_pos_x", $level) or $start_pos_x = "100";
$start_pos_y = lispContents("start_pos_y", $level) or $start_pos_y = "170";
$interactive_tm = lispContents("interactive-tm", $level) or die("no interactive tilemap found");
$background_tm = lispContents("background-tm", $level) or die("no background tilemap found");
$foreground_tm = lispContents("foreground-tm", $level) or die("no foreground tilemap found");

# extract objects list
# kind of a hack: object list is assumed to terminate at the first closing parenthesis that is alone on a line
if ($level !~ m{\(objects\s+(.*?)\s*\n\s*\)}s) { die("Objects list not found"); }
$objects = $1;
$objects =~ s{money}{jumpy}sg;
$objects =~ s{\(stay-on-platform\s+#[tf]\s*\)}{}sg;

# write out version-2 level on stdout
print qq{(supertux-level\n};
print qq{  (version 2)\n};
print qq{  (name (_ $name))\n} if ($name);
print qq{  (author $author)\n} if ($author);
print qq{  (sector\n};
print qq{    (name "main")\n};

print qq{    (tilemap\n};
print qq{      (z-pos -100)\n};
print qq{      (solid #f)\n};
print qq{      (speed 1)\n};
print qq{      (width $width)\n};
print qq{      (height $height)\n};
print qq{      (tiles $background_tm)\n};
print qq{    )\n};

print qq{    (tilemap\n};
print qq{      (z-pos 0)\n};
print qq{      (solid #t)\n};
print qq{      (speed 1)\n};
print qq{      (width $width)\n};
print qq{      (height $height)\n};
print qq{      (tiles $interactive_tm)\n};
print qq{    )\n};

print qq{    (tilemap\n};
print qq{      (z-pos 100)\n};
print qq{      (solid #f)\n};
print qq{      (speed 1)\n};
print qq{      (width $width)\n};
print qq{      (height $height)\n};
print qq{      (tiles $foreground_tm)\n};
print qq{    )\n};

print qq{    (spawnpoint\n};
print qq{      (name "main")\n};
print qq{      (x $start_pos_x)\n};
print qq{      (y $start_pos_y)\n};
print qq{    )\n};

print qq{    $objects\n};

print qq{  )\n};

print qq{)\n};

# EOF

