//  SuperTux
//  Copyright (C) 2023 Vankata453
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

#ifndef HEADER_SUPERTUX_SUPERTUX_SECTOR_BASE_HPP
#define HEADER_SUPERTUX_SUPERTUX_SECTOR_BASE_HPP

#include "supertux/game_object_manager.hpp"

#include "squirrel/squirrel_environment.hpp"

class Level;
class TileSet;

namespace Base {

/** A base for sector classes. Contains main properties and functions. */
class Sector : public GameObjectManager
{
public:
  Sector(const std::string& type);

  /** Needs to be called after parsing to finish the construction of
      the Sector before using it. */
  virtual void finish_construction(bool editable);

  virtual void draw(DrawingContext& context) = 0;
  virtual void update(float dt_sec) = 0;

  virtual TileSet* get_tileset() const = 0;
  virtual bool in_worldmap() const = 0;

  inline void set_name(const std::string& name) { m_name = name; }
  inline const std::string& get_name() const { return m_name; }

  inline void set_init_script(const std::string& init_script) { m_init_script = init_script; }
  void run_script(const std::string& script, const std::string& sourcename);

protected:
  virtual bool before_object_add(GameObject& object) override;
  virtual void before_object_remove(GameObject& object) override;

protected:
  std::string m_name;
  std::string m_init_script;

  std::unique_ptr<SquirrelEnvironment> m_squirrel_environment;

private:
  Sector(const Sector&) = delete;
  Sector& operator=(const Sector&) = delete;
};

} // namespace Base

#endif

/* EOF */
