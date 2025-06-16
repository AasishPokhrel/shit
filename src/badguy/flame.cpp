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

#include "badguy/flame.hpp"

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "editor/editor.hpp"
#include "math/util.hpp"
#include "object/sprite_particle.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/flip_level_transformer.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/surface.hpp"

static const std::string FLAME_SOUND = "sounds/flame.wav";

Flame::Flame(const ReaderMapping& reader, int type) :
  BadGuy(reader, "images/creatures/flame/flame.sprite", LAYER_FLOATINGOBJECTS,
         "images/objects/lightmap_light/lightmap_light-small.sprite"),
  angle(0),
  radius(),
  speed(),
  sound_source(),
  m_radius_indicator(Surface::from_file("images/creatures/flame/flame-editor.png"))
{
  if (type >= 0)
  {
    m_type = type;
    on_type_change(TypeChange::INITIAL);
  }
  else
  {
    parse_type(reader);
  }

  reader.get("radius", radius, 100.0f);
  reader.get("speed", speed, 2.0f);

  if (!Editor::is_active())
  {
    m_col.m_bbox.set_pos(Vector(m_start_position.x + cosf(angle) * radius,
                                m_start_position.y + sinf(angle) * radius));
  }
  m_countMe = false;
  m_glowing = true;
  SoundManager::current()->preload(FLAME_SOUND);

  set_colgroup_active(COLGROUP_TOUCHABLE);

  switch (m_type)
  {
    case FIRE:
      m_lightsprite->set_color(Color(0.21f, 0.13f, 0.08f));
      break;
    case GHOST:
      m_lightsprite->set_color(Color(0.21f, 0.00f, 0.21f));
      break;
    case ICE:
      m_lightsprite->set_color(Color(0.00f, 0.13f, 0.18f));
      break;
  }
}

GameObjectTypes
Flame::get_types() const
{
  return {
    { "fire", _("Fire") },
    { "ghost", _("Ghost") },
    { "ice", _("Ice") }
  };
}

std::string
Flame::get_default_sprite_name() const
{
  switch (m_type)
  {
    case GHOST:
      return "images/creatures/flame/ghostflame.sprite";
    case ICE:
      return "images/creatures/flame/iceflame.sprite";
    default:
      return m_default_sprite_name;
  }
}

ObjectSettings
Flame::get_settings()
{
  ObjectSettings result = BadGuy::get_settings();

  result.add_float(_("Radius"), &radius, "radius", 100.0f);
  result.add_float(_("Speed"), &speed, "speed", 2.0f);

  result.reorder({"speed", "sprite", "x", "y"});

  return result;
}

void
Flame::active_update(float dt_sec)
{
  angle = fmodf(angle + dt_sec * speed, math::TAU);
  if (!Editor::is_active()) {
    Vector newpos(m_start_position.x + cosf(angle) * radius,
                  m_start_position.y + sinf(angle) * radius);
    m_col.set_movement(newpos - get_pos());
    sound_source->set_position(get_pos());
  }

  if (m_type == ICE)
    m_sprite->set_angle(math::degrees(angle) * 3.0f);

  if (m_sprite->get_action() == "fade" && m_sprite->animation_done())
    remove_me();
}

void
Flame::draw(DrawingContext& context)
{
  BadGuy::draw(context);

  if (Editor::is_active())
  {
    Rectf rect(Vector(get_pos().x - radius + get_bbox().get_width() / 2,
                      get_pos().y - radius + get_bbox().get_height() / 2),
               Sizef(radius * 2, radius * 2));
    context.color().draw_surface_scaled(m_radius_indicator, rect, m_layer);
  }
}

void
Flame::activate()
{
  if (Editor::is_active())
    return;
  sound_source = SoundManager::current()->create_sound_source(FLAME_SOUND);
  sound_source->set_position(get_pos());
  sound_source->set_looping(true);
  sound_source->set_gain(1.0f);
  sound_source->set_reference_distance(32);
  sound_source->play();
}

void
Flame::deactivate()
{
  sound_source.reset();
}


void
Flame::kill_fall()
{
}

void
Flame::freeze()
{
  if (!is_freezable())
    return;

  SoundManager::current()->play("sounds/sizzle.ogg", get_pos());
  set_action("fade", 1);
  Sector::get().add<SpriteParticle>("images/particles/smoke.sprite",
                                         "default",
                                         m_col.m_bbox.get_middle(), ANCHOR_MIDDLE,
                                         Vector(0, -150), Vector(0,0), LAYER_BACKGROUNDTILES+2);
  set_group(COLGROUP_DISABLED);

  // Start the dead-script.
  run_dead_script();
}

void
Flame::ignite()
{
  if (!is_flammable())
    return;

  SoundManager::current()->play("sounds/sizzle.ogg", get_pos());
  set_action("fade", 1);
  Sector::get().add<SpriteParticle>("images/particles/smoke.sprite",
                                         "default",
                                         m_col.m_bbox.get_middle(), ANCHOR_MIDDLE,
                                         Vector(0, -150), Vector(0,0),
                                         LAYER_BACKGROUNDTILES+2);
  set_group(COLGROUP_DISABLED);

  // Start death script.
  run_dead_script();
}

bool
Flame::is_freezable() const
{
  return m_type == FIRE;
}

bool
Flame::is_flammable() const
{
  return m_type == ICE;
}

void
Flame::stop_looping_sounds()
{
  if (sound_source) {
    sound_source->stop();
  }
}

void
Flame::play_looping_sounds()
{
  if (sound_source) {
    sound_source->play();
  }
}

std::vector<Direction>
Flame::get_allowed_directions() const
{
  return {};
}

void
Flame::on_flip(float height)
{
  BadGuy::on_flip(height);
  FlipLevelTransformer::transform_flip(m_flip);
}

/* EOF */
