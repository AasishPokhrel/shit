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

#ifndef HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_OPENAL_SOUND_SOURCE_HPP

#include <al.h>

#include "audio/sound_source.hpp"

class OpenALSoundSource : public SoundSource
{
  friend class SoundManager;

public:
  OpenALSoundSource();
  ~OpenALSoundSource() override;

  virtual void play() override;
  virtual void stop(bool unload_buffer = true) override;
  virtual void pause() override;
  virtual bool playing() const override;

  virtual void set_looping(bool looping) override;
  virtual void set_relative(bool relative) override;
  virtual void set_gain(float gain) override;
  virtual void set_pitch(float pitch) override;
  virtual void set_position(const Vector& position) override;
  virtual void set_velocity(const Vector& position) override;
  virtual void set_reference_distance(float distance) override;

  virtual void set_volume(float volume);

  virtual bool paused() const;
  virtual void resume();
  virtual void update();

protected:
  ALuint m_source;
  float m_gain;
  float m_volume;

private:
  OpenALSoundSource(const OpenALSoundSource&) = delete;
  OpenALSoundSource& operator=(const OpenALSoundSource&) = delete;
};

#endif

/* EOF */
