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

#include "audio/sound_file.hpp"
#include "audio/sound_manager.hpp"
#include "audio/stream_sound_source.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"

StreamSoundSource::StreamSoundSource() :
  m_file(),
  m_fade_state(NoFading),
  m_fade_start_time(),
  m_fade_time(),
  m_looping(false)
{
  alGenBuffers(STREAMFRAGMENTS, m_buffers);
  try
  {
    SoundManager::check_al_error("Couldn't allocate audio buffers: ");
  }
  catch(std::exception& e)
  {
    log_warning << e.what() << std::endl;
  }
  //add me to update list
  SoundManager::current()->register_for_update( this );
}

StreamSoundSource::~StreamSoundSource()
{
  //don't update me any longer
  SoundManager::current()->remove_from_update( this );
  m_file.reset();
  stop();
  alDeleteBuffers(STREAMFRAGMENTS, m_buffers);
  try
  {
    SoundManager::check_al_error("Couldn't delete audio buffers: ");
  }
  catch(std::exception& e)
  {
    // Am I bovvered?
    log_warning << e.what() << std::endl;
  }
}

void
StreamSoundSource::set_sound_file(std::unique_ptr<SoundFile> newfile)
{
  m_file = std::move(newfile);

  ALint queued;
  alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
  for (size_t i = 0; i < STREAMFRAGMENTS - queued; ++i) {
    if (fillBufferAndQueue(m_buffers[i]) == false)
      break;
  }
}

void
StreamSoundSource::resume()
{
  OpenALSoundSource::resume();
  set_gain(1.0);
  m_fade_state = NoFading;
}

void
StreamSoundSource::update()
{
  ALint processed = 0;
  alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
  for (ALint i = 0; i < processed; ++i) {
    ALuint buffer;
    alSourceUnqueueBuffers(m_source, 1, &buffer);
    try
    {
      SoundManager::check_al_error("Couldn't unqueue audio buffer: ");
    }
    catch(std::exception& e)
    {
      log_warning << e.what() << std::endl;
    }

    if (fillBufferAndQueue(buffer) == false)
      break;
  }

  if (!playing() && !paused()) {
    if (processed == 0 || !m_looping)
      return;

    // we might have to restart the source if we had a buffer underrun
    log_info << "Restarting audio source because of buffer underrun" << std::endl;
    play();
  }

  if (m_fade_state == FadingOn || m_fade_state == FadingResume) {
    float time = g_real_time - m_fade_start_time;
    if (time >= m_fade_time) {
      set_gain(1.0);
      m_fade_state = NoFading;
    } else {
      set_gain(time / m_fade_time);
    }
  } else if (m_fade_state == FadingOff || m_fade_state == FadingPause) {
    float time = g_real_time - m_fade_start_time;
    if (time >= m_fade_time) {
      if (m_fade_state == FadingOff)
        stop();
      else
        pause();
      m_fade_state = NoFading;
    } else {
      set_gain( (m_fade_time - time) / m_fade_time);
    }
  }
}

void
StreamSoundSource::set_fading(FadeState state, float fade_time_)
{
  m_fade_state = state;
  m_fade_time = fade_time_;
  m_fade_start_time = g_real_time;
}

bool
StreamSoundSource::fillBufferAndQueue(ALuint buffer)
{
  // fill buffer
  std::unique_ptr<char[]> bufferdata(new char[STREAMFRAGMENTSIZE]);
  size_t bytesread = 0;
  do {
    bytesread += m_file->read(static_cast<char *>(bufferdata.get()) + bytesread,
      STREAMFRAGMENTSIZE - bytesread);
    // end of sound file
    if (bytesread < STREAMFRAGMENTSIZE) {
      if (m_looping)
        m_file->reset();
      else
        break;
    }
  } while(bytesread < STREAMFRAGMENTSIZE);

  if (bytesread > 0) {
    ALenum format = SoundManager::get_sample_format(*m_file);
    try
    {
      alBufferData(buffer, format, bufferdata.get(), static_cast<ALsizei>(bytesread), m_file->m_rate);
      SoundManager::check_al_error("Couldn't refill audio buffer: ");

      alSourceQueueBuffers(m_source, 1, &buffer);
      SoundManager::check_al_error("Couldn't queue audio buffer: ");
    }
    catch(std::exception& e)
    {
      log_warning << e.what() << std::endl;
    }
  }

  // return false if there aren't more buffers to fill
  return bytesread >= STREAMFRAGMENTSIZE;
}

/* EOF */
