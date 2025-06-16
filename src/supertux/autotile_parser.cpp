//  SuperTux
//  Copyright (C) 2020 A. Semphris <semphris@protonmail.com>
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

#include "supertux/autotile_parser.hpp"

#include <sstream>
#include <sexp/value.hpp>
#include <sexp/io.hpp>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/log.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "util/file_system.hpp"

AutotileParser::AutotileParser(std::vector<std::unique_ptr<AutotileSet>>& autotilesets, const std::string& filename,
                               int32_t start, int32_t end, int32_t offset) :
  m_autotilesets(autotilesets),
  m_filename(filename),
  m_tiles_path(),
  m_start(start),
  m_end(end),
  m_offset(offset)
{
}

void
AutotileParser::parse()
{
  if (m_offset && m_start + m_offset < 1)
  {
    m_start = -m_offset + 1;
    log_warning << "The defined offset would assign non-positive IDs to autotiles, autotiles below " << -m_offset + 1 << " will be ignored." << std::endl;
  }
  if (m_end < 0)
  {
    log_warning << "Cannot import autotiles with negative IDs." << std::endl;
    return;
  }
  if (m_start < 0)
  {
    log_warning << "Cannot import autotiles with negative IDs. Importing will start at ID 1." << std::endl;
    m_start = 1;
  }

  m_tiles_path = FileSystem::dirname(m_filename);

  auto doc = ReaderDocument::from_file(m_filename);
  auto root = doc.get_root();

  if (root.get_name() != "supertux-autotiles") {
    throw std::runtime_error("File is not a supertux autotile configuration file (Root list doesn't start with 'supertux-autotiles').");
  }

  auto iter = root.get_mapping().get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "autotileset")
    {
      parse_autotileset(iter.as_mapping(), false);
    }
    else if (iter.get_key() == "autotileset-corner")
    {
      parse_autotileset(iter.as_mapping(), true);
    }
    else
    {
      log_warning << "Unknown symbol '" << iter.get_key() << "' in autotile config file" << std::endl;
    }
  }
}

void
AutotileParser::parse_autotileset(const ReaderMapping& reader, bool corner)
{
  std::vector<Autotile*> autotiles;

  std::string name = "[unnamed]";
  if (!reader.get("name", name))
  {
    log_warning << "Unnamed autotileset parsed" << std::endl;
  }

  uint32_t default_id = 0;
  if (!reader.get("default", default_id))
  {
    log_warning << "No default tile for autotileset " << name << std::endl;
  }

  if (default_id < static_cast<uint32_t>(m_start) || (m_end && default_id > static_cast<uint32_t>(m_end)))
    default_id = 0;
  else
    default_id += m_offset;

  auto iter = reader.get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "autotile")
    {
      Autotile* autotile = parse_autotile(iter.as_mapping(), corner);
      if (autotile)
        autotiles.push_back(autotile);
    }
    else if (iter.get_key() != "name" && iter.get_key() != "default")
    {
      log_warning << "Unknown symbol '" << iter.get_key() << "' in autotile config file" << std::endl;
    }
  }

  if (autotiles.empty())
    return;

  std::unique_ptr<AutotileSet> autotileset = std::make_unique<AutotileSet>(std::move(autotiles), default_id, name, corner);

  if (g_config->developer_mode)
  {
    autotileset->validate(m_start, m_end);
  }

  m_autotilesets.push_back(std::move(autotileset));
}

