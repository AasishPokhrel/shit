//  SuperTux
//  Copyright (C) 2006 Ingo Ruhnke <grumbel@gmail.com>
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

#include "object/spotlight.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "util/reader.hpp"
#include "util/reader_mapping.hpp"

Spotlight::Direction
Spotlight::Direction_from_string(const std::string& s)
{
  if (s == "clockwise") {
    return Direction::CLOCKWISE;
  } else if (s == "counter-clockwise") {
    return Direction::COUNTERCLOCKWISE;
  } else if (s == "stopped") {
    return Direction::STOPPED;
  }

  throw std::runtime_error("Invalid spotlight direction from string '" + s + "'");
}

std::string
Spotlight::Direction_to_string(Direction dir)
{
  switch(dir) {
    case Direction::CLOCKWISE:
      return "clockwise";
    case Direction::COUNTERCLOCKWISE:
      return "counter-clockwise";
    case Direction::STOPPED:
      return "stopped";
  }

  throw std::runtime_error("Invalid spotlight direction '" + std::to_string(static_cast<int>(dir)) + "'");
}

Spotlight::Spotlight(const ReaderMapping& mapping) :
  MovingObject(mapping),
  m_angle(),
  m_center(SpriteManager::current()->create("images/objects/spotlight/spotlight_center.sprite")),
  m_base(SpriteManager::current()->create("images/objects/spotlight/spotlight_base.sprite")),
  m_lights(SpriteManager::current()->create("images/objects/spotlight/spotlight_lights.sprite")),
  m_light(SpriteManager::current()->create("images/objects/spotlight/light.sprite")),
  m_lightcone(SpriteManager::current()->create("images/objects/spotlight/lightcone.sprite")),
  m_color(1.0f, 1.0f, 1.0f),
  m_speed(50.0f),
  m_direction(),
  m_layer(0),
  m_enabled(true)
{
  m_col.m_group = COLGROUP_DISABLED;

  mapping.get("x", m_col.m_bbox.get_left(), 0.0f);
  mapping.get("y", m_col.m_bbox.get_top(), 0.0f);
  m_col.m_bbox.set_size(32, 32);

  mapping.get("angle", m_angle, 0.0f);
  mapping.get("speed", m_speed, 50.0f);

  if (!mapping.get_custom("r-direction", m_direction, Direction_from_string))
  {
    // Retrocompatibility
    bool counter_clockwise;
    mapping.get("counter-clockwise", counter_clockwise, false);
    m_direction = counter_clockwise ? Direction::COUNTERCLOCKWISE : Direction::CLOCKWISE;
  }

  std::vector<float> vColor;
  if (mapping.get("color", vColor))
    m_color = Color(vColor);

  m_layer = reader_get_layer(mapping, LAYER_TILES);
  mapping.get("enabled", m_enabled, true);
}

Spotlight::~Spotlight()
{
}

ObjectSettings
Spotlight::get_settings()
{
  ObjectSettings result = MovingObject::get_settings();

  result.add_bool(_("Enabled"), &m_enabled, "enabled", true);
  result.add_float(_("Angle"), &m_angle, "angle");
  result.add_color(_("Color"), &m_color, "color", Color::WHITE);
  result.add_float(_("Speed"), &m_speed, "speed", 50.0f);
  result.add_enum(_("Direction"), reinterpret_cast<int*>(&m_direction),
                  {_("Clockwise"), _("Counter-clockwise"), _("Stopped")},
                  {"clockwise", "counter-clockwise", "stopped"},
                  static_cast<int>(Direction::CLOCKWISE), "r-direction");
  result.add_int(_("Z-pos"), &m_layer, "z-pos", 0);

  result.reorder({"angle", "color", "z-pos", "x", "y"});

  return result;
}

void
Spotlight::update(float dt_sec)
{
  GameObject::update(dt_sec);

  if (!m_enabled)
    return;

  switch (m_direction)
  {
  case Direction::CLOCKWISE:
    m_angle += dt_sec * m_speed;
    break;

  case Direction::COUNTERCLOCKWISE:
    m_angle -= dt_sec * m_speed;
    break;

  case Direction::STOPPED:
    break;
  }
}

