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

#include "audio/openal_sound_source.hpp"

#include "audio/sound_manager.hpp"
#include "util/log.hpp"

OpenALSoundSource::OpenALSoundSource() :
  m_source(),
  m_gain(1.0f),
  m_volume(1.0f)
{
  alGenSources(1, &m_source);

  // Don't catch anything here: force the caller to catch the error, so that
  // the caller won't handle an object in an invalid state thinking it's clean
  SoundManager::check_al_error("Couldn't create audio source: ");

  set_reference_distance(128);
}

OpenALSoundSource::~OpenALSoundSource()
{
  stop();
  alDeleteSources(1, &m_source);
}

void
OpenALSoundSource::stop(bool unload_buffer)
{
#ifdef WIN32
  // See commit 417a8e7a8c599bfc2dceaec7b6f64ac865318ef1
  alSourceRewindv(1, &m_source); // Stops the source
#else
  alSourceStop(m_source);
#endif
  if (unload_buffer)
    alSourcei(m_source, AL_BUFFER, AL_NONE);
  try
  {
    SoundManager::check_al_error("Problem stopping audio source: ");
  }
  catch(const std::exception& e)
  {
    // Internal OpenAL error. Don't you crash on me, baby!
    log_warning << e.what() << std::endl;
  }
}

void
OpenALSoundSource::pause()
{
  alSourcePause(m_source);
  try
  {
    SoundManager::check_al_error("Couldn't pause audio source: ");
  }
  catch(const std::exception& e)
  {
    log_warning << e.what() << std::endl;
  }
}

void
OpenALSoundSource::play()
{
  alSourcePlay(m_source);

  try
  {
    SoundManager::check_al_error("Couldn't start audio source: ");
  }
  catch(const std::exception& e)
  {
    // We probably have too many sources playing simultaneously.
    log_warning << e.what() << std::endl;
  }
}

bool
OpenALSoundSource::playing() const
{
  ALint state = AL_PLAYING;
  alGetSourcei(m_source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}

void
OpenALSoundSource::resume()
{
  if ( !paused() )
  {
    return;
  }

  play();
}

bool
OpenALSoundSource::paused() const
{
    ALint state = AL_PAUSED;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}

void
OpenALSoundSource::update()
{
}

void
OpenALSoundSource::set_looping(bool looping)
{
  alSourcei(m_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

void
OpenALSoundSource::set_relative(bool relative)
{
  alSourcei(m_source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

void
OpenALSoundSource::set_position(const Vector& position)
{
  alSource3f(m_source, AL_POSITION, position.x, position.y, 0);
}

void
OpenALSoundSource::set_velocity(const Vector& velocity)
{
  alSource3f(m_source, AL_VELOCITY, velocity.x, velocity.y, 0);
}

void
OpenALSoundSource::set_gain(float gain)
{
  alSourcef(m_source, AL_GAIN, gain * m_volume);
  m_gain = gain;
}

void
OpenALSoundSource::set_pitch(float pitch)
{
  alSourcef(m_source, AL_PITCH, pitch);
}

void
OpenALSoundSource::set_reference_distance(float distance)
{
  alSourcef(m_source, AL_REFERENCE_DISTANCE, distance);
}

void
OpenALSoundSource::set_volume(float volume)
{
  m_volume = volume;
  alSourcef(m_source, AL_GAIN, m_gain * m_volume);
}

/* EOF */
