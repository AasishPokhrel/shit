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

#include "object/infoblock.hpp"

#include <algorithm>

#include "editor/editor.hpp"
#include "object/player.hpp"
#include "object/camera.hpp"
#include "supertux/info_box_line.hpp"
#include "supertux/sector.hpp"
#include "util/reader_mapping.hpp"
#include "video/drawing_context.hpp"

InfoBlock::InfoBlock(const ReaderMapping& mapping) :
  Block(mapping, "images/objects/bonus_block/infoblock.sprite"),
  m_message(),
  m_shown_pct(0),
  m_dest_pct(0),
  m_lines(),
  m_lines_height(0),
  m_frontcolor(0.6f, 0.7f, 0.8f, 0.5f),
  m_backcolor(0.f, 0.f, 0.f, 0.f),
  m_roundness(16.f),
  m_fadetransition(true),
  m_initial_y(0.0f)
{
  if (!mapping.get("message", m_message) && !(Editor::is_active()))
  {
    log_warning << "No message in InfoBlock" << std::endl;
  }
  std::vector<float> front_color;
  if (mapping.get("frontcolor", front_color))
  {
    m_frontcolor = Color(front_color);
  }
  std::vector<float> back_color;
  if (mapping.get("backcolor", back_color))
  {
    m_backcolor = Color(back_color);
  }
  mapping.get("roundness", m_roundness, 0.f);
  mapping.get("fadetransition", m_fadetransition, true);
  //stopped = false;
  //ringing = new AmbientSound(get_pos(), 0.5, 300, 1, "sounds/phone.wav");
  //Sector::get().add_object(ringing);

  // Split text string lines into a vector
  m_lines = InfoBoxLine::split(m_message, 400);
  for (const auto& line : m_lines) m_lines_height += line->get_height();
}

InfoBlock::~InfoBlock()
{
}

ObjectSettings
InfoBlock::get_settings()
{
  ObjectSettings result = Block::get_settings();

  result.add_multiline_translatable_text(_("Message"), &m_message, "message");

  result.add_color(_("Front Color"), &m_frontcolor, "frontcolor", Color(0.6f, 0.7f, 0.8f, 0.5f));

  result.add_color(_("Back Color"), &m_backcolor, "backcolor", Color(0.f, 0.f, 0.f, 0.f));

  result.add_float(_("Roundness"), &m_roundness, "roundness", 0.f);

  result.add_bool(_("Fade Transition"), &m_fadetransition, "fadetransition", true);

  result.reorder({ "message", "frontcolor", "backcolor", "roundness", "fadetransition", "x", "y" });

  return result;
}

void
InfoBlock::hit(Player& player)
{
  start_bounce(&player);

  //if (!stopped) {
  //  ringing->remove_me();
  //  stopped = true;
  //}

  if (m_dest_pct != 1) {

    // first hide all other InfoBlocks' messages in same sector
    for (auto& block : Sector::get().get_objects_by_type<InfoBlock>())
    {
      if (&block != this)
      {
        block.hide_message();
      }
    }

    Camera& cam = Sector::get().get_singleton_by_type<Camera>();
    if (m_original_y - m_lines_height - 10.f < cam.get_translation().y)
      m_initial_y = cam.get_translation().y + 10.0f;
    else
      m_initial_y = m_original_y - m_lines_height - 10.f;

    show_message();

  } else {
    hide_message();
  }
}

HitResponse
InfoBlock::collision(MovingObject& other, const CollisionHit& hit_)
{
  auto player = dynamic_cast<Player*> (&other);
  if (player && player->m_does_buttjump)
  {
    InfoBlock::hit(*player);
  }
  return Block::collision(other, hit_);
}

Player*
InfoBlock::get_nearest_player() const
{
  return Sector::get().get_nearest_player (m_col.m_bbox);
}

void
InfoBlock::update(float dt_sec)
{
  Block::update(dt_sec);

  if (dt_sec == 0) return;

  // hide message if player is too far away
  if (m_dest_pct > 0) {
    if (auto* player = get_nearest_player()) {
      Vector p1 = m_col.m_bbox.get_middle();
      Vector p2 = player->get_bbox().get_middle();
      Vector dist = (p2 - p1);
      float d = glm::length(dist);
      if (d > 128) m_dest_pct = 0;
    }
  }

  // handle soft fade-in and fade-out
  if (m_shown_pct != m_dest_pct) {
    float transitionspeed = m_fadetransition ? 1.f : 2.5f;
    if (m_dest_pct > m_shown_pct) m_shown_pct = std::min(m_shown_pct + 2 * dt_sec * transitionspeed, m_dest_pct);
    if (m_dest_pct < m_shown_pct) m_shown_pct = std::max(m_shown_pct - 2 * dt_sec * transitionspeed, m_dest_pct);
  }
}

void
InfoBlock::draw(DrawingContext& context)
{
  Block::draw(context);

  if (m_shown_pct <= 0) return;

  context.push_transform();
  //context.set_translation(Vector(0, 0));
  if (m_fadetransition)
    context.set_alpha(m_shown_pct);

  //float x1 = SCREEN_WIDTH/2-200;
  //float y1 = SCREEN_HEIGHT/2-200;
  float border = 8;
  float width = 400; // this is the text width only
  float height = m_lines_height; // this is the text height only
  float x1 = (m_col.m_bbox.get_left() + m_col.m_bbox.get_right())/2 - width/2;
  float x2 = (m_col.m_bbox.get_left() + m_col.m_bbox.get_right())/2 + width/2;
  float y1 = m_initial_y;

  if (x1 < 0) {
    x1 = 0;
    x2 = width;
  }

  if (x2 > Sector::get().get_width()) {
    x2 = Sector::get().get_width();
    x1 = x2 - width;
  }

  float growposx = (x1 - border) + (((width + 2 * border) / 2) - (((width + 2 * border) / 2) * m_shown_pct));
  float growposy = (y1 - border) + ((height + 2 * border - 4) / 2) - (((height + 2 * border - 4) / 2) * m_shown_pct);

  // lines_height includes one ITEMS_SPACE too much, so the bottom border is reduced by 4px
  context.color().draw_filled_rect(Rectf(Vector(m_fadetransition ? x1 - border : growposx,
    m_fadetransition ? y1 - border : growposy),
    Sizef(width + 2 * border, height + 2 * border - 4) * (m_fadetransition ? 1.f : m_shown_pct)),
    m_frontcolor, m_roundness, LAYER_FOREGROUND1 - 1);

  context.color().draw_filled_rect(Rectf(Vector((m_fadetransition ? x1 - border : growposx) - 4.f,
    (m_fadetransition ? y1 - border : growposy) - 4.f),
    Sizef(8.f, 8.f) + (Sizef((width + 2 * border), (height + 2 * border - 4)) * (m_fadetransition ? 1.f : m_shown_pct))),
    m_backcolor, m_roundness + 4.f, LAYER_FOREGROUND1 - 2);

  float y = y1;
  for (size_t i = 0; i < m_lines.size(); ++i) {
    if (y >= y1 + height) {
      //log_warning << "Too many lines of text in InfoBlock" << std::endl;
      //dest_pct = 0;
      //shown_pct = 0;
      break;
    }

    if (m_fadetransition || m_shown_pct >= 1.f)
    {
      m_lines[i]->draw(context, Rectf(x1, y, x2, y), LAYER_FOREGROUND1);
      y += m_lines[i]->get_height();
    }
  }

  context.pop_transform();
}

void
InfoBlock::show_message()
{
  m_dest_pct = 1;
}

void
InfoBlock::hide_message()
{
  m_dest_pct = 0;
}

/* EOF */
