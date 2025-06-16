//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_AUDIO_WAV_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_WAV_SOUND_FILE_HPP

#include <physfs.h>

#include "audio/sound_file.hpp"

class WavSoundFile final : public SoundFile
{
public:
  WavSoundFile(PHYSFS_file* file);
  ~WavSoundFile() override;

  virtual size_t read(void* buffer, size_t buffer_size) override;
  virtual void reset() override;

private:
  PHYSFS_file* m_file;
  PHYSFS_sint64 m_datastart;

private:
  WavSoundFile(const WavSoundFile&) = delete;
  WavSoundFile& operator=(const WavSoundFile&) = delete;
};

#endif

/* EOF */
