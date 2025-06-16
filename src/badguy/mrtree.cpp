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

#include "badguy/mrtree.hpp"

#include <math.h>

#include "audio/sound_manager.hpp"
#include "badguy/stumpy.hpp"
#include "badguy/viciousivy.hpp"
#include "math/random.hpp"
#include "math/util.hpp"
#include "object/player.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/sector.hpp"

static const float VICIOUSIVY_WIDTH = 32;
static const float VICIOUSIVY_HEIGHT = 32;
static const float VICIOUSIVY_Y_OFFSET = 24;

MrTree::MrTree(const ReaderMapping& reader) :
  WalkingBadguy(reader, "images/creatures/mr_tree/mr_tree.sprite","left","right", LAYER_OBJECTS,
                "images/objects/lightmap_light/lightmap_light-large.sprite")
{
  parse_type(reader);

  set_ledge_behavior(LedgeBehavior::SMART);
  SoundManager::current()->preload("sounds/mr_tree.ogg");
}

GameObjectTypes
MrTree::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "corrupted", _("Corrupted") }
  };
}

std::string
MrTree::get_default_sprite_name() const
{
  switch (m_type)
  {
    case CORRUPTED:
      return "images/creatures/mr_tree/corrupted/haunted_tree.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
MrTree::on_type_change(int old_type)
{
  MovingSprite::on_type_change(old_type);

  switch (m_type)
  {
    case NORMAL:
      walk_speed = 80.f;
      break;
    case CORRUPTED:
      walk_speed = 70.f;
      break;
    default:
      break;
  }
}

bool
MrTree::is_freezable() const
{
  return true;
}

bool
MrTree::collision_squished(MovingObject& object)
{
  if (m_type == CORRUPTED || m_frozen)
    return WalkingBadguy::collision_squished(object);

  auto player = dynamic_cast<Player*>(&object);
  if (player && (player->m_does_buttjump || player->is_invincible())) {
    player->bounce(*this);
    kill_fall();
    return true;
  }

  // Replace with Stumpy.
  Vector stumpy_pos = get_pos();
  stumpy_pos.x += 8;
  stumpy_pos.y += 28;
  auto& stumpy = Sector::get().add<Stumpy>(stumpy_pos, m_dir);
  remove_me();

  // Give feedback.
  SoundManager::current()->play("sounds/mr_tree.ogg", get_pos());
  if (player) player->bounce(*this);

  // Spawn some particles.
  // TODO: Provide convenience function in MovingSprite or MovingObject?
  for (int px = static_cast<int>(stumpy.get_bbox().get_left()); px < static_cast<int>(stumpy.get_bbox().get_right()); px+=10) {
    Vector ppos = Vector(static_cast<float>(px),
                         static_cast<float>(stumpy.get_bbox().get_top()) - 5.0f);
    float angle = graphicsRandom.randf(-math::PI_2, math::PI_2);
    float velocity = graphicsRandom.randf(45, 90);
    float vx = sinf(angle)*velocity;
    float vy = -cosf(angle)*velocity;
    Vector pspeed = Vector(vx, vy);
    Vector paccel = Vector(0, Sector::get().get_gravity()*10);
    Sector::get().add<SpriteParticle>("images/particles/leaf.sprite",
                                           "default",
                                           ppos, ANCHOR_MIDDLE,
                                           pspeed, paccel,
                                           LAYER_OBJECTS-1);
  }

  if (!m_frozen) { // Mr.Trees that are frozen don't spawn any Vicious Ivys.
    // Spawn ViciousIvy.
    Vector leaf1_pos(stumpy_pos.x - VICIOUSIVY_WIDTH - 1, stumpy_pos.y - VICIOUSIVY_Y_OFFSET);
    Rectf leaf1_bbox(leaf1_pos.x, leaf1_pos.y, leaf1_pos.x + VICIOUSIVY_WIDTH, leaf1_pos.y + VICIOUSIVY_HEIGHT);
    if (Sector::get().is_free_of_movingstatics(leaf1_bbox, this)) {
      auto& leaf1 = Sector::get().add<ViciousIvy>(leaf1_bbox.p1(), Direction::LEFT);
      leaf1.m_countMe = false;
    }

    // Spawn ViciousIvy.
    Vector leaf2_pos(stumpy_pos.x + m_sprite->get_current_hitbox_width() + 1, stumpy_pos.y - VICIOUSIVY_Y_OFFSET);
    Rectf leaf2_bbox(leaf2_pos.x, leaf2_pos.y, leaf2_pos.x + VICIOUSIVY_WIDTH, leaf2_pos.y + VICIOUSIVY_HEIGHT);
    if (Sector::get().is_free_of_movingstatics(leaf2_bbox, this)) {
      auto& leaf2 = Sector::get().add<ViciousIvy>(leaf2_bbox.p1(), Direction::RIGHT);
      leaf2.m_countMe = false;
    }
  }
  return true;
}

/* EOF */
