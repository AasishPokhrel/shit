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

#include "object/invisible_block.hpp"

#include "audio/sound_manager.hpp"
#include "editor/editor.hpp"
#include "object/player.hpp"
#include "supertux/constants.hpp"

InvisibleBlock::InvisibleBlock(const ReaderMapping& mapping) :
  Block(mapping, "images/objects/bonus_block/invisibleblock.sprite"),
  visible(false)
{
  parse_type(mapping);

  SoundManager::current()->preload("sounds/brick.wav");
}

GameObjectTypes
InvisibleBlock::get_types() const
{
  return {
    { "normal", _("Normal") },
    { "retro", _("Retro") }
  };
}

std::string
InvisibleBlock::get_default_sprite_name() const
{
  switch (m_type)
  {
    case RETRO:
      return "images/objects/bonus_block/retro_invisibleblock.sprite";
    default:
      return m_default_sprite_name;
  }
}

void
InvisibleBlock::draw(DrawingContext& context)
{
  if (visible || Editor::is_active())
    m_sprite->draw(context.color(), get_pos(), LAYER_OBJECTS);
}

bool
InvisibleBlock::collides(MovingObject& other, const CollisionHit& ) const
{
  if (visible)
    return true;

  // if we're not visible, only register a collision if this will make us visible
  auto player = dynamic_cast<Player*> (&other);
  if ((player)
      && (player->get_movement().y <= 0)
      && (player->get_bbox().get_top() > get_bbox().get_bottom() - SHIFT_DELTA)) {
    return true;
  }

  return false;
}

HitResponse
InvisibleBlock::collision(MovingObject& other, const CollisionHit& hit_)
{
  return Block::collision(other, hit_);
}

void
InvisibleBlock::hit(Player& player)
{
  SoundManager::current()->play("sounds/brick.wav", get_pos());

  if (visible)
    return;

  set_action("empty");
  start_bounce(&player);
  set_group(COLGROUP_STATIC);
  visible = true;
}

/* EOF */
