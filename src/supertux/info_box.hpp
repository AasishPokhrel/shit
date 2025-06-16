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

#ifndef HEADER_SUPERTUX_SUPERTUX_INFO_BOX_HPP
#define HEADER_SUPERTUX_SUPERTUX_INFO_BOX_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "video/surface_ptr.hpp"

class DrawingContext;
class InfoBoxLine;

/** This class is displaying a box with information text inside the game */
class InfoBox final
{
public:
  InfoBox(const std::string& text);

  void draw(DrawingContext& context);
  void scrolldown();
  void scrollup();
  void pagedown();
  void pageup();

private:
  size_t firstline;
  std::vector<std::unique_ptr<InfoBoxLine> > lines;
  std::map<std::string, Surface*> images;
  SurfacePtr arrow_scrollup;
  SurfacePtr arrow_scrolldown;

private:
  InfoBox(const InfoBox&) = delete;
  InfoBox& operator=(const InfoBox&) = delete;
};

#endif

/* EOF */
