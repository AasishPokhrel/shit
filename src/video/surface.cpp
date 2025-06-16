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

#include "video/surface.hpp"

#include <sstream>

#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/string_util.hpp"
#include "video/texture.hpp"
#include "video/texture_manager.hpp"
#include "video/video_system.hpp"

SurfacePtr
Surface::from_reader(const ReaderMapping& mapping, const std::optional<Rect>& rect, const std::string& filename)
{
  TexturePtr diffuse_texture;
  std::optional<ReaderMapping> diffuse_texture_mapping;
  if (mapping.get("diffuse-texture", diffuse_texture_mapping))
  {
    diffuse_texture = TextureManager::current()->get(*diffuse_texture_mapping, rect);
  }

  TexturePtr displacement_texture;
  std::optional<ReaderMapping> displacement_texture_mapping;
  if (mapping.get("displacement-texture", displacement_texture_mapping))
  {
    displacement_texture = TextureManager::current()->get(*displacement_texture_mapping, rect);
  }

  Flip flip = NO_FLIP;
  std::vector<bool> flip_v;
  if (mapping.get("flip", flip_v))
  {
    flip ^= flip_v[0] ? HORIZONTAL_FLIP : NO_FLIP;
    flip ^= flip_v[1] ? VERTICAL_FLIP : NO_FLIP;
  }

  auto surface = new Surface(diffuse_texture, displacement_texture, flip, filename);
  return SurfacePtr(surface);
}

SurfacePtr
Surface::from_file(const std::string& filename, const std::optional<Rect>& rect)
{
  if (StringUtil::has_suffix(filename, ".surface"))
  {
    ReaderDocument doc = ReaderDocument::from_file(filename);
    ReaderObject object = doc.get_root();
    if (object.get_name() != "supertux-surface")
    {
      std::ostringstream msg;
      msg << filename << ": error: not a 'supertux-surface' file";
      throw std::runtime_error(msg.str());
    }
    else
    {
      return Surface::from_reader(object.get_mapping(), rect, filename);
    }
  }
  else
  {
    auto textureManager = TextureManager::current();
    TexturePtr texture = rect ?
      textureManager->get(filename, *rect) : textureManager->get(filename);

    return SurfacePtr(new Surface(texture, TexturePtr(), NO_FLIP, filename));
  }
}

Surface::Surface(const TexturePtr& diffuse_texture,
                 const TexturePtr& displacement_texture,
                 Flip flip, const std::string& filename) :
  m_diffuse_texture(diffuse_texture),
  m_displacement_texture(displacement_texture),
  m_region(0, 0, m_diffuse_texture->get_image_width(), m_diffuse_texture->get_image_height()),
  m_flip(flip),
  m_source_filename(filename)
{
}

Surface::Surface(const TexturePtr& diffuse_texture,
                 const TexturePtr& displacement_texture,
                 const Rect& region,
                 Flip flip, const std::string& filename) :
  m_diffuse_texture(diffuse_texture),
  m_displacement_texture(displacement_texture),
  m_region(region),
  m_flip(flip),
  m_source_filename(filename)
{
}

SurfacePtr
Surface::from_texture(const TexturePtr& texture)
{
  return SurfacePtr(new Surface(texture, TexturePtr(), NO_FLIP));
}

Surface::~Surface()
{
}

SurfacePtr
Surface::clone(Flip flip) const
{
  SurfacePtr surface(new Surface(m_diffuse_texture,
                                 m_displacement_texture,
                                 m_region,
                                 m_flip ^ flip));
  return surface;
}

SurfacePtr
Surface::region(const Rect& rect) const
{
  SurfacePtr surface(new Surface(m_diffuse_texture,
                                 m_displacement_texture,
                                 rect,
                                 m_flip));
  return surface;
}

TexturePtr
Surface::get_texture() const
{
  return m_diffuse_texture;
}

TexturePtr
Surface::get_displacement_texture() const
{
  return m_displacement_texture;
}

int
Surface::get_width() const
{
  return m_region.get_width();
}

int
Surface::get_height() const
{
  return m_region.get_height();
}

/* EOF */
