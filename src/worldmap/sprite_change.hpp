//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_WORLDMAP_SPRITE_CHANGE_HPP
#define HEADER_SUPERTUX_WORLDMAP_SPRITE_CHANGE_HPP

#include "worldmap/worldmap_object.hpp"

#include <string>

namespace worldmap {

class SpriteChange final : public WorldMapObject
{
public:
  SpriteChange(const ReaderMapping& mapping);
  ~SpriteChange() override;

  static std::string class_name() { return "sprite-change"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Sprite Change"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return WorldMapObject::get_class_types().add(typeid(SpriteChange)); }

  virtual void draw_worldmap(DrawingContext& context) override;

  virtual ObjectSettings get_settings() override;

  /**
   * Activates the SpriteChange's stay action, if applicable
   */
  void set_stay_action();
  /**
   * Deactivates the SpriteChange's stay action, if applicable
   * @param propagate : Also change stay actions in the same stay group
   */
  void clear_stay_action(bool propagate = true);
  /**
   * Get the current value of in_stay_action
   */
  bool show_stay_action() const;

  /**
   * Clone the current sprite.
   */
  SpritePtr clone_sprite() const;

  inline bool change_on_touch() const { return m_change_on_touch; }

private:
  /** should Tux's sprite change when the tile has been completely entered,
      or already when the tile was just touched */
  bool m_change_on_touch;

  /** stay action can be used for objects like boats or cars, if it is
      not empty then this sprite will be displayed when tux left the
      tile towards another SpriteChange object. */
  std::string m_stay_action;

  /** name of a group in which only one SpriteChange will ever have
      its stay_action displayed.  Leave empty if you don't care. */
  std::string m_stay_group;

  /** should the stayaction be displayed */
  bool m_in_stay_action;

private:
  SpriteChange(const SpriteChange&) = delete;
  SpriteChange& operator=(const SpriteChange&) = delete;
};

} // namespace worldmap

#endif

/* EOF */