void
Spotlight::draw(DrawingContext& context)
{
  if (m_enabled)
  {
    m_light->set_color(m_color);
    m_light->set_blend(Blend::ADD);
    m_light->set_angle(m_angle);
    m_light->draw(context.light(), m_col.m_bbox.p1(), m_layer);

    //m_lightcone->set_angle(angle);
    //m_lightcone->draw(context.color(), position, m_layer);

    m_lights->set_color(m_color);
    m_lights->set_angle(m_angle);
    m_lights->draw(context.color(), m_col.m_bbox.p1(), m_layer);
  }

  m_base->set_angle(m_angle);
  m_base->draw(context.color(), m_col.m_bbox.p1(), m_layer);

  m_center->draw(context.color(), m_col.m_bbox.p1(), m_layer);

  if (m_enabled)
  {
    m_lightcone->set_color(m_color);
    m_lightcone->set_angle(m_angle);
    m_lightcone->draw(context.color(), m_col.m_bbox.p1(), LAYER_FOREGROUND1 + 10);
  }
}

HitResponse
Spotlight::collision(MovingObject& other, const CollisionHit& hit_)
{
  return FORCE_MOVE;
}

void
Spotlight::set_direction(const std::string& direction)
{
  m_direction = Direction_from_string(direction);
}

void
Spotlight::ease_speed(float speed, float time, const std::string& easing_)
{
  ease_speed(time, speed, EasingMode_from_string(easing_));
}

void
Spotlight::ease_angle(float angle, float time, const std::string& easing_)
{
  ease_angle(time, angle, EasingMode_from_string(easing_));
}

void
Spotlight::ease_color_rgba(float r, float g, float b, float a, float time, const std::string& easing_)
{
  ease_color(time, Color(r, g, b, a), EasingMode_from_string(easing_));
}

void
Spotlight::ease_angle(float time, float target, EasingMode ease)
{
  m_fade_helpers.push_back(std::make_unique<FadeHelper>(&m_angle, time, target, getEasingByName(ease)));
}

void
Spotlight::ease_speed(float time, float target, EasingMode ease)
{
  m_fade_helpers.push_back(std::make_unique<FadeHelper>(&m_speed, time, target, getEasingByName(ease)));
}

void
Spotlight::ease_color(float time, Color target, EasingMode ease)
{
  m_fade_helpers.push_back(std::make_unique<FadeHelper>(&m_color.red,   time, target.red,   getEasingByName(ease)));
  m_fade_helpers.push_back(std::make_unique<FadeHelper>(&m_color.green, time, target.green, getEasingByName(ease)));
  m_fade_helpers.push_back(std::make_unique<FadeHelper>(&m_color.blue,  time, target.blue,  getEasingByName(ease)));
  m_fade_helpers.push_back(std::make_unique<FadeHelper>(&m_color.alpha, time, target.alpha, getEasingByName(ease)));
}


void
Spotlight::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Spotlight>("Spotlight", vm.findClass("MovingObject"));

  cls.addFunc("set_enabled", &Spotlight::set_enabled);
  cls.addFunc("is_enabled", &Spotlight::is_enabled);
  cls.addFunc("set_direction", &Spotlight::set_direction);
  cls.addFunc("set_angle", &Spotlight::set_angle);
  cls.addFunc("fade_angle", &Spotlight::fade_angle);
  cls.addFunc<void, Spotlight, float, float, const std::string&>("ease_angle", &Spotlight::ease_angle);
  cls.addFunc("set_speed", &Spotlight::set_speed);
  cls.addFunc("fade_speed", &Spotlight::fade_speed);
  cls.addFunc<void, Spotlight, float, float, const std::string&>("ease_speed", &Spotlight::ease_speed);
  cls.addFunc("set_color_rgba", &Spotlight::set_color_rgba);
  cls.addFunc("fade_color_rgba", &Spotlight::fade_color_rgba);
  cls.addFunc("ease_color_rgba", &Spotlight::ease_color_rgba);

  cls.addVar("enabled", &Spotlight::m_enabled);
  cls.addVar("angle", &Spotlight::m_angle);
  cls.addVar("speed", &Spotlight::m_speed);
}

/* EOF */
