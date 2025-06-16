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

#ifndef HEADER_SUPERTUX_AUDIO_SOUND_ERROR_HPP
#define HEADER_SUPERTUX_AUDIO_SOUND_ERROR_HPP

#include <stdexcept>
#include <string>

class SoundError final : public std::exception
{
public:
  SoundError(const std::string& message) throw();
  SoundError(const SoundError&) = default;
  SoundError& operator=(const SoundError&) = default;
  ~SoundError() throw() override;

  virtual const char* what() const throw() override;

private:
  std::string m_message;
};

#endif

/* EOF */
