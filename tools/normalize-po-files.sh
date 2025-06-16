#!/bin/sh
find -iname '*.po' -execdir msgmerge --backup=off --no-fuzzy-matching --no-wrap -U '{}' messages.pot \;
