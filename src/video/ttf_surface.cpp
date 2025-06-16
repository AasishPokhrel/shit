//  SuperTux
//  Copyright (C) 2016 Ingo Ruhnke <grumbel@gmail.com>
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

#include "video/ttf_surface.hpp"

#include <SDL_ttf.h>

#include <sstream>

#include "util/log.hpp"
#include "video/sdl_surface.hpp"
#include "video/surface.hpp"
#include "video/ttf_font.hpp"
#include "video/ttf_surface_manager.hpp"
#include "video/video_system.hpp"

TTFSurfacePtr
TTFSurface::create(const TTFFont& font, const std::string& text)
{
  SDLSurfacePtr text_surface(TTF_RenderUTF8_Blended(font.get_ttf_font(),
                                                    text.c_str(),
                                                    SDL_Color{255, 255, 255, 255}));
  if (!text_surface)
  {
    log_warning << "Couldn't render text '" << text << "' :" << SDL_GetError();
    return std::make_shared<TTFSurface>(SurfacePtr(), Vector(0.0f, 0.0f));
  }

  // FIXME: handle shadow offset
  int grow = std::max(font.get_border() * 2, font.get_shadow_size() * 2);

  SDLSurfacePtr target = SDLSurface::create_rgba(text_surface->w + grow, text_surface->h + grow);

#if !SDL_VERSION_ATLEAST(2,0,5)
  // Perform blitting in ARGB8888, instead of RGBA8888, to avoid bug in older SDL2.
  // https://bugzilla.libsdl.org/show_bug.cgi?id=3159
  target.reset(SDL_ConvertSurfaceFormat(target.get(), SDL_PIXELFORMAT_ARGB8888, 0));
#endif

  { // shadow
    SDL_SetSurfaceAlphaMod(text_surface.get(), 192);
    SDL_SetSurfaceColorMod(text_surface.get(), 0, 0, 0);
    SDL_SetSurfaceBlendMode(text_surface.get(), SDL_BLENDMODE_BLEND);

    using P = std::tuple<int, int>;
    const std::initializer_list<std::tuple<int, int> > positions[] = {
      {},
      {P{0, 0}},
      {P{-1, 0}, P{1, 0}, P{0, -1}, P{0, 1}},
      {P{-2, 0}, P{2, 0}, P{0, -2}, P{0, 2},
       P{-1, -1}, P{1, -1}, P{-1, 1}, P{1, 1}}
    };

    int shadow_size = std::min(2, font.get_shadow_size());
    for (const auto& p : positions[shadow_size])
    {
      SDL_Rect dstrect{std::get<0>(p) + 2, std::get<1>(p) + 2, text_surface->w, text_surface->h};
      SDL_BlitSurface(text_surface.get(), nullptr,
                      target.get(), &dstrect);
    }
  }

  { // outline
    SDL_SetSurfaceAlphaMod(text_surface.get(), 255);
    SDL_SetSurfaceColorMod(text_surface.get(), 0, 0, 0);
    SDL_SetSurfaceBlendMode(text_surface.get(), SDL_BLENDMODE_BLEND);

    using P = std::tuple<int, int>;
    const std::initializer_list<std::tuple<int, int> > positions[] = {
      {},
      {P{-1, 0}, P{1, 0}, P{0, -1}, P{0, 1}},
      {P{-2, 0}, P{2, 0}, P{0, -2}, P{0, 2},
       P{-1, -1}, P{1, -1}, P{-1, 1}, P{1, 1}}
    };

    int border = std::min(2, font.get_border());
    for (const auto& p : positions[border])
    {
      SDL_Rect dstrect{std::get<0>(p), std::get<1>(p), text_surface->w, text_surface->h};
      SDL_BlitSurface(text_surface.get(), nullptr,
                      target.get(), &dstrect);
    }
  }

  { // white core
    SDL_SetSurfaceAlphaMod(text_surface.get(), 255);
    SDL_SetSurfaceColorMod(text_surface.get(), 255, 255, 255);
    SDL_SetSurfaceBlendMode(text_surface.get(), SDL_BLENDMODE_BLEND);

    SDL_Rect dstrect{0, 0, text_surface->w, text_surface->h};

    SDL_BlitSurface(text_surface.get(), nullptr, target.get(), &dstrect);
  }

#if !SDL_VERSION_ATLEAST(2,0,5)
  target.reset(SDL_ConvertSurfaceFormat(target.get(), SDL_PIXELFORMAT_RGBA8888, 0));
#endif

  SurfacePtr result = Surface::from_texture(VideoSystem::current()->new_texture(*target));
  return std::make_shared<TTFSurface>(result, Vector(0, 0));
}

TTFSurface::TTFSurface(const SurfacePtr& surface, const Vector& offset) :
  m_surface(surface),
  m_offset(offset)
{
}

int
TTFSurface::get_width() const
{
  if (m_surface) {
    return m_surface->get_width();
  } else {
    return 0;
  }
}

int
TTFSurface::get_height() const
{
  if (m_surface) {
    return m_surface->get_height();
  } else {
    return 0;
  }
}

/* EOF */
