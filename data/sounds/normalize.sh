#!/bin/sh

set -e

OUTPUTDIR="6db-less"

mkdir "$OUTPUTDIR"

for i in *.wav; do
    cp -vi "$i" "${OUTPUTDIR}/${i}"
    normalize-audio -g -6db "${OUTPUTDIR}/${i}"
done

for i in *.ogg; do
    oggdec "$i" -o "${OUTPUTDIR}/${i%%.ogg}.wav"
    normalize-audio -g -6db "${OUTPUTDIR}/${i%%.ogg}.wav"
    oggenc "${OUTPUTDIR}/${i%%.ogg}.wav"
    rm "${OUTPUTDIR}/${i%%.ogg}.wav"
done

# EOF #
