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

#include "audio/ogg_sound_file.hpp"

#include <config.h>

#include <assert.h>
#include <physfs.h>

OggSoundFile::OggSoundFile(PHYSFS_File* file_, double loop_begin_, double loop_at_) :
  m_file(file_),
  m_vorbis_file(),
  m_loop_begin(),
  m_loop_at()
{
  ov_callbacks callbacks = { cb_read, cb_seek, cb_close, cb_tell };
  ov_open_callbacks(m_file, &m_vorbis_file, nullptr, 0, callbacks);

  vorbis_info* vi = ov_info(&m_vorbis_file, -1);

  m_channels = vi->channels;
  m_rate = static_cast<int>(vi->rate);
  m_bits_per_sample = 16;
  m_size = static_cast<size_t> (ov_pcm_total(&m_vorbis_file, -1) * 2);

  double samples_begin = loop_begin_ * m_rate;
  double sample_loop   = loop_at_ * m_rate;

  m_loop_begin = static_cast<ogg_int64_t>(samples_begin);
  if (loop_begin_ < 0) {
    m_loop_at = static_cast<ogg_int64_t>(-1);
  } else {
    m_loop_at = static_cast<ogg_int64_t>(sample_loop);
  }
}

OggSoundFile::~OggSoundFile()
{
  ov_clear(&m_vorbis_file);
}

size_t
OggSoundFile::read(void* _buffer, size_t buffer_size)
{
  char*  buffer         = reinterpret_cast<char*> (_buffer);
  int    section        = 0;
  size_t totalBytesRead = 0;

  while (buffer_size>0) {
#ifdef WORDS_BIGENDIAN
    int bigendian = 1;
#else
    int bigendian = 0;
#endif

    size_t bytes_to_read    = buffer_size;
    if (m_loop_at > 0) {
      size_t      bytes_per_sample       = 2;
      ogg_int64_t time                   = ov_pcm_tell(&m_vorbis_file);
      ogg_int64_t samples_left_till_loop = m_loop_at - time;
      ogg_int64_t bytes_left_till_loop = samples_left_till_loop * bytes_per_sample;
      if (bytes_left_till_loop <= 4)
        break;

      if (bytes_left_till_loop < static_cast<ogg_int64_t>(bytes_to_read)) {
        bytes_to_read    = static_cast<size_t>(bytes_left_till_loop);
      }
    }

    long bytesRead
      = ov_read(&m_vorbis_file, buffer, static_cast<int>(bytes_to_read), bigendian,
                2, 1, &section);
    if (bytesRead == 0) {
      break;
    }
    buffer_size    -= bytesRead;
    buffer         += bytesRead;
    totalBytesRead += bytesRead;
  }

  return totalBytesRead;
}

void
OggSoundFile::reset()
{
  ov_pcm_seek(&m_vorbis_file, m_loop_begin);
}

size_t
OggSoundFile::cb_read(void* ptr, size_t size, size_t nmemb, void* source)
{
  auto file = reinterpret_cast<PHYSFS_file*> (source);

  PHYSFS_sint64 res
    = PHYSFS_readBytes(file, ptr, static_cast<PHYSFS_uint32> (size) * static_cast<PHYSFS_uint32> (nmemb));
  if (res <= 0)
    return 0;

  return static_cast<size_t> (res) / size;
}

int
OggSoundFile::cb_seek(void* source, ogg_int64_t offset, int whence)
{
  auto file = reinterpret_cast<PHYSFS_file*> (source);

  switch (whence) {
    case SEEK_SET:
      if (PHYSFS_seek(file, static_cast<PHYSFS_uint64> (offset)) == 0)
        return -1;
      break;
    case SEEK_CUR:
      if (PHYSFS_seek(file, PHYSFS_tell(file) + offset) == 0)
        return -1;
      break;
    case SEEK_END:
      if (PHYSFS_seek(file, PHYSFS_fileLength(file) + offset) == 0)
        return -1;
      break;
    default:
      assert(false);
      return -1;
  }
  return 0;
}

int
OggSoundFile::cb_close(void* source)
{
  auto file = reinterpret_cast<PHYSFS_file*> (source);
  PHYSFS_close(file);
  return 0;
}

long
OggSoundFile::cb_tell(void* source)
{
  auto file = reinterpret_cast<PHYSFS_file*> (source);
  return static_cast<long> (PHYSFS_tell(file));
}

/* EOF */
