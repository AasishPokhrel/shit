//  SuperTux
//  Copyright (C) 2022 A. Semphris <semphris@protonmail.com>
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

#include "object/lit_object.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "util/reader_mapping.hpp"

LitObject::LitObject(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/lightflower/lightflower1.sprite", LAYER_TILES),
  m_light_offset(-6.f, -17.f),
  m_light_sprite_name("images/objects/lightflower/light/glow_light.sprite"),
  m_sprite_action("default"),
  m_light_sprite_action("default"),
  m_light_sprite()
{
  reader.get("light-offset-x", m_light_offset.x);
  reader.get("light-offset-y", m_light_offset.y);

  reader.get("light-sprite", m_light_sprite_name);
  reader.get("layer", m_layer); // Backwards compatibility

  reader.get("action", m_sprite_action);
  reader.get("light-action", m_light_sprite_action);

  m_light_sprite = SpriteManager::current()->create(m_light_sprite_name);
  m_light_sprite->set_blend(Blend::ADD);

  set_action(m_sprite_action);
  m_light_sprite->set_action(m_light_sprite_action);

  set_group(COLGROUP_DISABLED);
}

void
LitObject::draw(DrawingContext& context)
{
  m_sprite->draw(context.color(), get_pos(), m_layer - 1, m_flip);
  m_light_sprite->draw(context.light(), get_pos() - m_light_offset, m_layer - 1, m_flip);
}

void
LitObject::update(float)
{
}

ObjectSettings
LitObject::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_sprite(_("Light sprite"), &m_light_sprite_name, "light-sprite", "images/objects/lightflower/light/glow_light.sprite");

  result.add_text(_("Sprite starting action"), &m_sprite_action, "action", "default");
  result.add_text(_("Light sprite starting action"), &m_light_sprite_action, "light-action", "default");

  result.add_float(_("Light sprite offset X"), &m_light_offset.x, "light-offset-x");
  result.add_float(_("Light sprite offset Y"), &m_light_offset.y, "light-offset-y");

  return result;
}

void
LitObject::after_editor_set()
{
  MovingSprite::after_editor_set();

  m_light_sprite = SpriteManager::current()->create(m_light_sprite_name);
  m_light_sprite->set_blend(Blend::ADD);

  set_action(m_sprite_action);
  m_light_sprite->set_action(m_light_sprite_action);
}

void
LitObject::on_flip(float height)
{
  MovingObject::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

const std::string&
LitObject::get_light_action() const
{
  return m_light_sprite->get_action();
}

void
LitObject::set_light_action(const std::string& action)
{
  m_light_sprite->set_action(action);
}


void
LitObject::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<LitObject>("LitObject", vm.findClass("MovingSprite"));

  cls.addFunc("get_light_action", &LitObject::get_light_action);
  cls.addFunc("set_light_action", &LitObject::set_light_action);

  cls.addVar("light_action", &LitObject::get_light_action, &LitObject::set_light_action);
}

/* EOF */
