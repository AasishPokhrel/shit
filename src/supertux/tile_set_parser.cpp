//  SuperTux
//  Copyright (C) 2008 Matthias Braun <matze@braunis.de>
//                     Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/tile_set_parser.hpp"

#include <sstream>
#include <sexp/value.hpp>
#include <sexp/io.hpp>

#include "supertux/autotile_parser.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/tile_set.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/file_system.hpp"
#include "video/surface.hpp"

TileSetParser::TileSetParser(TileSet& tileset, const std::string& filename,
                             int32_t start, int32_t end, int32_t offset) :
  m_tileset(tileset),
  m_filename(filename),
  m_tiles_path(),
  m_start(start),
  m_end(end),
  m_offset(offset)
{
}

void
TileSetParser::parse(bool imported)
{
  if (m_offset && m_start + m_offset < 1)
  {
    m_start = -m_offset + 1;
    log_warning << "The defined offset would assign non-positive IDs to tiles, tiles below " << -m_offset + 1 << " will be ignored." << std::endl;
  }
  if (m_end < 0)
  {
    log_warning << "Cannot import tiles with negative IDs." << std::endl;
    return;
  }
  if (m_start < 0)
  {
    log_warning << "Cannot import tiles with negative IDs. Importing will start at ID 1." << std::endl;
    m_start = 1;
  }

  m_tiles_path = FileSystem::dirname(m_filename);

  auto doc = ReaderDocument::from_file(m_filename);
  auto root = doc.get_root();

  if (root.get_name() != "supertux-tiles") {
    throw std::runtime_error("file is not a supertux tiles file.");
  }

  auto iter = root.get_mapping().get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "tile")
    {
      parse_tile(iter.as_mapping());
    }
    else if (iter.get_key() == "tilegroup")
    {
      /* tilegroups are only interesting for the editor */
      ReaderMapping reader = iter.as_mapping();
      Tilegroup tilegroup;
      reader.get("name", tilegroup.name);
      reader.get("tiles", tilegroup.tiles);

      // Allow offsetting every tile ID, specified in the tilegroup
      int32_t tiles_offset = 0;
      reader.get("offset", tiles_offset);

      bool has_valid_tile = false;
      for (int& tile : tilegroup.tiles)
      {
        if (tile == 0) continue;

        if (tile < m_start || (m_end && tile > m_end))
        {
          tile = 0;
        }
        else
        {
          tile += m_offset + tiles_offset;
          has_valid_tile = true;
        }
      }

      if (has_valid_tile)
      {
        m_tileset.add_tilegroup(std::move(tilegroup));
      }
    }
    else if (iter.get_key() == "tiles")
    {
      parse_tiles(iter.as_mapping());
    }
    else if (iter.get_key() == "autotileset")
    {
      ReaderMapping reader = iter.as_mapping();
      std::string autotile_filename;
      if (!reader.get("source", autotile_filename))
      {
        log_warning << "No source path for autotiles in file '" << m_filename << "'" << std::endl;
        continue;
      }

      int32_t import_offset = 0;
      reader.get("offset", import_offset);

      AutotileParser parser(m_tileset.m_autotilesets,
          FileSystem::normalize(m_tiles_path + autotile_filename),
          m_start, m_end, import_offset + m_offset);
      parser.parse();
    }
    else if (iter.get_key() == "import-tileset")
    {
      ReaderMapping reader = iter.as_mapping();
      std::string import_filename;
      if (!reader.get("file", import_filename))
      {
        log_warning << "No source path for imported tileset in file '" << m_filename << "'" << std::endl;
        continue;
      }
      if (import_filename == m_filename)
      {
        log_warning << "Tried to recursively import tileset '" << m_filename << "'" << std::endl;
        continue;
      }

      int32_t import_start = 0, import_end = 0, import_offset = 0;
      reader.get("start", import_start);
      reader.get("end", import_end);
      reader.get("offset", import_offset);

      import_offset += m_offset;

      if (import_start + import_offset < m_start)
      {
        import_start = std::max(0, m_start - import_offset);
      }
      if (m_end && (!import_end || import_end + import_offset > m_end))
      {
        import_end = m_end - import_offset;
      }
      if (import_end < import_start)
      {
        if (!imported) log_warning << "The defined range has a negative size, no tiles will be imported." << std::endl;
        continue;
      }

      TileSetParser parser(m_tileset, import_filename,
            import_start, import_end, import_offset);
      parser.parse(true);
    }
    else if (iter.get_key() == "additional")
    {
      ReaderMapping reader = iter.as_mapping();
      auto additional_iter = reader.get_iter();
      while (additional_iter.next())
      {
        if (additional_iter.get_key() == "thunderstorm") // Additional attributes for thunderstorms.
        {
          auto info_mapping = additional_iter.as_mapping();
          // Additional attributes for changing tiles for thunderstorms.
          std::vector<uint32_t> tiles;
          if (info_mapping.get("changing-tiles", tiles))
          {
            if (tiles.size() < 2)
            {
              log_warning << "Less than 2 tile IDs given for thunderstorm changing tiles." << std::endl;
              continue;
            }
            if (tiles.size() % 2 != 0) tiles.pop_back(); // If the number of tiles isn't even, remove last tile.
            for (int i = 0; i < static_cast<int>(tiles.size()); i += 2)
            {
              if (tiles[i] < static_cast<uint32_t>(m_start) || (m_end && tiles[i] > static_cast<uint32_t>(m_end)) ||
                  tiles[i + 1] < static_cast<uint32_t>(m_start) || (m_end && tiles[i + 1] > static_cast<uint32_t>(m_end)))
                continue; // Both tiles have to fit in the tile range.

              m_tileset.m_thunderstorm_tiles.insert({ tiles[i] + m_offset, tiles[i + 1] + m_offset });
            }
          }
        }
        else
        {
          log_warning << "Unknown symbol '" << additional_iter.get_key() << "' in \"additional\" section of tileset file" << std::endl;
        }
      }
    }
    else
    {
      log_warning << "Unknown symbol '" << iter.get_key() << "' in tileset file" << std::endl;
    }
  }
  /* Check for and remove any deprecated tiles from tilegroups */
  m_tileset.remove_deprecated_tiles();
  /* only create the unassigned tilegroup from the parent strf */
  if (g_config->developer_mode && !imported)
  {
    m_tileset.add_unassigned_tilegroup();
  }
}

