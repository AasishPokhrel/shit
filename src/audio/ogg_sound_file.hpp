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

#ifndef HEADER_SUPERTUX_AUDIO_OGG_SOUND_FILE_HPP
#define HEADER_SUPERTUX_AUDIO_OGG_SOUND_FILE_HPP

#include <vorbis/vorbisfile.h>

#include "audio/sound_file.hpp"

struct PHYSFS_File;

class OggSoundFile final : public SoundFile
{
private:
  static size_t cb_read(void* ptr, size_t size, size_t nmemb, void* source);
  static int cb_seek(void* source, ogg_int64_t offset, int whence);
  static int cb_close(void* source);
  static long cb_tell(void* source);

public:
  OggSoundFile(PHYSFS_File* file, double loop_begin, double loop_at);
  ~OggSoundFile() override;

  virtual size_t read(void* buffer, size_t buffer_size) override;
  virtual void reset() override;

private:
  PHYSFS_File* m_file;
  OggVorbis_File m_vorbis_file;
  ogg_int64_t m_loop_begin;
  ogg_int64_t m_loop_at;

private:
  OggSoundFile(const OggSoundFile&) = delete;
  OggSoundFile& operator=(const OggSoundFile&) = delete;
};


#endif

/* EOF */
