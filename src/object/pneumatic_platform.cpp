//  SuperTux - PneumaticPlatform
//  Copyright (C) 2007 Christoph Sommer <christoph.sommer@2007.expires.deltadevelopment.de>
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

#include "object/pneumatic_platform.hpp"

#include "object/player.hpp"
#include "object/portable.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"

PneumaticPlatformChild::PneumaticPlatformChild(const ReaderMapping& mapping, bool left, PneumaticPlatform& parent) :
  MovingSprite(mapping, "images/objects/platforms/small.sprite", LAYER_OBJECTS, COLGROUP_STATIC),
  m_parent(parent),
  m_left(left),
  m_contacts()
{
  if (!m_left) {
    set_pos(get_pos() + Vector(get_bbox().get_width(), 0));
  }
}

PneumaticPlatformChild::~PneumaticPlatformChild()
{
}

void
PneumaticPlatformChild::update(float dt_sec)
{
  const float offset_y = m_left ? m_parent.m_offset_y : -m_parent.m_offset_y;
  const Vector movement(0, (m_parent.m_start_y + offset_y) - get_pos().y);
  m_col.set_movement(movement);
  m_col.propagate_movement(movement);
}

HitResponse
PneumaticPlatformChild::collision(MovingObject& other, const CollisionHit& )
{
  // somehow the hit parameter does not get filled in, so to determine (hit.top == true) we do this:
  if (other.get_bbox().get_bottom() > m_col.m_bbox.get_top() + 2) return FORCE_MOVE;

  auto pl = dynamic_cast<Player*>(&other);
  if (pl) {
    if (pl->is_big()) m_contacts.insert(nullptr);
    auto pomo = dynamic_cast<MovingObject*>(pl->get_grabbed_object());
    assert(pomo);
    m_contacts.insert(pomo);
  }

  m_contacts.insert(&other);
  return FORCE_MOVE;
}

void PneumaticPlatformChild::editor_delete()
{
  // removing a child removes the whole platform
  m_parent.editor_delete();
}

void
PneumaticPlatformChild::on_flip(float height)
{
  MovingSprite::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

PneumaticPlatform::PneumaticPlatform(const ReaderMapping& mapping) :
  GameObject(mapping),
  m_pos(0.0f, 0.0f),
  m_sprite_name(),
  m_start_y(),
  m_speed_y(0),
  m_offset_y(0),
  m_children()
{
  mapping.get("x", m_pos.x);
  mapping.get("y", m_pos.y);
  mapping.get("sprite", m_sprite_name);

  m_children.push_back(&d_sector->add<PneumaticPlatformChild>(mapping, true, *this));
  m_children.push_back(&d_sector->add<PneumaticPlatformChild>(mapping, false, *this));

  m_start_y = m_children[0]->get_pos().y;
}

PneumaticPlatform::~PneumaticPlatform()
{
}

void
PneumaticPlatform::draw(DrawingContext& context)
{
}

void
PneumaticPlatform::update(float dt_sec)
{
  const int contact_diff = static_cast<int>(m_children[0]->m_contacts.size()) - static_cast<int>(m_children[1]->m_contacts.size());
  for (const auto& child : m_children) {
    child->m_contacts.clear();
  }

  const float gravity = Sector::get().get_gravity();

  m_speed_y += (static_cast<float>(contact_diff) * dt_sec) * 12.8f;
  m_speed_y -= (m_offset_y * dt_sec * 0.05f);
  m_speed_y *= 1 - dt_sec;

  m_offset_y += m_speed_y * dt_sec * gravity;

  if (m_offset_y < -256) {
    m_offset_y = -256;
    m_speed_y = 0;
  }

  if (m_offset_y > 256) {
    m_offset_y = 256;
    m_speed_y = -0;
  }
}

void
PneumaticPlatform::on_flip(float height)
{
  m_pos.y = height - m_pos.y - m_children[0]->m_col.m_bbox.get_height();
  m_start_y = height - m_start_y - m_children[0]->m_col.m_bbox.get_height();
}

void
PneumaticPlatform::editor_delete()
{
  // remove children
  for (auto& child : m_children) {
    child->remove_me();
  }

  // remove self
  remove_me();
}

ObjectSettings
PneumaticPlatform::get_settings()
{
  ObjectSettings result = GameObject::get_settings();

  result.add_sprite(_("Sprite"), &m_sprite_name, "sprite", "images/objects/platforms/small.sprite");
  result.add_float(_("X"), &m_pos.x, "x", 0.0f, OPTION_HIDDEN);
  result.add_float(_("Y"), &m_pos.y, "y", 0.0f, OPTION_HIDDEN);

  return result;
}

void
PneumaticPlatform::after_editor_set()
{
  GameObject::after_editor_set();
}

/* EOF */
