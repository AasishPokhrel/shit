//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//  Copyright (C) 2017 M. Teufel <mteufel@supertux.org>
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

#include "object/torch.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "object/player.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "util/reader_mapping.hpp"

Torch::Torch(const ReaderMapping& reader) :
  MovingSprite(reader, "images/objects/torch/torch1.sprite", LAYER_TILES),
  m_light_color(1.f, 1.f, 1.f),
  m_flame(SpriteManager::current()->create("images/objects/torch/flame.sprite")),
  m_flame_glow(SpriteManager::current()->create("images/objects/torch/flame_glow.sprite")),
  m_flame_light(SpriteManager::current()->create("images/objects/torch/flame_light.sprite")),
  m_burning(true)
{
  reader.get("burning", m_burning, true);
  reader.get("layer", m_layer); // Backwards compatibility

  std::vector<float> vColor;
  if (!reader.get("color", vColor)) vColor = { 1.f, 1.f, 1.f };

  m_flame_glow->set_blend(Blend::ADD);
  m_flame_light->set_blend(Blend::ADD);
  if (vColor.size() >= 3)
  {
    m_light_color = Color(vColor);
    m_flame->set_color(m_light_color);
    m_flame_glow->set_color(m_light_color);
    m_flame_light->set_color(m_light_color);
  }
  set_group(COLGROUP_TOUCHABLE);
}

void
Torch::draw(DrawingContext& context)
{
  if (m_burning)
  {
    Vector pos = get_pos();
    if (m_flip != NO_FLIP) pos.y -= 24.0f;
    m_flame->draw(context.color(), pos, m_layer - 1, m_flip);
    m_flame->set_action(m_light_color.greyscale() >= 1.f ? "default" : "greyscale");

    m_flame_light->draw(context.light(), pos, m_layer);
    m_flame_light->set_action(m_light_color.greyscale() >= 1.f ? "default" : "greyscale");

    m_flame_glow->draw(context.color(), pos, m_layer - 1, m_flip);
    m_flame_glow->set_action(m_light_color.greyscale() >= 1.f ? "default" : "greyscale");
  }

  m_sprite->draw(context.color(), get_pos(), m_layer - 1, m_flip);
}

void
Torch::update(float)
{
}

HitResponse
Torch::collision(MovingObject& other, const CollisionHit& )
{
  const auto* player = dynamic_cast<Player*>(&other);
  if (player != nullptr && !m_burning)
  {
    m_burning = true;
  }
  return ABORT_MOVE;
}

ObjectSettings
Torch::get_settings()
{
  ObjectSettings result = MovingSprite::get_settings();

  result.add_bool(_("Burning"), &m_burning, "burning", true);
  result.add_color(_("Color"), &m_light_color, "color", Color::WHITE);

  result.reorder({"burning", "sprite", "z-pos", "color", "x", "y"});

  return result;
}

void
Torch::after_editor_set()
{
  MovingSprite::after_editor_set();

  m_flame->set_color(m_light_color);
  m_flame_glow->set_color(m_light_color);
  m_flame_light->set_color(m_light_color);
}

void
Torch::on_flip(float height)
{
  MovingObject::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}


void
Torch::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<Torch>("Torch", vm.findClass("MovingSprite"));

  cls.addFunc("get_burning", &Torch::get_burning);
  cls.addFunc("set_burning", &Torch::set_burning);

  cls.addVar("burning", &Torch::m_burning);
}

/* EOF */