Autotile*
AutotileParser::parse_autotile(const ReaderMapping& reader, bool corner)
{
  std::vector<AutotileMask> autotile_masks;
  std::vector<std::pair<uint32_t, Autotile::AltConditions>> alt_ids;

  uint32_t tile_id;
  if (!reader.get("id", tile_id))
  {
    throw std::runtime_error("Missing 'id' parameter in autotileset config file.");
  }

  if (tile_id < static_cast<uint32_t>(m_start) || (m_end && tile_id > static_cast<uint32_t>(m_end)))
    return nullptr;

  tile_id += m_offset;

  bool solid;
  if (!reader.get("solid", solid))
  {
    if (!corner)
      throw std::runtime_error("Missing 'solid' parameter in autotileset config file.");
  }
  else
  {
    if (corner)
      log_warning << "'solid' parameter not needed for corner-based tiles in autotileset config file." << std::endl;
  }

  auto iter = reader.get_iter();
  while (iter.next())
  {
    if (iter.get_key() == "mask")
    {
      std::string mask;
      iter.get(mask);

      if (corner)
      {
        parse_mask_corner(mask, autotile_masks);
      }
      else
      {
        parse_mask(mask, autotile_masks, solid);
      }
    }
    else if (iter.get_key() == "alt-id")
    {
      ReaderMapping alt_reader = iter.as_mapping();

      uint32_t alt_id = 0;
      if (!alt_reader.get("id", alt_id))
      {
        log_warning << "No alt tile for autotileset" << std::endl;
        continue;
      }

      if (alt_id < static_cast<uint32_t>(m_start) || (m_end && alt_id > static_cast<uint32_t>(m_end)))
        continue;

      alt_id += m_offset;

      if (!alt_id)
        continue;

      std::vector<uint32_t> period_x = { 0, 0 };
      std::vector<uint32_t> period_y = { 0, 0 };
      float weight = 0.f;
      alt_reader.get("period-x", period_x);
      alt_reader.get("period-y", period_y);
      alt_reader.get("weight", weight);

      if (period_x.size() != 2 || period_y.size() != 2)
      {
        log_warning << "X/Y period for alt tile must contain exactly 2 values" << std::endl;
        continue;
      }

      alt_ids.push_back(std::pair<uint32_t, Autotile::AltConditions>(alt_id, {
          { period_x[0], period_x[1] },
          { period_y[0], period_y[1] },
          weight
        }));
    }
    else if (iter.get_key() != "id" && iter.get_key() != "solid")
    {
      log_warning << "Unknown symbol '" << iter.get_key() << "' in autotile config file" << std::endl;
    }
  }

  return new Autotile(tile_id, std::move(alt_ids), std::move(autotile_masks), solid);
}

void
AutotileParser::parse_mask(const std::string& mask, std::vector<AutotileMask>& autotile_masks, bool solid)
{
  parse_mask(mask, autotile_masks, solid, false);
}

void
AutotileParser::parse_mask_corner(const std::string& mask, std::vector<AutotileMask>& autotile_masks)
{
  parse_mask(mask, autotile_masks, true, true);
}

void
AutotileParser::parse_mask(const std::string& mask, std::vector<AutotileMask>& autotile_masks, bool solid, bool is_corner)
{
  size_t mask_size = is_corner ? 4 : 8;
  if (mask.size() != mask_size)
  {
    std::ostringstream msg;
    msg << (is_corner ? "Autotile config : corner-based mask isn't exactly 4 characters." : 
                       "Autotile config : mask isn't exactly 8 characters.");
    throw std::runtime_error(msg.str());
  }

  std::vector<uint8_t> masks;

  masks.push_back(0);

  for (size_t i = 0; i < mask_size; i++)
  {
    std::vector<uint8_t> new_masks;
    switch (mask[i])
    {
    case '0':
      for (uint8_t val : masks)
      {
        new_masks.push_back(static_cast<uint8_t>(val * 2));
      }
      break;
    case '1':
      for (uint8_t val : masks)
      {
        new_masks.push_back(static_cast<uint8_t>(val * 2 + 1));
      }
      break;
    case '*':
      for (uint8_t val : masks)
      {
        new_masks.push_back(static_cast<uint8_t>(val * 2));
        new_masks.push_back(static_cast<uint8_t>(val * 2 + 1));
      }
      break;
    default:
      throw std::runtime_error("Autotile config : unrecognized character");
    }
    masks = new_masks;
  }

  for (uint8_t val : masks)
  {
    autotile_masks.push_back(AutotileMask(val, solid));
  }
}

/* EOF */
