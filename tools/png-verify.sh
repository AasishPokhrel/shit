#!/bin/bash

# Try to open .png files and display libpng warning if any.

if [ "$#" -eq 0 ]; then
    find ../addons-src/ data/ -iname "*.png" -print0 | while read -d $'\0' i; do echo "$(pngtopnm "$i" 2>&1 > /dev/null )" | sed "s#^#${i}: #"; done | grep -v ': $'
else
  for i in "$@"; do
    pngtopnm "$i" 2>&1 > /dev/null | sed "s#^#${i}: #" | grep -v ': $'
  done
fi

# EOF #
