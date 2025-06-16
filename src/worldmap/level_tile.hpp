//  SuperTux
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmail.com>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//                2023 Vankata453
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

#ifndef HEADER_SUPERTUX_WORLDMAP_LEVEL_TILE_HPP
#define HEADER_SUPERTUX_WORLDMAP_LEVEL_TILE_HPP

#include "worldmap/worldmap_object.hpp"

#include "supertux/statistics.hpp"

namespace worldmap {

class LevelTile final : public WorldMapObject
{
public:
  LevelTile(const ReaderMapping& mapping);
  ~LevelTile() override;

  static std::string class_name() { return "level"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Level"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WorldMapObject::get_class_types().add(typeid(LevelTile)); }

  virtual ObjectSettings get_settings() override;

  inline void set_solved(bool v) { m_solved = v; update_sprite_action(); }
  inline bool is_solved() const { return m_solved; }

  inline void set_perfect(bool v) { m_perfect = v; update_sprite_action(); }
  inline bool is_perfect() const { return m_perfect; }

  inline Statistics& get_statistics() { return m_statistics; }
  inline const Statistics& get_statistics() const { return m_statistics; }

  void update_sprite_action();

  inline const std::string& get_title() const { return m_title; }
  inline const std::string& get_level_filename() const { return m_level_filename; }
  inline const std::string& get_basedir() const { return m_basedir; }
  inline Color get_title_color() const { return m_title_color; }
  inline const std::string& get_extro_script() const { return m_extro_script; }
  inline float get_target_time() const { return m_target_time; }
  inline bool is_auto_play() const { return m_auto_play; }

private:
  void load_level_information();

private:
  std::string m_basedir;
  std::string m_level_filename;
  std::string m_title;

  /** true if Tux should automatically enter this level if it's unfinished */
  bool m_auto_play;

  float m_target_time;

  /** Script that is run when the level is successfully finished */
  std::string m_extro_script;

  /** Level state */
  bool m_solved;
  bool m_perfect;

  Statistics m_statistics;

  Color m_title_color;

private:
  LevelTile(const LevelTile&) = delete;
  LevelTile& operator=(const LevelTile&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
