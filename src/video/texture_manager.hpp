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

#ifndef HEADER_SUPERTUX_VIDEO_TEXTURE_MANAGER_HPP
#define HEADER_SUPERTUX_VIDEO_TEXTURE_MANAGER_HPP

#include <config.h>
#include <unordered_map>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <optional>

#include "math/rect.hpp"
#include "util/currenton.hpp"
#include "video/sampler.hpp"
#include "video/sdl_surface_ptr.hpp"
#include "video/texture.hpp"
#include "video/texture_ptr.hpp"

class GLTexture;
class ReaderMapping;
struct SDL_Surface;

class TextureManager final : public Currenton<TextureManager>
{
  friend class Texture;

private:
  static const std::string s_dummy_texture;

public:
  TextureManager();
  ~TextureManager() override;

  TexturePtr get(const ReaderMapping& mapping, const std::optional<Rect>& region = std::nullopt);
  TexturePtr get(const std::string& filename);
  TexturePtr get(const std::string& filename,
                 const std::optional<Rect>& rect,
                 const Sampler& sampler = Sampler());
  TexturePtr create_dummy_texture() const;

  void reload();

  void debug_print(std::ostream& out) const;

  inline bool last_load_successful() const { return m_load_successful; }

private:
  const SDL_Surface& get_surface(const std::string& filename);
  void reap_cache_entry(const Texture::Key& key);

  /** on failure a dummy texture is returned and no exception is thrown */
  TexturePtr create_image_texture(const std::string& filename, const Sampler& sampler);

  TexturePtr create_image_texture(const std::string& filename, const Rect& rect, const Sampler& sampler);

  /** throw an exception on error */
  SDLSurfacePtr create_image_surface_raw(const std::string& filename, const Rect& rect, const Sampler& sampler);

  static SDLSurfacePtr create_dummy_surface();

private:
  std::map<Texture::Key, std::weak_ptr<Texture>> m_image_textures;
  std::unordered_map<std::string, SDLSurfacePtr> m_surfaces;
  bool m_load_successful;

private:
  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;
};

#endif

/* EOF */
