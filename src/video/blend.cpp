//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include <string>

#include "util/log.hpp"
#include "video/blend.hpp"

Blend
Blend_from_string(const std::string& text)
{
  if (text == "none")
  {
    return Blend::NONE;
  }
  else if (text == "blend")
  {
    return Blend::BLEND;
  }
  else if (text == "add")
  {
    return Blend::ADD;
  }
  else if (text == "mod")
  {
    return Blend::MOD;
  }
  else
  {
    log_warning << "Unknown blend mode: '" << text << "'" << std::endl;
    return Blend::BLEND;
  }
}

std::string
Blend_to_string(Blend blend)
{
  if (blend == Blend::NONE)
  {
    return "none";
  }
  else if (blend == Blend::BLEND)
  {
    return "blend";
  }
  else if (blend == Blend::ADD)
  {
    return "add";
  }
  else if (blend == Blend::MOD)
  {
    return "mod";
  }
  else
  {
    log_warning << "Unknown blend mode: " << static_cast<int>(blend) << std::endl;
    return "blend";
  }
}

/* EOF */
