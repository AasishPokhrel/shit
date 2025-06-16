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

#ifndef HEADER_SUPERTUX_VIDEO_SDLBASE_VIDEO_SYSTEM_HPP
#define HEADER_SUPERTUX_VIDEO_SDLBASE_VIDEO_SYSTEM_HPP

#include <SDL.h>

#include "video/video_system.hpp"

class SDLBaseVideoSystem : public VideoSystem
{
private:
public:
  SDLBaseVideoSystem();
  ~SDLBaseVideoSystem() override;

  virtual void set_title(const std::string& title) override;
  virtual void set_icon(const SDL_Surface& icon) override;

  virtual Size get_window_size() const override;
  virtual void on_resize(int w, int h) override;

protected:
  void create_sdl_window(Uint32 flags);
  void apply_video_mode();

protected:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_sdl_window;
  Size m_desktop_size;

private:
  SDLBaseVideoSystem(const SDLBaseVideoSystem&) = delete;
  SDLBaseVideoSystem& operator=(const SDLBaseVideoSystem&) = delete;
};

#endif

/* EOF */
