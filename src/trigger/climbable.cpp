//  SuperTux - Climbable area
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

#include "trigger/climbable.hpp"

#include "editor/editor.hpp"
#include "object/player.hpp"
#include "supertux/debug.hpp"
#include "supertux/resources.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {
const float GRACE_DX = 8; // How far off may the player's bounding-box be x-wise.
const float GRACE_DY = 8; // How far off may the player's bounding-box be y-wise.
const float ACTIVATE_TRY_FOR = 1; // How long to try correcting mis-alignment of player and climbable before giving up.
const float POSITION_FIX_AX = 30; // X-wise acceleration applied to player when trying to align player and Climbable.
const float POSITION_FIX_AY = 50; // Y-wise acceleration applied to player when trying to align player and Climbable.
}

Climbable::Climbable(const ReaderMapping& reader) :
  Trigger(reader),
  climbed_by(),
  trying_to_climb(),
  message()
{
  reader.get("message", message);
}

Climbable::~Climbable()
{
  for (auto* player : climbed_by)
    player->stop_climbing(*this);

  climbed_by.clear();
  trying_to_climb.clear();
}

ObjectSettings
Climbable::get_settings()
{
  ObjectSettings result = Trigger::get_settings();

  result.add_translatable_text(_("Message"), &message, "message");

  result.reorder({"message", "width", "height", "x", "y"});

  return result;
}

void
Climbable::update(float dt_sec)
{
  Trigger::update(dt_sec);

  auto it = climbed_by.begin();
  while (it != climbed_by.end())
  {
    if (!may_climb(**it))
    {
      (*it)->stop_climbing(*this);
      it = climbed_by.erase(it);
      continue;
    }
    it++;
  }
  auto it2 = trying_to_climb.begin();
  while (it2 != trying_to_climb.end())
  {
    if (it2->m_activate_try_timer->started())
    {
      // The "-20" to y velocity prevents Tux from walking in place on the ground for horizonal adjustments.
      if (it2->m_player->get_bbox().get_left() < m_col.m_bbox.get_left() - GRACE_DX) it2->m_player->add_velocity(Vector(POSITION_FIX_AX,-20));
      if (it2->m_player->get_bbox().get_right() > m_col.m_bbox.get_right() + GRACE_DX) it2->m_player->add_velocity(Vector(-POSITION_FIX_AX,-20));
      if (it2->m_player->get_bbox().get_top() < m_col.m_bbox.get_top() - GRACE_DY) it2->m_player->add_velocity(Vector(0,POSITION_FIX_AY));
      if (it2->m_player->get_bbox().get_bottom() > m_col.m_bbox.get_bottom() + GRACE_DY) it2->m_player->add_velocity(Vector(0,-POSITION_FIX_AY));
    }
    if (may_climb(*(it2->m_player)))
    {
      climbed_by.push_back(it2->m_player);
      it2->m_player->start_climbing(*this);
      it2 = trying_to_climb.erase(it2);
      continue;
    }
    it2++;
  }
}

void
Climbable::draw(DrawingContext& context)
{
  if (!climbed_by.empty() && !message.empty()) {
    context.push_transform();
    context.set_translation(Vector(0, 0));
    context.transform().scale = 1.f;
    Vector pos = Vector(0, static_cast<float>(SCREEN_HEIGHT) / 2.0f - Resources::normal_font->get_height() / 2.0f);
    context.color().draw_center_text(Resources::normal_font, _(message), pos, LAYER_HUD, Climbable::text_color);
    context.pop_transform();
  }
  if (Editor::is_active() || g_debug.show_collision_rects) {
    context.color().draw_filled_rect(m_col.m_bbox, Color(1.0f, 1.0f, 0.0f, 0.6f),
                             0.0f, LAYER_OBJECTS);
  }
}

void
Climbable::event(Player& player, EventType type)
{
  if (type == EVENT_ACTIVATE || (type == EVENT_TOUCH && player.get_controller().hold(Control::UP))) {
    if (player.get_grabbed_object() == nullptr && !player.is_swimming()){
      auto it = std::find_if(trying_to_climb.begin(), trying_to_climb.end(),
        [&player](const ClimbPlayer& element)
        {
          return element.m_player == &player;
        });
      if (it == trying_to_climb.end()) {
        trying_to_climb.push_back(ClimbPlayer{&player, std::make_unique<Timer>()});
        it = trying_to_climb.begin() + (trying_to_climb.size() - 1);
      }
      if (!may_climb(player))
        it->m_activate_try_timer->start(ACTIVATE_TRY_FOR);
    }
  }

  if (type == EVENT_LOSETOUCH) {
    player.stop_climbing(*this);
    auto it = climbed_by.begin();
    while (it != climbed_by.end())
    {
      if (*it == &player)
        it = climbed_by.erase(it);
      else
        it++;
    }
    auto it2 = trying_to_climb.begin();
    while (it2 != trying_to_climb.end())
    {
      if (it2->m_player == &player)
        it2 = trying_to_climb.erase(it2);
      else
        it2++;
    }
  }
}

bool
Climbable::may_climb(const Player& player) const
{
  if (player.is_swimming()) return false;
  if (player.get_bbox().get_left() < m_col.m_bbox.get_left() - GRACE_DX) return false;
  if (player.get_bbox().get_right() > m_col.m_bbox.get_right() + GRACE_DX) return false;
  if (player.get_bbox().get_top() < m_col.m_bbox.get_top() - GRACE_DY) return false;
  if (player.get_bbox().get_bottom() > m_col.m_bbox.get_bottom() + GRACE_DY) return false;
  return true;
}

/* EOF */
