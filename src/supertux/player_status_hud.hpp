//  SuperTux
//  Copyright (C) 2003 Tobias Glaesser <tobi.web@gmx.de>
//                2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_PLAYER_STATUS_HUD_HPP
#define HEADER_SUPERTUX_SUPERTUX_PLAYER_STATUS_HUD_HPP

#include "supertux/game_object.hpp"

#include <unordered_map>

#include "sprite/sprite.hpp"
#include "supertux/player_status.hpp"
#include "video/color.hpp"
#include "video/surface_ptr.hpp"

class DrawingContext;

class PlayerStatusHUD : public GameObject
{
private:
  static Color text_color;

public:
  PlayerStatusHUD(PlayerStatus& player_status);
  virtual GameObjectClasses get_class_types() const override { return GameObject::get_class_types().add(typeid(PlayerStatusHUD)); }

  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;

  virtual bool is_saveable() const override { return false; }
  virtual bool is_singleton() const override { return true; }
  virtual bool track_state() const override { return false; }

  void reset();

private:
  PlayerStatus& m_player_status;
  int displayed_coins;
  int displayed_coins_frame;
  SurfacePtr coin_surface;

  std::unordered_map<BonusType, SpritePtr> m_bonus_sprites;
  SurfacePtr m_item_pocket_border;

private:
  PlayerStatusHUD(const PlayerStatusHUD&) = delete;
  PlayerStatusHUD& operator=(const PlayerStatusHUD&) = delete;
};

#endif

/* EOF */
