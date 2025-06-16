//  SuperTux - Unstable Tile
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//  Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
//  Copyright (C) 2010 Florian Forster <supertux at octo.it>
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

#ifndef HEADER_SUPERTUX_OBJECT_UNSTABLE_TILE_HPP
#define HEADER_SUPERTUX_OBJECT_UNSTABLE_TILE_HPP

#include "object/moving_sprite.hpp"

#include "supertux/physic.hpp"
#include "supertux/timer.hpp"
#include "util/fade_helper.hpp"

/** A block that disintegrates when stood on */
class UnstableTile final : public MovingSprite
{
public:
  UnstableTile(const ReaderMapping& mapping, int type = -1);

  virtual HitResponse collision(MovingObject& other, const CollisionHit& hit) override;
  virtual void update(float dt_sec) override;
  virtual void draw(DrawingContext& context) override;
  virtual void on_flip(float height) override;

  static std::string class_name() { return "unstable_tile"; }
  virtual std::string get_class_name() const override { return class_name(); }
  static std::string display_name() { return _("Unstable Tile"); }
  virtual std::string get_display_name() const override { return display_name(); }
  virtual GameObjectClasses get_class_types() const override { return MovingSprite::get_class_types().add(typeid(UnstableTile)); }

  GameObjectTypes get_types() const override;
  std::string get_default_sprite_name() const override;

public:
  enum Type {
    ICE,
    BRICK,
    DELAYED
  };

private:
  enum State {
    STATE_NORMAL,   /**< default state */
    STATE_SHAKE,    /**< shaking, still solid */
    STATE_DISSOLVE, /**< dissolving, will turn non-solid after this */
    STATE_SLOWFALL, /**< slow fall phase (used when neither shaking nor dissolving exist */
    STATE_FALL      /**< falling down */
  };

private:
  void shake();
  void dissolve();
  void fall_down();
  void slow_fall();
  void revive();

private:
  Physic physic;
  State state;
  float slowfall_timer;

  Timer m_revive_timer;
  std::unique_ptr<FadeHelper> m_respawn;
  float m_alpha;
  Vector m_original_pos;

  /** DELAYED type management */
  Timer m_fall_timer;
  bool m_player_hit;

private:
  UnstableTile(const UnstableTile&) = delete;
  UnstableTile& operator=(const UnstableTile&) = delete;
};

#endif

/* EOF */