void
TileSetParser::parse_tile(const ReaderMapping& reader)
{
  uint32_t id;
  if (!reader.get("id", id))
  {
    throw std::runtime_error("Missing tile-id.");
  }
  if (id < static_cast<uint32_t>(m_start) || (m_end && id > static_cast<uint32_t>(m_end))) return;
  id += m_offset;

  uint32_t attributes = 0;

  bool value = false;
  if (reader.get("solid", value) && value)
    attributes |= Tile::SOLID;
  if (reader.get("unisolid", value) && value)
    attributes |= Tile::UNISOLID | Tile::SOLID;
  if (reader.get("brick", value) && value)
    attributes |= Tile::BRICK;
  if (reader.get("ice", value) && value)
    attributes |= Tile::ICE;
  if (reader.get("water", value) && value)
    attributes |= Tile::WATER;
  if (reader.get("hurts", value) && value)
    attributes |= Tile::HURTS;
  if (reader.get("fire", value) && value)
    attributes |= Tile::FIRE;
  if (reader.get("walljump", value) && value)
    attributes |= Tile::WALLJUMP;
  if (reader.get("fullbox", value) && value)
    attributes |= Tile::FULLBOX;
  if (reader.get("coin", value) && value)
    attributes |= Tile::COIN;
  if (reader.get("goal", value) && value)
    attributes |= Tile::GOAL;

  uint32_t data = 0;

  if (reader.get("north", value) && value)
    data |= Tile::WORLDMAP_NORTH;
  if (reader.get("south", value) && value)
    data |= Tile::WORLDMAP_SOUTH;
  if (reader.get("west", value) && value)
    data |= Tile::WORLDMAP_WEST;
  if (reader.get("east", value) && value)
    data |= Tile::WORLDMAP_EAST;
  if (reader.get("stop", value) && value)
    data |= Tile::WORLDMAP_STOP;

  reader.get("data", data);

  float fps = 10;
  reader.get("fps", fps);

  std::string object_name, object_data;
  reader.get("object-name", object_name);
  reader.get("object-data", object_data);

  if (reader.get("slope-type", data))
  {
    attributes |= Tile::SOLID | Tile::SLOPE;
  }

  std::vector<SurfacePtr> editor_surfaces;
  std::optional<ReaderMapping> editor_images_mapping;
  if (reader.get("editor-images", editor_images_mapping)) {
    editor_surfaces = parse_imagespecs(*editor_images_mapping);
  }

  std::vector<SurfacePtr> surfaces;
  std::optional<ReaderMapping> images_mapping;
  if (reader.get("images", images_mapping)) {
    surfaces = parse_imagespecs(*images_mapping);
  }

  bool deprecated = false;
  reader.get("deprecated", deprecated);

  auto tile = std::make_unique<Tile>(surfaces, editor_surfaces,
                                     attributes, data, fps,
                                     deprecated, object_name, object_data);
  m_tileset.add_tile(id, std::move(tile));
}

