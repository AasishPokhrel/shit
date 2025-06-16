//  SuperTux - Lantern
//  Copyright (C) 2006 Wolfgang Becker <uafr@gmx.de>
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

#include "object/lantern.hpp"

#include <algorithm>

#include "audio/sound_manager.hpp"
#include "badguy/treewillowisp.hpp"
#include "badguy/willowisp.hpp"
#include "editor/editor.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "util/reader_mapping.hpp"

Lantern::Lantern(const ReaderMapping& reader) :
  Rock(reader, "images/objects/lantern/lantern.sprite"),
  lightcolor(1.0f, 1.0f, 1.0f),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light.sprite"))
{
  std::vector<float> vColor;
  if (reader.get("color", vColor)) {
    lightcolor = Color(vColor);
  } else {
    if (!Editor::is_active()) {
      lightcolor = Color(1, 1, 1);
    }
  }
  lightsprite->set_blend(Blend::ADD);
  updateColor();
  SoundManager::current()->preload("sounds/willocatch.wav");
}

Lantern::Lantern(const Vector& pos) :
  Rock(pos, "images/objects/lantern/lantern.sprite"),
  lightcolor(0.0f, 0.0f, 0.0f),
  lightsprite(SpriteManager::current()->create("images/objects/lightmap_light/lightmap_light.sprite"))
{
  lightsprite->set_blend(Blend::ADD);
  updateColor();
  SoundManager::current()->preload("sounds/willocatch.wav");
}

ObjectSettings
Lantern::get_settings()
{
  ObjectSettings result = Rock::get_settings();

  result.add_color(_("Color"), &lightcolor, "color", Color::WHITE);

  result.reorder({"color", "name", "x", "y"});

  return result;
}

void
Lantern::after_editor_set()
{
  Rock::after_editor_set();

  updateColor();
}

void
Lantern::updateColor(){
  lightsprite->set_color(lightcolor);
  //Turn lantern off if light is black
  if (lightcolor.red == 0 && lightcolor.green == 0 && lightcolor.blue == 0){
    set_action("off");
    m_sprite->set_color(Color(1.0f, 1.0f, 1.0f));
  } else {
    set_action("default");
    m_sprite->set_color(lightcolor);
  }
}

void
Lantern::draw(DrawingContext& context){
  //Draw the Sprite.
  MovingSprite::draw(context);
  //Let there be light.
  lightsprite->draw(context.light(), m_col.m_bbox.get_middle(), 0);
}

HitResponse Lantern::collision(MovingObject& other, const CollisionHit& hit) {

  WillOWisp* wow = dynamic_cast<WillOWisp*>(&other);

  if (wow && (is_open() || wow->get_color().greyscale() == 0.f)) {
    // collided with WillOWisp while grabbed and unlit
    SoundManager::current()->play("sounds/willocatch.wav", get_pos());
    lightcolor = wow->get_color();
    updateColor();
    wow->vanish();
  }

  TreeWillOWisp* twow = dynamic_cast<TreeWillOWisp*>(&other);
  if (twow && (is_open() || twow->get_color().greyscale() == 0.f)) {
    // collided with TreeWillOWisp while grabbed and unlit
    SoundManager::current()->play("sounds/willocatch.wav", get_pos());
    lightcolor = twow->get_color();
    updateColor();
    twow->vanish();
  }

  return Rock::collision(other, hit);
}

void
Lantern::grab(MovingObject& object, const Vector& pos, Direction dir)
{
  Rock::grab(object, pos, dir);

  // if lantern is not lit, draw it as opened
  if (is_open()) {
    set_action("off-open");
  }

}

void
Lantern::ungrab(MovingObject& object, Direction dir)
{
  // if lantern is not lit, it was drawn as opened while grabbed. Now draw it as closed again
  if (is_open()) {
    set_action("off");
  }

  Rock::ungrab(object, dir);
}

bool
Lantern::is_open() const
{
  return (is_grabbed() && lightcolor.red == 0 && lightcolor.green == 0 && lightcolor.blue == 0);
}

void
Lantern::add_color(const Color& c)
{
  lightcolor.red   = std::min(1.0f, lightcolor.red   + c.red);
  lightcolor.green = std::min(1.0f, lightcolor.green + c.green);
  lightcolor.blue  = std::min(1.0f, lightcolor.blue  + c.blue);
  lightcolor.alpha = std::min(1.0f, lightcolor.alpha + c.alpha);
  updateColor();
}

/* EOF */
