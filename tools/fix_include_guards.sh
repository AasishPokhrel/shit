#!/bin/sh

for i in "$@"; do
    NAME=$(echo $i | sed "s/[\/\.]/_/g;s/^/SUPERTUX_/" | tr [a-z] [A-Z])
    echo $i $NAME
    sed  -i "s/^#ifndef HEADER_.*/#ifndef HEADER_$NAME/" $i;
    sed  -i "s/^#define HEADER_.*/#define HEADER_$NAME/" $i;
done

# EOF #
