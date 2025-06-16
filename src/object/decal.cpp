//  SuperTux - Decal
//  Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
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

#include "object/decal.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "supertux/flip_level_transformer.hpp"
#include "sprite/sprite_manager.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"

Decal::Decal(const ReaderMapping& reader) :
  MovingSprite(reader, "images/decal/explanations/billboard-bigtux.png", LAYER_OBJECTS, COLGROUP_DISABLED),
  m_default_action("default"),
  m_solid(),
  m_fade_sprite(m_sprite.get()->clone()),
  m_fade_timer(),
  m_sprite_timer(),
  m_visible(true)
{
  m_layer = reader_get_layer(reader, LAYER_OBJECTS);

  reader.get("solid", m_solid, false);
  if (m_solid)
    set_group(COLGROUP_STATIC);
  if (reader.get("action", m_default_action))
    set_action(m_default_action, -1);
}

ObjectSettings
Decal::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_bool(_("Solid"), &m_solid, "solid", false);
  result.add_text(_("Action"), &m_default_action, "action", "default");

  result.reorder({"z-pos", "sprite", "x", "y"});

  return result;
}

Decal::~Decal()
{
}

void
Decal::draw(DrawingContext& context)
{
  if (m_visible || m_fade_timer.started())
    m_sprite->draw(context.color(), get_pos(), m_layer, m_flip);
  if (m_visible && m_sprite_timer.started())
    m_fade_sprite->draw(context.color(), get_pos(), m_layer, m_flip);
}

void
Decal::fade_in(float fade_time)
{
  if (m_visible)
    return;
  m_visible = true;
  m_fade_timer.start(fade_time);
}

void
Decal::fade_out(float fade_time)
{
  if (!m_visible)
    return;
  m_visible = false;
  m_fade_timer.start(fade_time);
}

void
Decal::fade_sprite(const std::string& new_sprite, float fade_time)
{
  m_fade_sprite = SpriteManager::current()->create(new_sprite);
  m_sprite.swap(m_fade_sprite);
  // From now on flip_sprite == the old one
  m_sprite.get()->set_alpha(0);
  m_sprite_timer.start(fade_time);
}

void
Decal::on_flip(float height)
{
  MovingObject::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

void
Decal::update(float)
{
  if (m_sprite_timer.started())
  {
    if (m_sprite_timer.check())
    {
      m_sprite.get()->set_alpha(1.0f);
      m_fade_sprite.get()->set_alpha(0.0f);
      m_sprite_timer.stop();
    }
    else
    {
      // Square root makes the background stay at fairly constant color/transparency
      float new_alpha = sqrtf(m_sprite_timer.get_progress());
      float old_alpha = sqrtf(m_sprite_timer.get_timeleft() / m_sprite_timer.get_period());
      m_sprite.get()->set_alpha(new_alpha);
      m_fade_sprite.get()->set_alpha(old_alpha);
    }
  }

  if (m_fade_timer.started())
  {
    if (m_fade_timer.check())
    {
      if(m_visible)
        m_sprite.get()->set_alpha(1.0f);
      else
        m_sprite.get()->set_alpha(0.0f);
      m_fade_timer.stop();
    }
    else
    {
      float alpha;
      if (m_visible) alpha = m_fade_timer.get_progress();
      else alpha = m_fade_timer.get_timeleft() / m_fade_timer.get_period();
      m_sprite.get()->set_alpha(alpha);
    }
  }
}


void
Decal::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Decal>("Decal", vm.findClass("MovingSprite"));

  cls.addFunc("fade_sprite", &Decal::fade_sprite);
  cls.addFunc("change_sprite", &MovingSprite::change_sprite); // Deprecated; for compatibility
  cls.addFunc("fade_in", &Decal::fade_in);
  cls.addFunc("fade_out", &Decal::fade_out);
}

/* EOF */
