//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2023 mrkubax10 <mrkubax10@onet.pl>
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

#include "object/ambient_sound.hpp"

#include <limits>

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/vm.hpp>

#include "audio/sound_manager.hpp"
#include "audio/sound_source.hpp"
#include "editor/editor.hpp"
#include "object/player.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

AmbientSound::AmbientSound(const ReaderMapping& mapping) :
  MovingObject(mapping),
  m_sample(),
  m_sound_source(),
  m_radius(),
  m_radius_in_px(),
  m_volume(),
  m_has_played_sound(false)
{
  m_col.m_group = COLGROUP_DISABLED;

  float w, h;
  mapping.get("x", m_col.m_bbox.get_left(), 0.0f);
  mapping.get("y", m_col.m_bbox.get_top(), 0.0f);
  mapping.get("width" , w, 32.0f);
  mapping.get("height", h, 32.0f);
  m_col.m_bbox.set_size(w, h);

  mapping.get("radius", m_radius, 1.0f);
  mapping.get("sample", m_sample, "");
  mapping.get("volume", m_volume, 1.0f);

  m_radius_in_px = m_radius*32.0f;

  prepare_sound_source();
}

AmbientSound::AmbientSound(const Vector& pos, float radius, float vol, const std::string& file) :
  m_sample(file),
  m_sound_source(),
  m_radius(radius),
  m_radius_in_px(m_radius*32.0f),
  m_volume(vol),
  m_has_played_sound(false)
{
  m_col.m_group = COLGROUP_DISABLED;

  m_col.m_bbox.set_pos(pos);
  m_col.m_bbox.set_size(32, 32);

  prepare_sound_source();
}

AmbientSound::~AmbientSound()
{
  stop_looping_sounds();
}

ObjectSettings
AmbientSound::get_settings()
{
  ObjectSettings result = MovingObject::get_settings();

  result.add_sound(_("Sound"), &m_sample, "sample");
  result.add_float(_("Radius (in tiles)"), &m_radius, "radius");
  result.add_float(_("Volume"), &m_volume, "volume");

  result.reorder({"sample", "radius", "volume", "width", "height", "name", "x", "y"});

  return result;
}

HitResponse
AmbientSound::collision(MovingObject& other, const CollisionHit& hit_)
{
  return ABORT_MOVE;
}

void
AmbientSound::draw(DrawingContext& context)
{
  if (Editor::is_active())
    context.color().draw_filled_rect(m_col.m_bbox, Color(0.0f, 0.0f, 1.0f, 0.6f),
                                     0.0f, LAYER_OBJECTS);
}

void
AmbientSound::stop_looping_sounds()
{
  if (m_sound_source)
    m_sound_source->stop(false);
}

void
AmbientSound::play_looping_sounds()
{
  if (Editor::is_active())
    return;

  m_sound_source->play();
}

void
AmbientSound::update(float dt_sec)
{
  const Player* const nearest_player = Sector::get().get_nearest_player(get_bbox().get_middle());
  if (!nearest_player)
    return;
  const Rectf& player_bbox = nearest_player->get_bbox();
  const Vector player_center = player_bbox.get_middle();

  if (get_bbox().overlaps(player_bbox))
    m_sound_source->set_gain(m_volume);
  else
  {
    float player_distance = m_radius+1;
    if (player_center.x >= get_bbox().get_left() && player_center.x <= get_bbox().get_right())
      player_distance = player_center.y < get_bbox().get_top() ? get_bbox().get_top() - player_center.y : player_center.y - get_bbox().get_bottom();
    else if (player_center.y >= get_bbox().get_top() && player_center.y <= get_bbox().get_bottom())
      player_distance = player_center.x < get_bbox().get_left() ? get_bbox().get_left() - player_center.x : player_center.x - get_bbox().get_right();
    else if (player_center.x <= get_bbox().get_left() && player_center.y <= get_bbox().get_top())
      player_distance = glm::distance(player_center, get_bbox().p1());
    else if (player_center.x >= get_bbox().get_right() && player_center.y <= get_bbox().get_top())
      player_distance = glm::distance(player_center, get_bbox().p1() + Vector(get_bbox().get_width(), 0));
    else if (player_center.x <= get_bbox().get_left() && player_center.y >= get_bbox().get_bottom())
      player_distance = glm::distance(player_center, get_bbox().p1() + Vector(0, get_bbox().get_height()));
    else if (player_center.x >= get_bbox().get_right() && player_center.y >= get_bbox().get_bottom())
      player_distance = glm::distance(player_center, get_bbox().p2());
    m_sound_source->set_gain(std::max(m_radius_in_px - player_distance, 0.0f) / m_radius_in_px * m_volume);
  }

  if (!m_has_played_sound)
  {
    m_sound_source->play();
    m_has_played_sound = true;
  }
}

void
AmbientSound::prepare_sound_source()
{
  if (Editor::is_active())
    return;

  if (m_sample.empty())
  {
    remove_me();
    return;
  }

  try
  {
    m_sound_source = SoundManager::current()->create_sound_source(m_sample);
    if (!m_sound_source)
      throw std::runtime_error("file not found");

    m_sound_source->set_gain(0);
    m_sound_source->set_looping(true);
    m_sound_source->set_relative(true);
  }
  catch(const std::exception& e)
  {
    log_warning << "Couldn't load '" << m_sample << "': " << e.what() << std::endl;
    m_sound_source.reset();
    remove_me();
  }
}


void
AmbientSound::register_class(ssq::VM& vm)
{
  ssq::Class cls = vm.addAbstractClass<AmbientSound>("AmbientSound", vm.findClass("MovingObject"));

  cls.addFunc("get_pos_x", &MovingObject::get_x);
  cls.addFunc("get_pos_y", &MovingObject::get_y);
}

/* EOF */