void
TileSetParser::parse_tiles(const ReaderMapping& reader)
{
  // List of ids (use 0 if the tile should be ignored)
  std::vector<uint32_t> ids;
  // List of attributes of the tile
  std::vector<uint32_t> attributes;
  // List of data for the tiles
  std::vector<uint32_t> datas;

  // width and height of the image in tile units, this is used for two
  // purposes:
  //  a) so we don't have to load the image here to know its dimensions
  //  b) so that the resulting 'tiles' entry is more robust,
  //  ie. enlarging the image won't break the tile id mapping
  // FIXME: height is actually not used, since width might be enough for
  // all purposes, still feels somewhat more natural this way
  unsigned int width  = 0;
  unsigned int height = 0;

  bool has_ids = reader.get("ids",        ids);
  bool has_attributes = reader.get("attributes", attributes);
  bool has_datas = reader.get("datas", datas);

  reader.get("width", width);
  reader.get("height", height);

  bool shared_surface = false;
  reader.get("shared-surface", shared_surface);

  float fps = 10;
  reader.get("fps",     fps);

  // Allow specifying additional offset to tiles
  int32_t tiles_offset = 0;
  reader.get("offset", tiles_offset);

  bool deprecated = false;
  reader.get("deprecated", deprecated);

  if (ids.empty() || !has_ids)
  {
    throw std::runtime_error("No IDs specified.");
  }
  if (width == 0)
  {
    throw std::runtime_error("Width is zero.");
  }
  else if (height == 0)
  {
    throw std::runtime_error("Height is zero.");
  }
  else if (fps < 0)
  {
    throw std::runtime_error("Negative fps.");
  }
  else if (ids.size() != width*height)
  {
    std::ostringstream err;
    err << "Number of ids (" << ids.size() <<  ") and "
      "dimensions of image (" << width << "x" << height << " = " << width*height << ") "
      "differ";
    throw std::runtime_error(err.str());
  }
  else if (has_attributes && (ids.size() != attributes.size()))
  {
    std::ostringstream err;
    err << "Number of ids (" << ids.size() <<  ") and attributes (" << attributes.size()
        << ") mismatch, but must be equal";
    throw std::runtime_error(err.str());
  }
  else if (has_datas && ids.size() != datas.size())
  {
    std::ostringstream err;
    err << "Number of ids (" << ids.size() <<  ") and datas (" << datas.size()
        << ") mismatch, but must be equal";
    throw std::runtime_error(err.str());
  }
  else
  {
    if (shared_surface)
    {
      std::vector<SurfacePtr> editor_surfaces;
      std::optional<ReaderMapping> editor_surfaces_mapping;
      if (reader.get("editor-images", editor_surfaces_mapping)) {
        editor_surfaces = parse_imagespecs(*editor_surfaces_mapping);
      }

      std::vector<SurfacePtr> surfaces;
      std::optional<ReaderMapping> surfaces_mapping;
      if (reader.get("image", surfaces_mapping) ||
         reader.get("images", surfaces_mapping)) {
        surfaces = parse_imagespecs(*surfaces_mapping);
      }

      for (size_t i = 0; i < ids.size(); ++i)
      {
        if (!ids[i] || (ids[i] < static_cast<uint32_t>(m_start) || (m_end && ids[i] > static_cast<uint32_t>(m_end)))) continue;
        ids[i] += m_offset + tiles_offset;

        const int x = static_cast<int>(32 * (i % width));
        const int y = static_cast<int>(32 * (i / width));

        std::vector<SurfacePtr> regions;
        regions.reserve(surfaces.size());
        std::transform(surfaces.begin(), surfaces.end(), std::back_inserter(regions),
            [x, y] (const SurfacePtr& surface) { 
              return surface->region(Rect(x, y, Size(32, 32)));
            });

        std::vector<SurfacePtr> editor_regions;
        editor_regions.reserve(editor_surfaces.size());
        std::transform(editor_surfaces.begin(), editor_surfaces.end(), std::back_inserter(editor_regions),
            [x, y] (const SurfacePtr& surface) { 
              return surface->region(Rect(x, y, Size(32, 32)));
            });

        auto tile = std::make_unique<Tile>(regions,
                                           editor_regions,
                                           (has_attributes ? attributes[i] : 0),
                                           (has_datas ? datas[i] : 0),
                                           fps, deprecated);

        m_tileset.add_tile(ids[i], std::move(tile));
      }
    }
    else // (!shared_surface)
    {
      for (size_t i = 0; i < ids.size(); ++i)
      {
        if(!ids[i] || (ids[i] < static_cast<uint32_t>(m_start) || (m_end && ids[i] > static_cast<uint32_t>(m_end)))) continue;
        ids[i] += m_offset + tiles_offset;

        int x = static_cast<int>(32 * (i % width));
        int y = static_cast<int>(32 * (i / width));

        std::vector<SurfacePtr> surfaces;
        std::optional<ReaderMapping> surfaces_mapping;
        if (reader.get("image", surfaces_mapping) ||
           reader.get("images", surfaces_mapping)) {
          surfaces = parse_imagespecs(*surfaces_mapping, Rect(x, y, Size(32, 32)));
        }

        std::vector<SurfacePtr> editor_surfaces;
        std::optional<ReaderMapping> editor_surfaces_mapping;
        if (reader.get("editor-images", editor_surfaces_mapping)) {
          editor_surfaces = parse_imagespecs(*editor_surfaces_mapping, Rect(x, y, Size(32, 32)));
        }

        auto tile = std::make_unique<Tile>(surfaces,
                                           editor_surfaces,
                                           (has_attributes ? attributes[i] : 0),
                                           (has_datas ? datas[i] : 0),
                                           fps, deprecated);

        m_tileset.add_tile(ids[i], std::move(tile));
      }
    }
  }
}

