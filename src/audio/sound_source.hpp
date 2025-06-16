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

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_SOURCE_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_SOURCE_HPP

#include "math/fwd.hpp"

/** A sound source represents the source of audio output. You can
    place sources at certain points in your world or set their
    velocity to produce doppler effects */
class SoundSource
{
public:
  SoundSource() {}
  virtual ~SoundSource() {}

  virtual void play() = 0;
  virtual void stop(bool unload_buffer = true) = 0;
  virtual void pause() = 0;
  virtual bool playing() const = 0;

  virtual void set_looping(bool looping) = 0;
  virtual void set_relative(bool relative) = 0;
  /// Set volume (0.0 is silent, 1.0 is normal)
  virtual void set_gain(float gain) = 0;
  virtual void set_pitch(float pitch) = 0;
  virtual void set_position(const Vector& position) = 0;
  virtual void set_velocity(const Vector& velocity) = 0;
  virtual void set_reference_distance(float distance) = 0;

private:
  SoundSource(const SoundSource&) = delete;
  SoundSource& operator=(const SoundSource&) = delete;
};

#endif

/* EOF */
