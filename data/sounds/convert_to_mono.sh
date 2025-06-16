# Copyright (c) 2022 A. Semphris <semphris@protonmail.com>
# Released under Creative Commons Zero (CC0)
# https://creativecommons.org/publicdomain/zero/1.0/legalcode

# Sounds MUST be mono (1 channel only) for positional sounds to work with OpenAL
# Requires ffmpeg and ffprobe (the former almost always comes with the former)

for file in `ls`; do
  if [[ ! "$file" =~ .sh$ ]] && [ "$(ffprobe -i $file -show_streams -select_streams a:0 2>&1 | grep channels=1)" = "" ]; then
    ffmpeg -i $file -map_channel 0.0.0 temp.$file && mv temp.$file $file;
  fi
done