std::vector<SurfacePtr>
  TileSetParser::parse_imagespecs(const ReaderMapping& images_mapping,
                                  const std::optional<Rect>& surface_region) const
{
  std::vector<SurfacePtr> surfaces;

  // (images "foo.png" "foo.bar" ...)
  // (images (region "foo.png" 0 0 32 32))
  auto iter = images_mapping.get_iter();
  while (iter.next())
  {
    if (iter.is_string())
    {
      std::string file = iter.as_string_item();
      surfaces.push_back(Surface::from_file(FileSystem::join(m_tiles_path, file), surface_region));
    }
    else if (iter.is_pair() && iter.get_key() == "surface")
    {
      surfaces.push_back(Surface::from_reader(iter.as_mapping(), surface_region));
    }
    else if (iter.is_pair() && iter.get_key() == "region")
    {
      auto const& sx = iter.as_mapping().get_sexp();
      auto const& arr = sx.as_array();
      if (arr.size() != 6)
      {
        log_warning << "(region X Y WIDTH HEIGHT) tag malformed: " << sx << std::endl;
      }
      else
      {
        const std::string file = arr[1].as_string();
        const int x = arr[2].as_int();
        const int y = arr[3].as_int();
        const int w = arr[4].as_int();
        const int h = arr[5].as_int();

        Rect rect(x, y, x + w, y + h);

        if (surface_region)
        {
          rect.left += surface_region->left;
          rect.top += surface_region->top;

          rect.right = rect.left + surface_region->get_width();
          rect.bottom = rect.top + surface_region->get_height();
        }

        surfaces.push_back(Surface::from_file(FileSystem::join(m_tiles_path, file),
                                              rect));
      }
    }
    else
    {
      log_warning << "Expected string or list in images tag" << std::endl;
    }
  }

  return surfaces;
}

/* EOF */
